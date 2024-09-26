#ifndef __usb_h
#define __usb_h

#include "include/dht11.h"
#include "tinyusb.h"
#include "tusb_cdc_acm.h"
#include "tusb_console.h"

#define bufferSize 64

typedef enum interpret_ret {
  INTERP_OK_SSID,
  INTERP_OK_PW,
  INTERP_OK_NAME,
  INTERP_RESTART,
  INTERP_COMMIT,
  INTERPED_BAD_DATA,
  INTERP_REQ_PRINT,
} interpret_ret;

typedef struct settings_t {
  char name[bufferSize];
  char SSID[bufferSize];
  char password[bufferSize];
  dht_t *dht;
} settings_t;

void usbTask(void *pvParameter);
esp_err_t settingsInit(settings_t *settings);

#endif // !__usb_h
