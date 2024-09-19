idf_component_register(SRCS "main.c" "wifi.c" "mqtt.c" "dht11.c"
                    INCLUDE_DIRS "."
                    PRIV_REQUIRES esp_wifi 
                    nvs_flash 
                    mqtt
                    esp_driver_gpio)
