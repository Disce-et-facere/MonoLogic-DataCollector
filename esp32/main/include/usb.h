#ifndef __usb_h
#define __usb_h

#include "include/dht11.h"

typedef enum interpret_ret {
  INTERP_OK_SSID,
  INTERP_OK_PW,
  INTERP_OK_NAME,
  INTERP_RESTART,
  INTERP_COMMIT,
  INTERP_BAD_DATA,
  INTERP_REQ_PRINT,
  INTERP_NO_MUTEX,
} interpret_ret;

void usbTask(void *pvParameter);
esp_err_t settingsInit(settings_t *settings);

#endif // !__usb_h
