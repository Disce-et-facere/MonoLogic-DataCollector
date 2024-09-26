#include "include/usb.h"
#include <stdbool.h>

#ifndef _wifi_h
#define _wifi_h

bool wifiInitStation(settings_t *settings);
void printMAC(void);

#endif // !_wifi_h
