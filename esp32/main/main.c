#include "esp_err.h"
#include "esp_log.h"
#include "esp_system.h"
#include "freertos/idf_additions.h"
#include "freertos/projdefs.h"
#include "include/dht11.h"
#include "include/https.h"
#include "include/usb.h"
#include "include/wifi.h"
#include "nvs.h"
#include "nvs_flash.h"
#include "portmacro.h"
#include <stdint.h>
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
  // NVS testing
  int32_t amountHTTPSSent = 0;
  const char *nvsTAG = "NVS";
  ESP_LOGI(nvsTAG, "Opening NVS");


  nvs_handle_t nvsHandle;
  ret = nvs_open("storage", NVS_READWRITE, &nvsHandle);
  if (ret == ESP_OK) {
    ret = nvs_get_i32(nvsHandle, "amountSent", &amountHTTPSSent);
    switch (ret) {
    case ESP_OK:
      ESP_LOGI(nvsTAG, "Read ok!");
      break;
    case ESP_ERR_NVS_NOT_FOUND:
      ESP_LOGI(nvsTAG, "Value not found!");
      break;
    default:
      printf("Error (%s) reading!\n", esp_err_to_name(ret));
      break;
    }

    amountHTTPSSent++;

    ret = nvs_set_i32(nvsHandle, "amountSent", amountHTTPSSent);
    if (ret != ESP_OK) {
      ESP_LOGI(nvsTAG, "set failed");
    }
    ret = nvs_commit(nvsHandle);
    if (ret != ESP_OK) {
      ESP_LOGI(nvsTAG, "commit failed");
    }

    nvs_close(nvsHandle);
  }

  ESP_LOGI(nvsTAG, "Value is %d", (int)amountHTTPSSent);
  */

  dht_t *dhtStructPtr = (dht_t *)malloc(sizeof(dht_t));
  ESP_ERROR_CHECK(dhtInit(dhtStructPtr));

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

  TaskHandle_t usbHandle = NULL;
  taskRet = xTaskCreate(&usbTask, "USB Task", 4096, NULL, 5, &usbHandle);
  if (taskRet == errCOULD_NOT_ALLOCATE_REQUIRED_MEMORY) {
    vTaskDelete(usbHandle);
    ESP_LOGE("USB", "Could not allocate memory for task");
    esp_restart();
  }
}
