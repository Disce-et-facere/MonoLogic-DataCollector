#include <stdbool.h>

#ifndef _wifi_h
#define _wifi_h

static const char *WIFITAG = "wifi station";

bool wifiInitStation(void);
void printMAC(void);

#endif // !_wifi_h
