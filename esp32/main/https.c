#include "esp_crt_bundle.h"
#include "esp_http_client.h"
#include "esp_log.h"
#include "esp_tls.h"
#include <string.h>
#include <sys/param.h>

#define MAX_HTTP_RECV_BUFFER 512
#define MAX_HTTP_OUTPUT_BUFFER 2048

const char *HTTPTAG = "HTTP";

const char *cert =
    "-----BEGIN CERTIFICATE-----"
    "MIIDKzCCAhOgAwIBAgIUXraiznIaZ/WUXzLO2gBKU6Fo/1owDQYJKoZIhvcNAQEL"
    "BQAwJTELMAkGA1UEBhMCU0UxFjAUBgNVBAMMDXNraXBwaW5ncy5jb20wHhcNMjQw"
    "OTIyMTAzMTU0WhcNMjUwOTIyMTAzMTU0WjAlMQswCQYDVQQGEwJTRTEWMBQGA1UE"
    "AwwNc2tpcHBpbmdzLmNvbTCCASIwDQYJKoZIhvcNAQEBBQADggEPADCCAQoCggEB"
    "ALp2IGVaA9fAxnPtoBW5JQfjk0SzMviajUjSbdw1Jomv15X/2rqLZYcQFqnyE6LJ"
    "cydTs/1RSMFECWUw2b4j/l9z28PGlgxtfvR7F0TFhQoySRhIUNk7hPO395nuXXw9"
    "CDbOhsmlMiy9MHaeihoOObyPheDwjCZx8pFUzgPDT6eVN/YgbJWNJzl14ppvixCr"
    "7+8AoYQRaKYMZLyfhSm6twdQFyPu79hxjenwybcsJz5lNRuWGgZM0zBOXfFYiKyq"
    "dH7gOBL7PEuvOdyV8Upn112FZSDKx6bk6xJlfXyBveX/b5WhPHI+J0887mt1aI4j"
    "vyeFGNYpkYxqAdxMGtwoPwkCAwEAAaNTMFEwHQYDVR0OBBYEFO/kkzEqdBXsl+zt"
    "2mVt2wrwviIWMB8GA1UdIwQYMBaAFO/kkzEqdBXsl+zt2mVt2wrwviIWMA8GA1Ud"
    "EwEB/wQFMAMBAf8wDQYJKoZIhvcNAQELBQADggEBAEn31PzZsCVZZqH0YkcOLw1v"
    "jZsEbRh0RtSInIF0f+YElOO+ptmiOiwtaI8OcjtdY2utXfcePOV+7A2rtXWUzhon"
    "XtR4vVMttYr0TZ+V4nJDRUED+6+68spxw3PuwpvLzJWCpd3r7zsyNDfhxuBOWU1w"
    "50PVlPnEXI3CRXl13ykenMyw4gQ7tEB8ekamdhd2RQn5LnYDTQhIyguUgCsz64vU"
    "YGAhx3EzKBoi3rEZCbR68tMx4icgglb4AlNXwvPXKwwsGbzln9PudYCpuGkl2EVE"
    "8jt3hCTNQ9egkNraAObeMFB0aHO5LMbF2a57NEidAIN4l8+97Gpnbu5qkAXd9PE="
    "-----END CERTIFICATE-----";

