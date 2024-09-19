#include "dht11.h"
#include "driver/gpio.h"
#include "esp_err.h"
#include "esp_log.h"
#include "esp_timer.h"
#include "hal/gpio_types.h"
#include "rom/ets_sys.h"
#include "soc/gpio_num.h"

dht_err_t dht_init(dht_t *dht) {
  dht->gpio = GPIO_NUM_19;
  dht->lastRead = -2000000;
  return ESP_OK;
}

static int16_t waitForResponse(gpio_num_t gpio, int16_t microsToWait,
                               uint8_t level) {
  int16_t microsWaited = 0;
  while (gpio_get_level(gpio) == level) {
    microsWaited++;
    if (microsWaited > microsToWait) {
      return -1;
    }
    ets_delay_us(1);
  }
  return microsWaited;
}

dht_err_t dht_read(dht_t *dht) {
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

  ESP_LOGI(DHTTAG, "Starting DHT");
  gpio_set_direction(dht->gpio, GPIO_MODE_OUTPUT);
  gpio_set_level(dht->gpio, 0);
  ets_delay_us(20000);
  gpio_set_level(dht->gpio, 1);
  ets_delay_us(40);
  gpio_set_direction(dht->gpio, GPIO_MODE_INPUT);

  ESP_LOGI(DHTTAG, "waiting For response");

  if (waitForResponse(dht->gpio, 80, 0) == -1) {
    ESP_LOGE(DHTTAG, "first wait error");
    return DHT_TIMEOUT_FAIL;
  }
  if (waitForResponse(dht->gpio, 80, 1) == -1) {
    ESP_LOGE(DHTTAG, "second wait error");
    return DHT_TIMEOUT_FAIL;
  }

  uint8_t incomingData[5] = {0};

  for (uint8_t i = 0; i < 40; i++) {
    if (waitForResponse(dht->gpio, 50, 0) == -1) {
      return DHT_TIMEOUT_FAIL;
    }
    int16_t bitData = waitForResponse(dht->gpio, 70, 1);
    if (bitData == -1) {
      return DHT_FAIL;
    }
    if (bitData > 27) {
      if (i < 8) {
        // humidity integral
        incomingData[0] |= (1 << (7 - i));
      } else if (i < 16) {
        // humidity decimal
        incomingData[1] |= (1 << (7 - (i - 8)));
      } else if (i < 24) {
        // temperature integral
        incomingData[2] |= (1 << (7 - (i - 16)));
      } else if (i < 32) {
        // temperature decimal
        incomingData[3] |= (1 << (7 - (i - 24)));
      } else {
        // checksum
        incomingData[4] |= (1 << (7 - (i - 32)));
      }
    }
  }

  if (incomingData[4] !=
      (incomingData[0] + incomingData[1] + incomingData[2] + incomingData[3])) {
    return DHT_CHECKSUM_FAIL;
  }

  dht->temperature.decimal = incomingData[0];
  dht->temperature.integral = incomingData[1];
  dht->humidity.integral = incomingData[2];
  dht->humidity.decimal = incomingData[3];

  return DHT_OK;
}
