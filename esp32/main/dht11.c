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

dht_err_t dhtInit(dht_t *dht) {
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
  ESP_LOGI(DHTTAG, "Waking DHT");
  gpio_set_direction(dht->gpio, GPIO_MODE_OUTPUT);
  gpio_set_level(dht->gpio, 0);
  ets_delay_us(20 * 1000);
  gpio_set_level(dht->gpio, 1);
  ets_delay_us(40);
  gpio_set_direction(dht->gpio, GPIO_MODE_INPUT);
}

dht_err_t dhtRead(dht_t *dht) {
  int64_t curTime = esp_timer_get_time();
  if (curTime < 2000000) {
    ets_delay_us(2000000 - curTime);
    curTime = esp_timer_get_time();
  }
  if (dht->lastRead > curTime - 2000000) {
    // DHT not ready, reuse old data
    return DHT_READ_TOO_EARLY;
  }
  dht->lastRead = curTime;

  wakeDHT(dht);

  if (waitForResponse(dht->gpio, 80, 0) == -1) {
    ESP_LOGE(DHTTAG, "first wait error");
    return DHT_TIMEOUT_FAIL;
  }
  if (waitForResponse(dht->gpio, 80, 1) == -1) {
    ESP_LOGE(DHTTAG, "second wait error");
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
  }

  dht->temperature.integer = (uint8_t)incomingData[2];
  dht->temperature.decimal = incomingData[3];
  dht->humidity.integer = (uint8_t)incomingData[0];
  dht->humidity.decimal = incomingData[1];
  dht->sent = false;

  return DHT_OK;
}

float getDHTValue(dhtValue *dhtValue) {
  return dhtValue->integer + ((float)dhtValue->decimal / 10);
}

void dhtTask(void *pvParameter) {
  dht_t *dhtStructPtr = (dht_t *)pvParameter;
  ESP_LOGI(DHTTAG, "Entering dht loop");
  while (true) {
    dht_err_t dhtStatus = dhtRead(dhtStructPtr);
    switch (dhtStatus) {
    case DHT_OK:
      ESP_LOGI(DHTTAG, "read ok");
      ESP_LOGI(DHTTAG, "Temp: %.1f\tHumidity: %1.f",
               getDHTValue(&dhtStructPtr->temperature),
               getDHTValue(&dhtStructPtr->humidity));
      break;
    case DHT_TIMEOUT_FAIL:
      ESP_LOGE(DHTTAG, "Dht timeout error");
      break;
    case DHT_FAIL:
      ESP_LOGE(DHTTAG, "Unk error in dht");
      break;
    case DHT_READ_TOO_EARLY:
      ESP_LOGI(DHTTAG, "DHT read too early, using old values");
      break;
    case DHT_CHECKSUM_FAIL:
      ESP_LOGE(DHTTAG, "Dht checksum error");
      break;
    }
    vTaskDelay((dhtTimeBetweenRead * 1000) / portTICK_PERIOD_MS);
  }
}
