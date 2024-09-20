/* WiFi station Example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/
#include "dht11.h"
#include "esp_log.h"
#include "freertos/idf_additions.h"
#include "freertos/projdefs.h"
#include "mqtt.h"
#include "mqtt_client.h"
#include "nvs_flash.h"
#include "portmacro.h"
#include "wifi.h"
#include <stdlib.h>

dht_t *dhtStructPtr;

void dhtTask(void *pvParameter) {
  /*dht_t dhtStruct;*/
  /*dhtInit(&dhtStruct);*/
  dhtInit(dhtStructPtr);
  dht_t *dhtStruct = dhtStructPtr;
  ESP_LOGI(DHTTAG, "Entering dht loop");
  while (true) {
    /*dht_err_t dhtStatus = dhtRead(&dhtStruct);*/
    dht_err_t dhtStatus = dhtRead(dhtStructPtr);
    switch (dhtStatus) {
    case DHT_OK:
      ESP_LOGI(DHTTAG, "read ok");
      ESP_LOGI(DHTTAG, "Temperature: %d.%d Humidity: %d.%d%%",
               dhtStruct->temperature.integer, dhtStruct->temperature.decimal,
               dhtStruct->humidity.integer, dhtStruct->humidity.decimal);
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

void mqttTask(void *pvParameter) {
  ESP_LOGI(WIFITAG, "ESP_WIFI_MODE_STA");
  if (wifi_init_sta()) {
    ESP_LOGI(WIFITAG, "Connected to wifi successfully");
    ESP_LOGI(MQTTTAG, "MQTT starting");
    esp_mqtt_client_handle_t mqttClient = mqtt_init();
    ESP_LOGI(MQTTTAG, "MQTT started");
    esp_mqtt_client_publish(mqttClient, "/idfpye/qos1", "publish", 0, 1, 0);
    esp_mqtt_client_enqueue(mqttClient, "/idfpye/qos1", "enqueue", 0, 1, 0,
                            false);
    char buff[100];
    while (true) {
      sprintf(buff, "{ \"Temperature\":%d.%d, \"Humidity\":%d.%d }",
              dhtStructPtr->temperature.integer,
              dhtStructPtr->temperature.decimal, dhtStructPtr->humidity.integer,
              dhtStructPtr->humidity.decimal);
      esp_mqtt_client_enqueue(mqttClient, "/idfpye/qos1", buff, 0, 1, 0, false);
      ESP_LOGI(MQTTTAG, "read temp %d.%d", dhtStructPtr->temperature.integer,
               dhtStructPtr->temperature.decimal);
      vTaskDelay(2000 / portTICK_PERIOD_MS);
    }
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

  dhtStructPtr = (dht_t *)malloc(sizeof(dht_t));

  TaskHandle_t dhtHandle = NULL;
  BaseType_t taskRet =
      xTaskCreate(&dhtTask, "DHT task", 4096, NULL, 5, &dhtHandle);
  if (taskRet == errCOULD_NOT_ALLOCATE_REQUIRED_MEMORY) {
    vTaskDelete(dhtHandle);
    ESP_LOGE(DHTTAG, "Could not allocate memory for task");
  }

  TaskHandle_t mqttHandle = NULL;
  taskRet = xTaskCreate(&mqttTask, "mqtt task", 4096, NULL, 5, &mqttHandle);
  if (taskRet == errCOULD_NOT_ALLOCATE_REQUIRED_MEMORY) {
    vTaskDelete(mqttHandle);
    ESP_LOGE(MQTTTAG, "Could not allocate memory for task");
  }
}
