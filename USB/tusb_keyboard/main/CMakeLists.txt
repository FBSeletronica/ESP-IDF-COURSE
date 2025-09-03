idf_component_register(
    SRCS "tusb_hid_example_main.c" "hid_keyboard.c"
    INCLUDE_DIRS "."
    PRIV_REQUIRES esp_driver_gpio
    )
