#include "include/usb.h"
#include "freertos/idf_additions.h"
#include <stdint.h>
#include <stdio.h>

static void interpretInput(char *str, settings_t *settings) {
  switch (str[0]) {
  case 's':
    snprintf(settings->SSID, bufferSize, "%s", (str + 1));
    break;
  case 'p':
    snprintf(settings->password, bufferSize, "%s", (str + 1));
    break;
  case 'n':
    snprintf(settings->name, bufferSize, "%s", (str + 1));
    break;
  }
}

void usbTask(void *pvParameter) {
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

  settings_t settings = {0};

  char buffer[bufferSize];
  while (1) {
    if (fgets(buffer, sizeof(buffer), stdin) != NULL) {
      interpretInput(buffer, &settings);
    }
    printf("SSID: %s\nPW: %s\nName: %s\n", settings.SSID, settings.password,
           settings.name);
    vTaskDelay((2 * 1000) / portTICK_PERIOD_MS);
  }
}