esp_err_t _http_event_handler(esp_http_client_event_t *evt) {
  static char *output_buffer; // Buffer to store response of http request from
                              // event handler
  static int output_len;      // Stores number of bytes read
  switch (evt->event_id) {
  case HTTP_EVENT_ERROR:
    ESP_LOGD(HTTPTAG, "HTTP_EVENT_ERROR");
    break;
  case HTTP_EVENT_ON_CONNECTED:
    ESP_LOGD(HTTPTAG, "HTTP_EVENT_ON_CONNECTED");
    break;
  case HTTP_EVENT_HEADER_SENT:
    ESP_LOGD(HTTPTAG, "HTTP_EVENT_HEADER_SENT");
    break;
  case HTTP_EVENT_ON_HEADER:
    ESP_LOGD(HTTPTAG, "HTTP_EVENT_ON_HEADER, key=%s, value=%s", evt->header_key,
             evt->header_value);
    break;
  case HTTP_EVENT_ON_DATA:
    ESP_LOGD(HTTPTAG, "HTTP_EVENT_ON_DATA, len=%d", evt->data_len);
    // Clean the buffer in case of a new request
    if (output_len == 0 && evt->user_data) {
      // we are just starting to copy the output data into the use
      memset(evt->user_data, 0, MAX_HTTP_OUTPUT_BUFFER);
    }
    /*
     *  Check for chunked encoding is added as the URL for chunked encoding used
     * in this example returns binary data. However, event handler can also be
     * used in case chunked encoding is used.
     */
    if (!esp_http_client_is_chunked_response(evt->client)) {
      // If user_data buffer is configured, copy the response into the buffer
      int copy_len = 0;
      if (evt->user_data) {
        // The last byte in evt->user_data is kept for the NULL character in
        // case of out-of-bound access.
        copy_len = MIN(evt->data_len, (MAX_HTTP_OUTPUT_BUFFER - output_len));
        if (copy_len) {
          memcpy(evt->user_data + output_len, evt->data, copy_len);
        }
      } else {
        int content_len = esp_http_client_get_content_length(evt->client);
        if (output_buffer == NULL) {
          // We initialize output_buffer with 0 because it is used by strlen()
          // and similar functions therefore should be null terminated.
          output_buffer = (char *)calloc(content_len + 1, sizeof(char));
          output_len = 0;
          if (output_buffer == NULL) {
            ESP_LOGE(HTTPTAG,
                     "Failed to allocate memory for output buffer with size %d",
                     content_len);
            return ESP_FAIL;
          }
        }
        copy_len = MIN(evt->data_len, (content_len - output_len));
        if (copy_len) {
          memcpy(output_buffer + output_len, evt->data, copy_len);
        }
      }
      output_len += copy_len;
    }

    break;
  case HTTP_EVENT_ON_FINISH:
    ESP_LOGD(HTTPTAG, "HTTP_EVENT_ON_FINISH");
    if (output_buffer != NULL) {
      // Response is accumulated in output_buffer. Uncomment the below line to
      // print the accumulated response ESP_LOG_BUFFER_HEX(TAG, output_buffer,
      // output_len);
      ESP_LOGI(HTTPTAG, "%s", output_buffer);
      free(output_buffer);
      output_buffer = NULL;
    }
    output_len = 0;
    break;
  case HTTP_EVENT_DISCONNECTED:
    ESP_LOGI(HTTPTAG, "HTTP_EVENT_DISCONNECTED");
    int mbedtls_err = 0;
    esp_err_t err = esp_tls_get_and_clear_last_error(
        (esp_tls_error_handle_t)evt->data, &mbedtls_err, NULL);
    if (err != 0) {
      ESP_LOGI(HTTPTAG, "Last esp error code: 0x%x", err);
      ESP_LOGI(HTTPTAG, "Last mbedtls failure: 0x%x", mbedtls_err);
    }
    if (output_buffer != NULL) {
      free(output_buffer);
      output_buffer = NULL;
    }
    output_len = 0;
    break;
  case HTTP_EVENT_REDIRECT:
    ESP_LOGD(HTTPTAG, "HTTP_EVENT_REDIRECT");
    esp_http_client_set_header(evt->client, "From", "user@example.com");
    esp_http_client_set_header(evt->client, "Accept", "text/html");
    esp_http_client_set_redirection(evt->client);
    break;
  }
  return ESP_OK;
}

void https_with_url(void) {

  esp_http_client_config_t config = {
      .url = "https://httpbin.org/get",
      .event_handler = _http_event_handler,
      .crt_bundle_attach = esp_crt_bundle_attach,
  };
  esp_http_client_handle_t client = esp_http_client_init(&config);
  esp_http_client_set_method(client, HTTP_METHOD_GET);

  const char *testPost =
      "{\"temperature\":100,\"humidity\":0,\"timestamp\":\"0\"}";
  esp_http_client_set_url(
      client, "https://www.skippings.com/api/sensor-data/A1:B2:C3:D4:E5");
  esp_http_client_set_method(client, HTTP_METHOD_POST);
  esp_http_client_set_header(client, "Content-Type", "application/json");
  esp_http_client_set_post_field(client, testPost, strlen(testPost));

  esp_err_t err = esp_http_client_perform(client);

  if (err == ESP_OK) {
    ESP_LOGI(HTTPTAG, "HTTPS Status = %d, content_length = %" PRId64,
             esp_http_client_get_status_code(client),
             esp_http_client_get_content_length(client));
  } else {
    ESP_LOGE(HTTPTAG, "Error perform http request %s", esp_err_to_name(err));
  }
  esp_http_client_cleanup(client);

  esp_tls_cfg_t tlsCfg = {
      .crt_bundle_attach = esp_crt_bundle_attach,
  };
  struct esp_tls *tls =
      esp_tls_conn_http_new("https://www.skippings.com", &tlsCfg);
  if (tls != NULL) {
    ESP_LOGI(HTTPTAG, "CON GOOD?????");
  } else {
    ESP_LOGE(HTTPTAG, "NO GOOD :/");
  }
}
