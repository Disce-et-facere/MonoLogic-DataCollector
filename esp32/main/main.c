/* WiFi station Example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/
#include "dht11.h"
#include "esp_log.h"
#include "freertos/idf_additions.h"
#include "mqtt.h"
#include "mqtt_client.h"
#include "nvs_flash.h"
#include "portmacro.h"
#include "wifi.h"

void DHT_task(void *pvParameter) {
  dht_t dhtStruct;
  dht_init(&dhtStruct);
  ESP_LOGI(DHTTAG, "Entering dht loop");
  while (true) {
    dht_err_t dhtStatus = dht_read(&dhtStruct);
    switch (dhtStatus) {
    case DHT_OK:
      ESP_LOGI(DHTTAG, "read ok");
      /*ESP_LOGI(DHTTAG, "Temperature: %d.%d\nHumidity: %d.%d%%",
               dhtStruct.temperature.integral, dhtStruct.temperature.decimal,
               dhtStruct.humidity.integral, dhtStruct.humidity.decimal);
        */
      ESP_LOGI(DHTTAG, "Temp: %d\tHumid: %d", dhtStruct.tempSimple,
               dhtStruct.humidSimple);
      break;
    case DHT_TIMEOUT_FAIL:
      ESP_LOGE(DHTTAG, "Dht timeout error");
      break;
    case DHT_FAIL:
      ESP_LOGE(DHTTAG, "Unk error in dht");
      break;
    case DHT_READ_TOO_EARLY:
      //    ESP_LOGI(DHTTAG, "DHT read too early, using old values");
      break;
    case DHT_CHECKSUM_FAIL:
      ESP_LOGE(DHTTAG, "Dht checksum error");
      break;
    }
    vTaskDelay(2000 / portTICK_PERIOD_MS);
  }
}

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
    ESP_LOGI(MQTTTAG, "MQTT started");
    esp_mqtt_client_publish(mqttClient, "/idfpye/qos1", "publish", 0, 1, 0);
    esp_mqtt_client_enqueue(mqttClient, "/idfpye/qos1", "enqueue", 0, 1, 0,
                            false);
  }
  xTaskCreate(&DHT_task, "DHT task", 4096, NULL, 5, NULL);
}
