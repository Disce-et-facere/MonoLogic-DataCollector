#ifndef __dht11_h
#define __dht11_h

#include "freertos/idf_additions.h"
#include "soc/gpio_num.h"
#include <stdbool.h>
#include <stdint.h>

#define dhtTimeBetweenRead 5
#define bufferSize 64

typedef enum dht_err_t {
  DHT_OK,
  DHT_TIMEOUT_FAIL,
  DHT_BAD_DATA,
  DHT_READ_TOO_EARLY,
  DHT_CHECKSUM_FAIL,
  DHT_MUTEX_FAIL,
} dht_err_t;

typedef struct dhtValue {
  int8_t integer;
  int8_t decimal;
} dhtValue;

typedef struct dht_t {
  bool sent;
  gpio_num_t gpio;
  int64_t lastRead;
  dhtValue temperature;
  dhtValue humidity;
} dht_t;

typedef struct settings_t {
  char name[bufferSize];
  char SSID[bufferSize];
  char password[bufferSize];
  dht_t *dht;
  SemaphoreHandle_t mutex;
} settings_t;

dht_err_t dhtInit(dht_t *dht);
dht_err_t dhtRead(settings_t *settings);
void dhtTask(void *pvParameter);
float getDHTValue(dhtValue *dhtValue);

#endif // !__dht11_h
