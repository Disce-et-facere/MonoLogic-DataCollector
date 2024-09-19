#ifndef __dht11_h
#define __dht11_h

#include "esp_err.h"
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
    int8_t integral;
    uint8_t decimal;
  } temperature;
  struct {
    int8_t integral;
    uint8_t decimal;
  } humidity;
} dht_t;

dht_err_t dht_init(dht_t *dht);
dht_err_t dht_read(dht_t *dht);

#endif // !__dht11_h
