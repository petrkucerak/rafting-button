idf_component_register(SRCS "espnow_utils.c"
                    INCLUDE_DIRS "."
                    REQUIRES esp_wifi peripheral distributed_protocols esp_timer)
