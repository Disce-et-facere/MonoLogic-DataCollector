#ifndef __mqtt_h
#define __mqtt_h

#include "mqtt_client.h"

static const char *MQTTTAG = "Mqtt status";
esp_mqtt_client_handle_t mqtt_init(void);

#endif // !__mqtt_h
