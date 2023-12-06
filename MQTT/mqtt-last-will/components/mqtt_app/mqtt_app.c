#include "esp_log.h"
#include "mqtt_client.h"

#include "freertos/event_groups.h"

#include "mqtt_app.h"

static const char *TAG = "MQTT Client";

static esp_mqtt_client_handle_t client; //handler for mqtt client

static EventGroupHandle_t status_mqtt_event_group;

#define MQTT_CONNECTED    BIT0



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
        xEventGroupSetBits(status_mqtt_event_group, MQTT_CONNECTED);
        mqtt_app_publish("asdfg/status","Online",1,0);                  //publish message "Online" to topic "esp32/status" with qos 0 and retain 0
        break;
    case MQTT_EVENT_DISCONNECTED:
        ESP_LOGI(TAG, "MQTT_EVENT_DISCONNECTED");
        xEventGroupClearBits(status_mqtt_event_group, MQTT_CONNECTED);
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

int mqtt_is_connected(void)
{
    EventBits_t bits = xEventGroupGetBits(status_mqtt_event_group);
    if (bits & MQTT_CONNECTED)
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

void mqtt_app_start(void)
{
    status_mqtt_event_group = xEventGroupCreate();
    //config mqtt client
    esp_mqtt_client_config_t esp_mqtt_client_config = {
        .network.disable_auto_reconnect = false,                //enable auto reconnect
        .session.keepalive = 30,                                //set keep alive to 30 seconds
        .broker.address.uri = CONFIG_BROKER_URL, //mqtt broker url
        .broker.address.port = 1883,                            //mqtt broker port
        .session.last_will = {
            .topic = "asdfg/status",
            .msg = "Offline",
            .msg_len = strlen("Offline"),
            .qos = 1,
            }
        };
    client = esp_mqtt_client_init(&esp_mqtt_client_config);                             //initialize mqtt client
    esp_mqtt_client_register_event(client, ESP_EVENT_ANY_ID, mqtt_event_handler, NULL); //register callback function
    esp_mqtt_client_start(client);                                                      //start mqtt client 
    mqtt_app_publish("asdfg/status","Online",1,0);                                      //publish message "Online" to topic "esp32/status" with qos 0 and retain 0
}
void mqtt_app_subscribe(char *topic,int qos)
{
    int msg_id = esp_mqtt_client_subscribe(client, topic, qos);
    ESP_LOGI(TAG, "sent subscribe successful, msg_id=%d", msg_id);
}
void mqtt_app_unsubscribe(char *topic)
{
    int msg_id = esp_mqtt_client_unsubscribe(client, topic);
    ESP_LOGI(TAG, "sent unsubscribe successful, msg_id=%d", msg_id);
}

void mqtt_app_publish(char *topic, char *payload, int qos, int retain)
{
    int msg_id = esp_mqtt_client_publish(client, topic, payload, strlen(payload), qos, retain);
    ESP_LOGI(TAG, "sent publish successful, msg_id=%d", msg_id);
}

void mqtt_app_stop(void)
{
    esp_mqtt_client_stop(client);
    ESP_LOGI(TAG, "MQTT stopped");
}