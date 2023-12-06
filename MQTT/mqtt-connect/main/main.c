/******************************************************************************
* MQTT Connection Example
* This example shows how to connect to a MQTT broker 
* This code is part of the course "Programe o ESP32 com ESP-IDF" by Fábio Souza
* The course is available on https://cursos.embarcados.com.br
*
* This example code Creative Commons Attribution 4.0 International License.
* When using the code, you must keep the above copyright notice,
* this list of conditions and the following disclaimer in the source code.
* (http://creativecommons.org/licenses/by/4.0/)

* Author: Fábio Souza
* This code is for teaching purposes only.
* No warranty of any kind is provided.
*******************************************************************************/
#include <stdio.h>
#include <string.h>

//freertos includes
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "esp_log.h"
#include "nvs_flash.h"

#include "mqtt_client.h"     //mqtt library

#include "wifi.h"            //wifi component

//tag for logging
static const char *TAG = "MQTT Example";

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

//main func
void app_main(void)
{
    //Initialize NVS
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {   //if nvs flash is not initialized
      ESP_ERROR_CHECK(nvs_flash_erase());                                             //erase nvs flash
      ret = nvs_flash_init();                                                         //initialize nvs flash
    }
    ESP_ERROR_CHECK(ret);                                                             //check error

    ESP_ERROR_CHECK(wifi_init_sta());                                 //initialize wifi station mode 
                                                             

    //config mqtt client
    esp_mqtt_client_config_t esp_mqtt_client_config = {
        .broker.address.uri = "mqtt://mqtt.eclipseprojects.io", //mqtt broker url
        .broker.address.port = 1883,                            //mqtt broker port
        };
    client = esp_mqtt_client_init(&esp_mqtt_client_config);                             //initialize mqtt client
    esp_mqtt_client_register_event(client, ESP_EVENT_ANY_ID, mqtt_event_handler, NULL); //register callback function
    esp_mqtt_client_start(client);                                                      //start mqtt client                                   
}
