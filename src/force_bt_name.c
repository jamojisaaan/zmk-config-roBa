/*
 * 起動後、Bluetooth 名をファームウェアの CONFIG_BT_DEVICE_NAME にそろえる。
 *
 * Zephyr は CONFIG_BT_DEVICE_NAME_DYNAMIC と CONFIG_BT_SETTINGS が有効だと、一度保存された
 * 名前（設定 "bt/name"）を起動時に読み込み、ファームウェアの既定名より優先する
 * （subsys/bluetooth/host/settings.c）。ファームウェアを書き換えても設定領域は残るため、
 * 古い名前（"roBa"）のままになる。そこで起動から少し待って、違っていれば bt_set_name() で
 * 上書きする（bt_set_name は同じ名前なら何もせず、違えば設定にも保存する）。
 * キーマップやペアリングなど、ほかの設定には触れない。
 */

#include <string.h>

#include <zephyr/bluetooth/bluetooth.h>
#include <zephyr/init.h>
#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>

LOG_MODULE_REGISTER(roba_bt_name, LOG_LEVEL_INF);

static void force_bt_name(struct k_work *work) {
    const char *current = bt_get_name();

    if (strcmp(current, CONFIG_BT_DEVICE_NAME) == 0) {
        return;
    }

    int err = bt_set_name(CONFIG_BT_DEVICE_NAME);
    LOG_INF("BT name \"%s\" -> \"%s\" (err %d)", current, CONFIG_BT_DEVICE_NAME, err);
}

static K_WORK_DELAYABLE_DEFINE(force_bt_name_work, force_bt_name);

static int roba_force_bt_name_init(void) {
    /* Bluetooth の初期化と設定の読み込みが終わってから行う */
    k_work_schedule(&force_bt_name_work, K_SECONDS(5));
    return 0;
}

SYS_INIT(roba_force_bt_name_init, APPLICATION, 99);
