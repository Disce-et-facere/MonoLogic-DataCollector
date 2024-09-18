#include <stdbool.h>

#ifndef _wifi_h
#define _wifi_h
/* The event group allows multiple bits for each event, but we only care about
 * two events:
 * - we are connected to the AP with an IP
 * - we failed to connect after the maximum amount of retries */
#define WIFI_CONNECTED_BIT BIT0
#define WIFI_FAIL_BIT BIT1

static const char *WIFITAG = "wifi station";

/*static void event_handler(void *arg, esp_event_base_t event_base,
                          int32_t event_id, void *event_data);
                          */
bool wifi_init_sta(void);

#endif // !_wifi_h
