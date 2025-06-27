#include <string.h>
#include "http_client.h"
#include "esp_log.h"
#include "esp_http_client.h"

extern SemaphoreHandle_t wifi_semaphore;    //semaphore for wifi connection sincronization http client

static const char *TAG = "HTTP_CLIENT"; //tag for logging
static int s_len;
static char *s_response;

//http client handle
esp_err_t _http_event_handle(esp_http_client_event_t *evt)
{
    switch(evt->event_id) {
        case HTTP_EVENT_ERROR:
            ESP_LOGI(TAG, "HTTP_EVENT_ERROR");
            break;
        case HTTP_EVENT_ON_CONNECTED:
            ESP_LOGI(TAG, "HTTP_EVENT_ON_CONNECTED");
            break;
        case HTTP_EVENT_HEADER_SENT:
            ESP_LOGI(TAG, "HTTP_EVENT_HEADER_SENT");
            break;
        case HTTP_EVENT_ON_HEADER:
            ESP_LOGI(TAG, "HTTP_EVENT_ON_HEADER");
            if (s_len > 0) {
                memcpy(s_response, evt->data, evt->data_len < s_len ? evt->data_len : s_len);
                s_len -= evt->data_len;
                s_response += evt->data_len;
            }
            break;
        case HTTP_EVENT_ON_DATA:
            ESP_LOGI(TAG, "HTTP_EVENT_ON_DATA");
            if (!esp_http_client_is_chunked_response(evt->client)) {
                if (s_len > 0) {
                    memcpy(s_response, evt->data, evt->data_len < s_len ? evt->data_len : s_len);
                    s_len -= evt->data_len;
                    s_response += evt->data_len;
                }
            }
            break;
        case HTTP_EVENT_ON_FINISH:
            ESP_LOGI(TAG, "HTTP_EVENT_ON_FINISH");
            break;
        case HTTP_EVENT_DISCONNECTED:
            ESP_LOGI(TAG, "HTTP_EVENT_DISCONNECTED");
            break;
        case HTTP_EVENT_REDIRECT:
            ESP_LOGD(TAG, "HTTP_EVENT_REDIRECT");
            break;
    }
    return ESP_OK;
}

//http request function
esp_err_t http_client_request(const char *url, char *response, const int len)
{
    s_response = response;
    s_len = len;
    esp_http_client_config_t config = {
        .url = url,
        .event_handler = _http_event_handle
    };
    esp_http_client_handle_t client = esp_http_client_init(&config);
    esp_err_t err = esp_http_client_perform(client);

    esp_http_client_cleanup(client);
    return err;
}
