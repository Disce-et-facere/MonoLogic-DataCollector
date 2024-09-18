idf_component_register(SRCS "main.c" "wifi.c"
                    INCLUDE_DIRS "."
                    PRIV_REQUIRES esp_wifi nvs_flash)
