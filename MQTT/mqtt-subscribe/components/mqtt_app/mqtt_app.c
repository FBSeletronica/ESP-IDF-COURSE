#include "esp_log.h"
#include "mqtt_client.h"

#include "mqtt_app.h"

static const char *TAG = "MQTT Client";

static esp_mqtt_client_handle_t client; //handler for mqtt client

/*
* Função de callback para eventos MQTT
*/
static void mqtt_event_handler(void *event_handler_arg,esp_event_base_t event_base, 
                                int32_t event_id, void *event_data)
{
    ESP_LOGI(TAG, "Event dispatched from event loop base=%s, event_id=%" PRIi32 "", event_base, event_id);
    esp_mqtt_event_handle_t event = event_data;

    switch ((esp_mqtt_event_id_t)event_id)
    {
    case MQTT_EVENT_CONNECTED:
        ESP_LOGI(TAG, "MQTT_EVENT_CONNECTED");
        break;
    case MQTT_EVENT_DISCONNECTED:
        ESP_LOGI(TAG, "MQTT_EVENT_DISCONNECTED");
        break;
    case MQTT_EVENT_SUBSCRIBED:
        ESP_LOGI(TAG, "MQTT_EVENT_SUBSCRIBED");
        break;
    case MQTT_EVENT_UNSUBSCRIBED:
        ESP_LOGI(TAG, "MQTT_EVENT_UNSUBSCRIBED");
        break;
    case MQTT_EVENT_PUBLISHED:
        ESP_LOGI(TAG, "MQTT_EVENT_PUBLISHED");
        break;
    case MQTT_EVENT_DATA:
        ESP_LOGI(TAG, "MQTT_EVENT_DATA");
        printf("topic: %.*s\n", event->topic_len, event->topic);
        printf("message: %.*s\n", event->data_len, event->data);
        break;
    case MQTT_EVENT_ERROR:
        ESP_LOGE(TAG, "ERROR %s", strerror(event->error_handle->esp_transport_sock_errno));
        break;
    default:
        ESP_LOGI(TAG, "Outro Evento id:%d", event->event_id);
        break;
    }
}

void mqtt_app_start(void)
{
    //config mqtt client
    esp_mqtt_client_config_t esp_mqtt_client_config = {
        .broker.address.uri = CONFIG_BROKER_URL, //mqtt broker url
        .broker.address.port = 1883,                            //mqtt broker port
        };
    client = esp_mqtt_client_init(&esp_mqtt_client_config);                             //initialize mqtt client
    esp_mqtt_client_register_event(client, ESP_EVENT_ANY_ID, mqtt_event_handler, NULL); //register callback function
    esp_mqtt_client_start(client);                                                      //start mqtt client 
}
void mqtt_app_subscribe(char *topic,int qos)
{
    int msg_id = esp_mqtt_client_subscribe(client, topic, 0);
    ESP_LOGI(TAG, "sent subscribe successful, msg_id=%d", msg_id);
}
void mqtt_app_unsubscribe(char *topic)
{
    int msg_id = esp_mqtt_client_unsubscribe(client, topic);
    ESP_LOGI(TAG, "sent unsubscribe successful, msg_id=%d", msg_id);
}

