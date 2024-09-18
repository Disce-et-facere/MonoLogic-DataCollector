/* WiFi station Example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/
#include "esp_log.h"
#include "mqtt.h"
#include "mqtt_client.h"
#include "nvs_flash.h"
#include "wifi.h"

void app_main(void) {
  // Initialize NVS
  esp_err_t ret = nvs_flash_init();
  if (ret == ESP_ERR_NVS_NO_FREE_PAGES ||
      ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
    ESP_ERROR_CHECK(nvs_flash_erase());
    ret = nvs_flash_init();
  }
  ESP_ERROR_CHECK(ret);

  ESP_LOGI(WIFITAG, "ESP_WIFI_MODE_STA");
  if (wifi_init_sta()) {
    ESP_LOGI(WIFITAG, "Connected to wifi successfully");
    ESP_LOGI(MQTTTAG, "MQTT starting");
    esp_mqtt_client_handle_t mqttClient = mqtt_init();
    ESP_LOGI(MQTTTAG, "MQTT started succesfully");
    esp_mqtt_client_publish(mqttClient, "/idfpye/qos1", "publish", 0, 1, 0);
    esp_mqtt_client_enqueue(mqttClient, "/idfpye/qos1", "enqueue", 0, 1, 0,
                            false);
  }
}
