#include "mqtt.h"
#include "esp_event.h"
#include "esp_event_base.h"
#include "esp_log.h"
#include "mqtt_client.h"
#include "nvs_flash.h"

static void mqtt_event_handler(void *arg, esp_event_base_t event_base,
                               int32_t event_id, void *event_data) {
  switch (event_id) {
  case MQTT_EVENT_CONNECTED: {

    break;
  }
  case MQTT_EVENT_DISCONNECTED: {
    break;
  }
  }
}
