#include "mqtt.h"
#include "esp_event.h"
#include "esp_event_base.h"
#include "esp_log.h"
#include "mqtt_client.h"

static void mqtt_event_handler(void *arg, esp_event_base_t event_base,
                               int32_t event_id, void *event_data) {
  esp_mqtt_event_handle_t event = event_data;
  esp_mqtt_client_handle_t client = event->client;
  switch (event_id) {
  case MQTT_EVENT_CONNECTED: {
    ESP_LOGI(MQTTTAG, "connected to MQTT");
    esp_mqtt_client_subscribe(client, "/idfpye/qos1", 1);
    ESP_LOGI(MQTTTAG, "subscribing");
    break;
  }
  case MQTT_EVENT_DISCONNECTED: {
    ESP_LOGI(MQTTTAG, "disconnected from MQTT");
    break;
  }
  case MQTT_EVENT_SUBSCRIBED: {
    ESP_LOGI(MQTTTAG, "subscribed successfully");
    break;
  }
  case MQTT_EVENT_PUBLISHED: {
    ESP_LOGI(MQTTTAG, "message published successfully");
    break;
  }
  case MQTT_EVENT_DATA: {
    ESP_LOGI(MQTTTAG, "Got data from MQTT");
    printf("TOPIC=%.*s\r\n", event->topic_len, event->topic);
    printf("DATA=%.*s\r\n", event->data_len, event->data);
    break;
  }
  case MQTT_EVENT_ERROR: {
    ESP_LOGE(MQTTTAG, "Error happened not good");
    break;
  }
  case MQTT_EVENT_BEFORE_CONNECT: {
    ESP_LOGE(MQTTTAG, "MQTT event before connecting");
    break;
  }
  default: {
    ESP_LOGE(MQTTTAG, "Unknown MQTT eventID: %d", event->event_id);
    break;
  }
  }
}

esp_mqtt_client_handle_t mqtt_init(void) {
  const esp_mqtt_client_config_t mqtt_conf = {
      .broker.address.uri = "mqtt://test.mosquitto.org:1883",
  };
  esp_mqtt_client_handle_t client = esp_mqtt_client_init(&mqtt_conf);
  esp_mqtt_client_register_event(client, ESP_EVENT_ANY_ID, mqtt_event_handler,
                                 client);
  ESP_ERROR_CHECK(esp_mqtt_client_start(client));
  return client;
}

void mqttTask(void *pvParameter) {
  dht_t *dhtStructPtr = (dht_t *)pvParameter;
  ESP_LOGI(WIFITAG, "ESP_WIFI_MODE_STA");
  if (wifi_init_sta()) {
    ESP_LOGI(WIFITAG, "Connected to wifi successfully");
    ESP_LOGI(MQTTTAG, "MQTT starting");
    esp_mqtt_client_handle_t mqttClient = mqtt_init();
    ESP_LOGI(MQTTTAG, "MQTT started");
    esp_mqtt_client_publish(mqttClient, "/idfpye/qos1", "publish", 0, 1, 0);
    esp_mqtt_client_enqueue(mqttClient, "/idfpye/qos1", "enqueue", 0, 1, 0,
                            false);
    char buff[100];
    while (true) {
      sprintf(buff, "{ \"Temperature\":%d.%d, \"Humidity\":%d.%d }",
              dhtStructPtr->temperature.integer,
              dhtStructPtr->temperature.decimal, dhtStructPtr->humidity.integer,
              dhtStructPtr->humidity.decimal);
      esp_mqtt_client_enqueue(mqttClient, "/idfpye/qos1", buff, 0, 1, 0, false);
      ESP_LOGI(MQTTTAG, "read temp %d.%d", dhtStructPtr->temperature.integer,
               dhtStructPtr->temperature.decimal);
      vTaskDelay(2000 / portTICK_PERIOD_MS);
    }
  }
}
