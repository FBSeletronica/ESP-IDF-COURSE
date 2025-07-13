idf_component_register(SRCS "one_shot_alarm.c"
                    INCLUDE_DIRS "."
                    PRIV_REQUIRES esp_driver_gpio esp_driver_gptimer)
