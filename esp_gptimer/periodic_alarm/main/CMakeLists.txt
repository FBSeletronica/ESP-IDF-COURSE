idf_component_register(SRCS "periodic_alarm.c"
                    INCLUDE_DIRS "."
                    PRIV_REQUIRES esp_driver_gpio esp_driver_gptimer)
