#include "esp_event.h"
#include "esp_event_base.h"
#include "esp_log.h"
#include "esp_wifi.h"
#include "freertos/event_groups.h"
#include "freertos/task.h"
#include "nvs_flash.h"
#include <string.h>

#ifndef _wifi_h
#define _wifi_h
/* The event group allows multiple bits for each event, but we only care about
 * two events:
 * - we are connected to the AP with an IP
 * - we failed to connect after the maximum amount of retries */
#define WIFI_CONNECTED_BIT BIT0
#define WIFI_FAIL_BIT BIT1

static const char *TAG = "wifi station";

/*static void event_handler(void *arg, esp_event_base_t event_base,
                          int32_t event_id, void *event_data);
                          */
bool wifi_init_sta(void);

#endif // !_wifi_h
