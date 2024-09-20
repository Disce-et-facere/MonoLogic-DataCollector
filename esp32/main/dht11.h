#ifndef __dht11_h
#define __dht11_h

#include "soc/gpio_num.h"
#include <stdint.h>

static const char *DHTTAG = "DHT";

typedef enum dht_err_t {
  DHT_OK,
  DHT_TIMEOUT_FAIL,
  DHT_FAIL,
  DHT_READ_TOO_EARLY,
  DHT_CHECKSUM_FAIL
} dht_err_t;

typedef struct dht_t {
  gpio_num_t gpio;
  int64_t lastRead;
  struct {
    int8_t integer;
    uint8_t decimal;
  } temperature;
  struct {
    int8_t integer;
    uint8_t decimal;
  } humidity;
} dht_t;

dht_err_t dhtInit(dht_t *dht);
dht_err_t dhtRead(dht_t *dht);
void dhtTask(void *pvParameter);

#endif // !__dht11_h
