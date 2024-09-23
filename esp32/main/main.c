#include "dht11.h"
#include "esp_interface.h"
#include "esp_log.h"
#include "esp_system.h"
#include "freertos/idf_additions.h"
#include "freertos/projdefs.h"
#include "https.h"
#include "mqtt.h"
#include "nvs_flash.h"
#include "portmacro.h"
#include "wifi.h"
#include <stdlib.h>

void app_main(void) {
  // Initialize NVS
  esp_err_t ret = nvs_flash_init();
  if (ret == ESP_ERR_NVS_NO_FREE_PAGES ||
      ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
    ESP_ERROR_CHECK(nvs_flash_erase());
    ret = nvs_flash_init();
  }
  ESP_ERROR_CHECK(ret);

  /*
  dht_t *dhtStructPtr = (dht_t *)malloc(sizeof(dht_t));
  dhtInit(dhtStructPtr);

  TaskHandle_t mqttHandle = NULL;
  BaseType_t taskRet =
      xTaskCreate(&httpsTask, "http task", 8192, dhtStructPtr, 5, &mqttHandle);
  if (taskRet == errCOULD_NOT_ALLOCATE_REQUIRED_MEMORY) {
    vTaskDelete(mqttHandle);
    ESP_LOGE(MQTTTAG, "Could not allocate memory for task");
    esp_restart();
  }*/

  /*TaskHandle_t dhtHandle = NULL;
  taskRet =
      xTaskCreate(&dhtTask, "DHT task", 4096, dhtStructPtr, 5, &dhtHandle);
  if (taskRet == errCOULD_NOT_ALLOCATE_REQUIRED_MEMORY) {
    vTaskDelete(dhtHandle);
    ESP_LOGE(DHTTAG, "Could not allocate memory for task");
    esp_restart();
  }*/
  if (wifi_init_sta()) {
    printMAC();
    https_with_url();
  }
}
