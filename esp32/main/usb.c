#include "include/usb.h"
#include "esp_err.h"
#include "esp_log.h"
#include "esp_system.h"
#include "freertos/idf_additions.h"
#include "nvs.h"
#include "projdefs.h"
#include "tinyusb.h"
#include "tusb_cdc_acm.h"
#include <stdio.h>

static const char *ESP_ssid = "SSID";
static const char *ESP_pw = "PASSWORD";
static const char *ESP_Name = "NAME";

static interpret_ret interpretInput(char *str, settings_t *settings) {
  if (xSemaphoreTake(settings->mutex, (TickType_t)10)) {
    switch (str[0]) {
    case 's':
      snprintf(settings->SSID, bufferSize, "%s", (str + 1));
      xSemaphoreGive(settings->mutex);
      return INTERP_OK_SSID;
    case 'p':
      snprintf(settings->password, bufferSize, "%s", (str + 1));
      xSemaphoreGive(settings->mutex);
      return INTERP_OK_PW;
    case 'n':
      snprintf(settings->name, bufferSize, "%s", (str + 1));
      xSemaphoreGive(settings->mutex);
      return INTERP_OK_NAME;
    case 'r':
      xSemaphoreGive(settings->mutex);
      return INTERP_RESTART;
    case 'c':
      xSemaphoreGive(settings->mutex);
      return INTERP_COMMIT;
    case 'g':
      xSemaphoreGive(settings->mutex);
      return INTERP_REQ_PRINT;
    }
    xSemaphoreGive(settings->mutex);
    return INTERP_BAD_DATA;
  }
  return INTERP_NO_MUTEX;
}

static esp_err_t nvsRead(const char *key, char *buffer, size_t buffSize) {
  ESP_LOGI("NVS", "NVS starting");
  nvs_handle_t nvsHandle;
  esp_err_t ret = nvs_open("storage", NVS_READWRITE, &nvsHandle);
  if (ret == ESP_OK) {
    ret = nvs_get_str(nvsHandle, key, buffer, &buffSize);
    switch (ret) {
    case ESP_OK:
      ESP_LOGI("NVS", "Read good on key %s", key);
      break;
    case ESP_ERR_NVS_NOT_FOUND:
      ESP_LOGI("NVS", "Could not find key %s", key);
      buffer[0] = '\0';
      break;
    }
  }
  nvs_close(nvsHandle);
  return ret;
}

static esp_err_t nvsCommit(const char *key, char *buffer) {
  nvs_handle_t nvsHandle;
  esp_err_t ret = nvs_open("storage", NVS_READWRITE, &nvsHandle);
  if (ret == ESP_OK) {
    ret = nvs_set_str(nvsHandle, key, buffer);
    if (ret != ESP_OK) {
      ESP_LOGI("NVS", "%s set failed", key);
    }
    ret = nvs_commit(nvsHandle);
    if (ret != ESP_OK) {
      ESP_LOGI("NVS", "commit failed");
      nvs_close(nvsHandle);
      return ret;
    }
  }
  nvs_close(nvsHandle);
  return ret;
}

static void nvsCommitAll(settings_t *settings) {
  /*ESP_LOGI("NVS", "Committing...");*/
  esp_err_t ret = ESP_OK;
  ret = nvsCommit(ESP_ssid, settings->SSID);
  if (ret != ESP_OK) {
    return;
  }
  ret = nvsCommit(ESP_pw, settings->password);
  if (ret != ESP_OK) {
    return;
  }
  ret = nvsCommit(ESP_Name, settings->name);
  if (ret != ESP_OK) {
    return;
  }
  ESP_LOGI("NVS", "All commit good");
}

esp_err_t settingsInit(settings_t *settings) {
  if (settings == NULL) {
    return errCOULD_NOT_ALLOCATE_REQUIRED_MEMORY;
  }
  settings->mutex = xSemaphoreCreateMutex();
  if (settings->mutex == NULL) {
    ESP_LOGE("MUTEX", "Mutex creation failed");
    return errCOULD_NOT_ALLOCATE_REQUIRED_MEMORY;
  }
  (void)nvsRead(ESP_ssid, settings->SSID, sizeof(settings->SSID));
  (void)nvsRead(ESP_pw, settings->password, sizeof(settings->password));
  (void)nvsRead(ESP_Name, settings->name, sizeof(settings->name));
  return ESP_OK;
}

void usbTask(void *pvParameter) {
  settings_t *settingsPtr = (settings_t *)pvParameter;

  const tinyusb_config_t tusb_cfg = {
      .device_descriptor = NULL,
      .string_descriptor = NULL,
      .external_phy =
          false, // In the most cases you need to use a `false` value
      .configuration_descriptor = NULL,
  };

  ESP_ERROR_CHECK(tinyusb_driver_install(&tusb_cfg));

  tinyusb_config_cdcacm_t acm_cfg = {
      0}; // the configuration uses default values

  ESP_ERROR_CHECK(tusb_cdc_acm_init(&acm_cfg));

  char buffer[bufferSize];
  while (1) {
    if (fgets(buffer, sizeof(buffer), stdin) != NULL) {
      interpretInput(buffer, settingsPtr);
      switch (interpretInput(buffer, settingsPtr)) {
      case INTERP_OK_SSID:
        ESP_LOGI("USB", "SSID OK: %s", settingsPtr->SSID);
        break;
      case INTERP_OK_NAME:
        ESP_LOGI("USB", "NAME OK: %s", settingsPtr->name);
        break;
      case INTERP_OK_PW:
        ESP_LOGI("USB", "PW OK: %s", settingsPtr->password);
        break;
      case INTERP_COMMIT:
        nvsCommitAll(settingsPtr);
        break;
      case INTERP_RESTART:
        esp_restart();
        break;
      case INTERP_BAD_DATA:
        ESP_LOGI("USB", "Bad data");
        break;
      case INTERP_REQ_PRINT:
        ESP_LOGI("USB", "Current settings\nSSID: %s\nPW: %s\nName: %s",
                 settingsPtr->SSID, settingsPtr->password, settingsPtr->name);
        break;
      case INTERP_NO_MUTEX:
        ESP_LOGE("USB", "Could not get mutex");
        break;
      }
    }
    vTaskDelay((2 * 1000) / portTICK_PERIOD_MS);
  }
}
