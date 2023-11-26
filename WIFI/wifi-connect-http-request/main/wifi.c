#include "wifi.h"
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "freertos/semphr.h"
#include "esp_system.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_log.h"
#include "nvs_flash.h"

#include "lwip/err.h"
#include "lwip/sys.h"

/* The examples use WiFi configuration that you can set via project configuration menu
   If you'd rather not, just change the below entries to strings with
   the config you want - ie #define EXAMPLE_WIFI_SSID "mywifissid"
*/
#define EXAMPLE_ESP_WIFI_SSID      CONFIG_ESP_WIFI_SSID
#define EXAMPLE_ESP_WIFI_PASS      CONFIG_ESP_WIFI_PASSWORD
#define EXAMPLE_ESP_MAXIMUM_RETRY  CONFIG_ESP_MAXIMUM_RETRY

/* FreeRTOS event group to signal when we are connected*/
static EventGroupHandle_t s_wifi_event_group;

/* The event group allows multiple bits for each event, but we only care about two events:
 * - we are connected to the AP with an IP
 * - we failed to connect after the maximum amount of retries */
#define WIFI_CONNECTED_BIT BIT0
#define WIFI_FAIL_BIT      BIT1

static const char *TAG = "wifi library";    //tag for logging

static int s_retry_num = 0;                 //retry number

extern SemaphoreHandle_t wifi_semaphore;    //wifi semaphore declaration in main.c

static void event_handler(void* arg, esp_event_base_t event_base,
                                int32_t event_id, void* event_data)
{
    if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_START) {                 //if wifi event and wifi event station start
        esp_wifi_connect();
    } else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_DISCONNECTED) {   //if wifi event and wifi event station disconnected
        if (s_retry_num < EXAMPLE_ESP_MAXIMUM_RETRY) {
            esp_wifi_connect();                                                         //connect to wifi
            s_retry_num++;                                                              //increment retry number
            ESP_LOGI(TAG, "retry to connect to the AP");                                //log retry to connect to the AP
        } else {
            xEventGroupSetBits(s_wifi_event_group, WIFI_FAIL_BIT);                      //set wifi fail bit    
        }
        ESP_LOGI(TAG,"connect to the AP fail");                                         //log connect to the AP fail
    } else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP) {             //if ip event and ip event station got ip
        ip_event_got_ip_t* event = (ip_event_got_ip_t*) event_data;                     //get ip event data
        ESP_LOGI(TAG, "got ip:" IPSTR, IP2STR(&event->ip_info.ip));                     //log got ip
        s_retry_num = 0;                                                                //reset retry number
        xEventGroupSetBits(s_wifi_event_group, WIFI_CONNECTED_BIT);                     //set wifi connected bit
    }
}

void wifi_init_sta(void)
{
    s_wifi_event_group = xEventGroupCreate();               //create event group
    ESP_ERROR_CHECK(esp_netif_init());                      //initialize tcp/ip adapter
    ESP_ERROR_CHECK(esp_event_loop_create_default());       //create default event loop
    esp_netif_create_default_wifi_sta();                    //create default wifi station

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();    //initialize wifi configuration
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));                   //initialize wifi with configuration     

    ESP_ERROR_CHECK(esp_event_handler_register(WIFI_EVENT, ESP_EVENT_ANY_ID, &event_handler, NULL));    //register event handler
    ESP_ERROR_CHECK(esp_event_handler_register(IP_EVENT, IP_EVENT_STA_GOT_IP, &event_handler, NULL));   //register event handler

    wifi_config_t wifi_config = {               //wifi configuration
        .sta = {
            .ssid = EXAMPLE_ESP_WIFI_SSID,              //wifi ssid
            .password = EXAMPLE_ESP_WIFI_PASS,          //wifi password
	        .threshold.authmode = WIFI_AUTH_WPA2_PSK,   //wifi authentication mode
            .pmf_cfg = {                                //wifi pmf configuration
                .capable = true,                        //wifi capable
                .required = false                       //wifi required     
            },
        },
    };
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA) );                 //set wifi mode
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &wifi_config) );   //set wifi configuration
    ESP_ERROR_CHECK(esp_wifi_start() );                                 //start wifi        

    ESP_LOGI(TAG, "wifi_init_sta finished.");                           //log wifi initialization finished

    /* Waiting until either the connection is established (WIFI_CONNECTED_BIT) or connection failed for the maximum
     * number of re-tries (WIFI_FAIL_BIT). The bits are set by event_handler() (see above) */
    EventBits_t bits = xEventGroupWaitBits(s_wifi_event_group,
            WIFI_CONNECTED_BIT | WIFI_FAIL_BIT,
            pdFALSE,
            pdFALSE,
            portMAX_DELAY);

    /* xEventGroupWaitBits() returns the bits before the call returned, hence we can test which event actually
     * happened. */
    if (bits & WIFI_CONNECTED_BIT) {                                //if wifi connected bit is set
        ESP_LOGI(TAG, "connected to ap SSID:%s password:%s",        //log connected to ap
                 EXAMPLE_ESP_WIFI_SSID, EXAMPLE_ESP_WIFI_PASS);
        xSemaphoreGive(wifi_semaphore);                             //give semaphore
    } else if (bits & WIFI_FAIL_BIT) {                              //if wifi fail bit is set
        ESP_LOGI(TAG, "Failed to connect to SSID:%s, password:%s",  //log failed to connect to ap
                 EXAMPLE_ESP_WIFI_SSID, EXAMPLE_ESP_WIFI_PASS);
    } else {
        ESP_LOGE(TAG, "UNEXPECTED EVENT");                          //log unexpected event
    }
}



