#ifndef __mqtt_h
#define __mqtt_h

#include "dht11.h"
#include "mqtt_client.h"
#include "wifi.h"

static const char *MQTTTAG = "Mqtt status";
esp_mqtt_client_handle_t mqtt_init(void);
void mqttTask(void *pvParameter);

#endif // !__mqtt_h
