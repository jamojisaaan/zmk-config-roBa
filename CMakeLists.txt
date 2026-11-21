if(CONFIG_ROBA_FORCE_BT_NAME)
  zephyr_library()
  zephyr_library_sources(src/force_bt_name.c)
endif()
