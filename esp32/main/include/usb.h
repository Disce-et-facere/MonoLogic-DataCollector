#ifndef __usb_h
#define __usb_h

#include "tinyusb.h"
#include "tusb_cdc_acm.h"
#include "tusb_console.h"

#define bufferSize 64

typedef struct settings_t {
  char name[bufferSize];
  char SSID[bufferSize];
  char password[bufferSize];
} settings_t;

void usbTask(void *pvParameter);

#endif // !__usb_h
