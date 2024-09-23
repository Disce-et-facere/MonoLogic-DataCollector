idf_component_register(SRCS "main.c" "wifi.c" "dht11.c" "https.c" "mqtt.c"
                    INCLUDE_DIRS "."
                    PRIV_REQUIRES esp_wifi 
                    nvs_flash 
                    esp_driver_gpio
                    mqtt
                    esp_http_client)
