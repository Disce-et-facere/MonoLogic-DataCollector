#include "esp_interface.h"
#include "esp_log.h"
#include "esp_system.h"
#include "freertos/idf_additions.h"
#include "freertos/projdefs.h"
#include "include/dht11.h"
#include "include/https.h"
#include "include/wifi.h"
#include "nvs_flash.h"
#include "portmacro.h"
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

  dht_t *dhtStructPtr = (dht_t *)malloc(sizeof(dht_t));
  dhtInit(dhtStructPtr);

  TaskHandle_t httpsHandle = NULL;
  BaseType_t taskRet =
      xTaskCreate(&httpsTask, "http task", 8192, dhtStructPtr, 5, &httpsHandle);
  if (taskRet == errCOULD_NOT_ALLOCATE_REQUIRED_MEMORY) {
    vTaskDelete(httpsHandle);
    ESP_LOGE("HTTP", "Could not allocate memory for task");
    esp_restart();
  }

  TaskHandle_t dhtHandle = NULL;
  taskRet =
      xTaskCreate(&dhtTask, "DHT task", 4096, dhtStructPtr, 5, &dhtHandle);
  if (taskRet == errCOULD_NOT_ALLOCATE_REQUIRED_MEMORY) {
    vTaskDelete(dhtHandle);
    ESP_LOGE("DHT", "Could not allocate memory for task");
    esp_restart();
  }
}
