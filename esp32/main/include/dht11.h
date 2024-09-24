#ifndef __dht11_h
#define __dht11_h

#include "soc/gpio_num.h"
#include <stdbool.h>
#include <stdint.h>

#define dhtTimeBetweenRead 20

typedef enum dht_err_t {
  DHT_OK,
  DHT_TIMEOUT_FAIL,
  DHT_FAIL,
  DHT_READ_TOO_EARLY,
  DHT_CHECKSUM_FAIL
} dht_err_t;

typedef struct dhtValue {
  int8_t integer;
  uint8_t decimal;
} dhtValue;

typedef struct dht_t {
  bool sent;
  gpio_num_t gpio;
  int64_t lastRead;
  dhtValue temperature;
  dhtValue humidity;
} dht_t;

dht_err_t dhtInit(dht_t *dht);
dht_err_t dhtRead(dht_t *dht);
void dhtTask(void *pvParameter);
float getDHTValue(dhtValue *dhtValue);

#endif // !__dht11_h
