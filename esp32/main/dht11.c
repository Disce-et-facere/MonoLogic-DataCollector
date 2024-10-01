#include "include/dht11.h"
#include "driver/gpio.h"
#include "esp_err.h"
#include "esp_log.h"
#include "esp_timer.h"
#include "freertos/idf_additions.h"
#include "hal/gpio_types.h"
#include "rom/ets_sys.h"
#include "soc/gpio_num.h"
#include <stdint.h>

static const char *DHTTAG = "DHT";

// TODO: Add gpio pin to settings
dht_err_t dhtInit(dht_t *dht) {
  if (dht == NULL) {
    return errCOULD_NOT_ALLOCATE_REQUIRED_MEMORY;
  }
  dht->gpio = GPIO_NUM_7;
  dht->lastRead = -2000000;
  return ESP_OK;
}

static int16_t waitForResponse(gpio_num_t gpio, int16_t microsToWait,
                               uint8_t level) {
  int16_t microsWaited = 0;
  while (gpio_get_level(gpio) == level) {
    if (microsWaited++ > microsToWait) {
      return -1;
    }
    ets_delay_us(1);
  }
  return microsWaited;
}

void wakeDHT(dht_t *dht) {
  gpio_set_direction(dht->gpio, GPIO_MODE_OUTPUT);
  gpio_set_level(dht->gpio, 0);
  ets_delay_us(20 * 1000);
  gpio_set_level(dht->gpio, 1);
  ets_delay_us(40);
  gpio_set_direction(dht->gpio, GPIO_MODE_INPUT);
}

dht_err_t dhtRead(settings_t *settings) {
  int64_t curTime = esp_timer_get_time();
  // NOTE: Only this func touches lastRead so no mutex needed
  dht_t *dht = settings->dht;
  if (dht->lastRead > curTime - 2000000) {
    return DHT_READ_TOO_EARLY;
  }
  dht->lastRead = curTime;

  wakeDHT(dht);

  if (waitForResponse(dht->gpio, 80, 0) == -1) {
    return DHT_TIMEOUT_FAIL;
  }
  if (waitForResponse(dht->gpio, 80, 1) == -1) {
    return DHT_TIMEOUT_FAIL;
  }

  int8_t incomingData[5] = {0};

  for (int i = 0; i < 40; i++) {
    if (waitForResponse(dht->gpio, 50, 0) == -1) {
      return DHT_TIMEOUT_FAIL;
    }
    if (waitForResponse(dht->gpio, 70, 1) > 28) {
      incomingData[i / 8] |= (1 << (7 - (i % 8)));
    }
  }

  if (incomingData[4] !=
      (incomingData[0] + incomingData[1] + incomingData[2] + incomingData[3])) {
    return DHT_CHECKSUM_FAIL;
  } else if ((incomingData[0] + incomingData[1] + incomingData[2] +
              incomingData[3] + incomingData[4]) == 0) {
    return DHT_BAD_DATA;
  }

  if (xSemaphoreTake(settings->mutex, (TickType_t)10)) {
    dht->temperature.integer = incomingData[2];
    dht->temperature.decimal = incomingData[3];
    dht->humidity.integer = incomingData[0];
    dht->humidity.decimal = incomingData[1];
    dht->sent = false;
    xSemaphoreGive(settings->mutex);
  } else {
    return DHT_MUTEX_FAIL;
  }

  return DHT_OK;
}

float getDHTValue(dhtValue *dhtValue) {
  return dhtValue->integer + ((float)dhtValue->decimal / 10);
}

void dhtTask(void *pvParameter) {
  settings_t *settings = (settings_t *)pvParameter;
  while (true) {
    dht_err_t dhtRet = dhtRead(settings);
    switch (dhtRet) {
    case DHT_OK:
      ESP_LOGI(DHTTAG, "Temp: %.1f\tHumidity: %.1f%%",
               getDHTValue(&settings->dht->temperature),
               getDHTValue(&settings->dht->humidity));
      break;
    case DHT_TIMEOUT_FAIL:
      ESP_LOGE(DHTTAG, "Timeout error");
      break;
    case DHT_BAD_DATA:
      ESP_LOGE(DHTTAG, "Unk error in dht");
      break;
    case DHT_READ_TOO_EARLY:
      ESP_LOGI(DHTTAG, "Read too early, using old values");
      break;
    case DHT_CHECKSUM_FAIL:
      ESP_LOGE(DHTTAG, "Checksum error");
      break;
    case DHT_MUTEX_FAIL:
      ESP_LOGE(DHTTAG, "Failed to aquire mutex");
      break;
    }
    vTaskDelay((dhtTimeBetweenRead * 1000) / portTICK_PERIOD_MS);
  }
}
