
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "freertos/semphr.h"
#include "esp_system.h"
#include "esp_mac.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_log.h"

#include "lwip/err.h"
#include "lwip/sys.h"

#include "wifi.h"

#define EXAMPLE_ESP_WIFI_SSID      CONFIG_ESP_WIFI_SSID
#define EXAMPLE_ESP_WIFI_PASS      CONFIG_ESP_WIFI_PASSWORD
#define EXAMPLE_ESP_MAXIMUM_RETRY  CONFIG_ESP_MAXIMUM_RETRY

/* FreeRTOS event group to signal when we are connected*/
static EventGroupHandle_t s_wifi_event_group;

#define WIFI_CONNECTED_BIT BIT0
#define WIFI_FAIL_BIT      BIT1

static esp_netif_t *esp_netif;
static const char *TAG = "wifi library";    //tag for logging
static int s_retry_num = 0;                 //retry number
static bool attempt_reconnect = false;      //attempt reconnect

  
char *get_wifi_disconnection_string(wifi_err_reason_t wifi_err_reason)
{
    switch (wifi_err_reason)            //switch wifi error reason
    {
    case WIFI_REASON_UNSPECIFIED:
        return "WIFI_REASON_UNSPECIFIED";
    case WIFI_REASON_AUTH_EXPIRE:
        return "WIFI_REASON_AUTH_EXPIRE";
    case WIFI_REASON_AUTH_LEAVE:
        return "WIFI_REASON_AUTH_LEAVE";
    case WIFI_REASON_ASSOC_EXPIRE:
        return "WIFI_REASON_ASSOC_EXPIRE";
    case WIFI_REASON_ASSOC_TOOMANY:
        return "WIFI_REASON_ASSOC_TOOMANY";
    case WIFI_REASON_NOT_AUTHED:
        return "WIFI_REASON_NOT_AUTHED";
    case WIFI_REASON_NOT_ASSOCED:
        return "WIFI_REASON_NOT_ASSOCED";
    case WIFI_REASON_ASSOC_LEAVE:
        return "WIFI_REASON_ASSOC_LEAVE";
    case WIFI_REASON_ASSOC_NOT_AUTHED:
        return "WIFI_REASON_ASSOC_NOT_AUTHED";
    case WIFI_REASON_DISASSOC_PWRCAP_BAD:
        return "WIFI_REASON_DISASSOC_PWRCAP_BAD";
    case WIFI_REASON_DISASSOC_SUPCHAN_BAD:
        return "WIFI_REASON_DISASSOC_SUPCHAN_BAD";
    case WIFI_REASON_BSS_TRANSITION_DISASSOC:
        return "WIFI_REASON_BSS_TRANSITION_DISASSOC";
    case WIFI_REASON_IE_INVALID:
        return "WIFI_REASON_IE_INVALID";
    case WIFI_REASON_MIC_FAILURE:
        return "WIFI_REASON_MIC_FAILURE";
    case WIFI_REASON_4WAY_HANDSHAKE_TIMEOUT:
        return "WIFI_REASON_4WAY_HANDSHAKE_TIMEOUT";
    case WIFI_REASON_GROUP_KEY_UPDATE_TIMEOUT:
        return "WIFI_REASON_GROUP_KEY_UPDATE_TIMEOUT";
    case WIFI_REASON_IE_IN_4WAY_DIFFERS:
        return "WIFI_REASON_IE_IN_4WAY_DIFFERS";
    case WIFI_REASON_GROUP_CIPHER_INVALID:
        return "WIFI_REASON_GROUP_CIPHER_INVALID";
    case WIFI_REASON_PAIRWISE_CIPHER_INVALID:
        return "WIFI_REASON_PAIRWISE_CIPHER_INVALID";
    case WIFI_REASON_AKMP_INVALID:
        return "WIFI_REASON_AKMP_INVALID";
    case WIFI_REASON_UNSUPP_RSN_IE_VERSION:
        return "WIFI_REASON_UNSUPP_RSN_IE_VERSION";
    case WIFI_REASON_INVALID_RSN_IE_CAP:
        return "WIFI_REASON_INVALID_RSN_IE_CAP";
    case WIFI_REASON_802_1X_AUTH_FAILED:
        return "WIFI_REASON_802_1X_AUTH_FAILED";
    case WIFI_REASON_CIPHER_SUITE_REJECTED:
        return "WIFI_REASON_CIPHER_SUITE_REJECTED";
    case WIFI_REASON_TDLS_PEER_UNREACHABLE:
        return "WIFI_REASON_TDLS_PEER_UNREACHABLE";
    case WIFI_REASON_TDLS_UNSPECIFIED:
        return "WIFI_REASON_TDLS_UNSPECIFIED";
    case WIFI_REASON_SSP_REQUESTED_DISASSOC:
        return "WIFI_REASON_SSP_REQUESTED_DISASSOC";
    case WIFI_REASON_NO_SSP_ROAMING_AGREEMENT:
        return "WIFI_REASON_NO_SSP_ROAMING_AGREEMENT";
    case WIFI_REASON_BAD_CIPHER_OR_AKM:
        return "WIFI_REASON_BAD_CIPHER_OR_AKM";
    case WIFI_REASON_NOT_AUTHORIZED_THIS_LOCATION:
        return "WIFI_REASON_NOT_AUTHORIZED_THIS_LOCATION";
    case WIFI_REASON_SERVICE_CHANGE_PERCLUDES_TS:
        return "WIFI_REASON_SERVICE_CHANGE_PERCLUDES_TS";
    case WIFI_REASON_UNSPECIFIED_QOS:
        return "WIFI_REASON_UNSPECIFIED_QOS";
    case WIFI_REASON_NOT_ENOUGH_BANDWIDTH:
        return "WIFI_REASON_NOT_ENOUGH_BANDWIDTH";
    case WIFI_REASON_MISSING_ACKS:
        return "WIFI_REASON_MISSING_ACKS";
    case WIFI_REASON_EXCEEDED_TXOP:
        return "WIFI_REASON_EXCEEDED_TXOP";
    case WIFI_REASON_STA_LEAVING:
        return "WIFI_REASON_STA_LEAVING";
    case WIFI_REASON_END_BA:
        return "WIFI_REASON_END_BA";
    case WIFI_REASON_UNKNOWN_BA:
        return "WIFI_REASON_UNKNOWN_BA";
    case WIFI_REASON_TIMEOUT:
        return "WIFI_REASON_TIMEOUT";
    case WIFI_REASON_PEER_INITIATED:
        return "WIFI_REASON_PEER_INITIATED";
    case WIFI_REASON_AP_INITIATED:
        return "WIFI_REASON_AP_INITIATED";
    case WIFI_REASON_INVALID_FT_ACTION_FRAME_COUNT:
        return "WIFI_REASON_INVALID_FT_ACTION_FRAME_COUNT";
    case WIFI_REASON_INVALID_PMKID:
        return "WIFI_REASON_INVALID_PMKID";
    case WIFI_REASON_INVALID_MDE:
        return "WIFI_REASON_INVALID_MDE";
    case WIFI_REASON_INVALID_FTE:
        return "WIFI_REASON_INVALID_FTE";
    case WIFI_REASON_TRANSMISSION_LINK_ESTABLISH_FAILED:
        return "WIFI_REASON_TRANSMISSION_LINK_ESTABLISH_FAILED";
    case WIFI_REASON_ALTERATIVE_CHANNEL_OCCUPIED:
        return "WIFI_REASON_ALTERATIVE_CHANNEL_OCCUPIED";
    case WIFI_REASON_BEACON_TIMEOUT:
        return "WIFI_REASON_BEACON_TIMEOUT";
    case WIFI_REASON_NO_AP_FOUND:
        return "WIFI_REASON_NO_AP_FOUND";
    case WIFI_REASON_AUTH_FAIL:
        return "WIFI_REASON_AUTH_FAIL";
    case WIFI_REASON_ASSOC_FAIL:
        return "WIFI_REASON_ASSOC_FAIL";
    case WIFI_REASON_HANDSHAKE_TIMEOUT:
        return "WIFI_REASON_HANDSHAKE_TIMEOUT";
    case WIFI_REASON_CONNECTION_FAIL:
        return "WIFI_REASON_CONNECTION_FAIL";
    case WIFI_REASON_AP_TSF_RESET:
        return "WIFI_REASON_AP_TSF_RESET";
    case WIFI_REASON_ROAMING:
        return "WIFI_REASON_ROAMING";
    case WIFI_REASON_ASSOC_COMEBACK_TIME_TOO_LONG:
        return "WIFI_REASON_ASSOC_COMEBACK_TIME_TOO_LONG";
    case WIFI_REASON_SA_QUERY_TIMEOUT:
        return "WIFI_REASON_SA_QUERY_TIMEOUT";
    }
    return "UNKNOWN";
}

static void event_handler(void* arg, esp_event_base_t event_base,
                                int32_t event_id, void* event_data)
{
    if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_START) {                 //if wifi event and wifi event station start
        esp_wifi_connect();
    } else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_DISCONNECTED) {   //if wifi event and wifi event station disconnected
        
        wifi_event_sta_disconnected_t *wifi_event_sta_disconnected = event_data;        //get wifi event data
        ESP_LOGW(TAG, "DISCONNECTED %d: %s", wifi_event_sta_disconnected->reason,       //log wifi event data
                 get_wifi_disconnection_string(wifi_event_sta_disconnected->reason));
        if (attempt_reconnect)
        {
            if (wifi_event_sta_disconnected->reason == WIFI_REASON_NO_AP_FOUND ||         //if wifi event data reason is no ap found or
                wifi_event_sta_disconnected->reason == WIFI_REASON_ASSOC_LEAVE ||         //if wifi event data reason is assoc leave or  
                wifi_event_sta_disconnected->reason == WIFI_REASON_AUTH_EXPIRE)           //if wifi event data reason is auth expire
            {

                if (s_retry_num < EXAMPLE_ESP_MAXIMUM_RETRY) {                                  //if retry number is less than maximum retry
                    esp_wifi_connect();                                                         //connect to wifi
                    s_retry_num++;                                                              //increment retry number
                    ESP_LOGI(TAG, "retry to connect to the AP");                                //log retry to connect to the AP
                } else {
                    ESP_LOGE(TAG,"connect to the AP fail"); 
                    xEventGroupSetBits(s_wifi_event_group, WIFI_FAIL_BIT);                      //set wifi fail bit    
                }
            }
        }                                        
    } else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP) {             //if ip event and ip event station got ip
        ip_event_got_ip_t* event = (ip_event_got_ip_t*) event_data;                     //get ip event data
        ESP_LOGI(TAG, "got ip:" IPSTR, IP2STR(&event->ip_info.ip));                     //log got ip
        s_retry_num = 0;                                                                //reset retry number
        xEventGroupSetBits(s_wifi_event_group, WIFI_CONNECTED_BIT);                     //set wifi connected bit
    }
}

static void ap_event_handler(void* arg, esp_event_base_t event_base,
                                    int32_t event_id, void* event_data)
{

    if (event_id == WIFI_EVENT_AP_STACONNECTED) {                                           //if wifi event ap station connected
        wifi_event_ap_staconnected_t* event = (wifi_event_ap_staconnected_t*) event_data;   //get wifi event data
        ESP_LOGI(TAG, "station "MACSTR" join, AID=%d",                                      //log station join
                 MAC2STR(event->mac), event->aid);
    } else if (event_id == WIFI_EVENT_AP_STADISCONNECTED) {                                           //if wifi event ap station disconnected
        wifi_event_ap_stadisconnected_t* event = (wifi_event_ap_stadisconnected_t*) event_data;      //get wifi event data
        ESP_LOGI(TAG, "station "MACSTR" leave, AID=%d",                                              //log station leave
                 MAC2STR(event->mac), event->aid);
    }
}

void wifi_init(void){
    ESP_ERROR_CHECK(esp_netif_init());                      //initialize tcp/ip adapter
    ESP_ERROR_CHECK(esp_event_loop_create_default());       //create default event loop
    
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();    //initialize wifi configuration
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));                   //initialize wifi with configuration 

    ESP_ERROR_CHECK(esp_event_handler_register(WIFI_EVENT, ESP_EVENT_ANY_ID, &event_handler, NULL));    //register event handler
    ESP_ERROR_CHECK(esp_event_handler_register(IP_EVENT, IP_EVENT_STA_GOT_IP, &event_handler, NULL));   //register event handler

    ESP_ERROR_CHECK(esp_wifi_set_storage(WIFI_STORAGE_RAM));                                            //set wifi storage
}

esp_err_t wifi_init_sta(void)
{
    wifi_init();                                            //initialize wifi
    attempt_reconnect = true;
    s_wifi_event_group = xEventGroupCreate();               //create event group
    esp_netif = esp_netif_create_default_wifi_sta();        //create default wifi station
    
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
        return ESP_OK;                                              //return ok
    } else if (bits & WIFI_FAIL_BIT) {                              //if wifi fail bit is set
        ESP_LOGE(TAG, "Failed to connect to SSID:%s, password:%s",  //log failed to connect to ap
                 EXAMPLE_ESP_WIFI_SSID, EXAMPLE_ESP_WIFI_PASS);
    } else {
        ESP_LOGE(TAG, "UNEXPECTED EVENT");                          //log unexpected event
    }
    return ESP_FAIL;                                                //return fail
}


void wifi_connect_ap(const char *ssid, const char *pass)
{
    wifi_init();                                            //initialize wifi
    ESP_LOGI(TAG, "ESP_WIFI_MODE_AP");                      //log wifi mode
    esp_netif = esp_netif_create_default_wifi_ap();         //create default wifi access point

    ESP_ERROR_CHECK(esp_event_handler_instance_register(WIFI_EVENT,         //register event handler instance
                                                        ESP_EVENT_ANY_ID,   //event id
                                                        &ap_event_handler,  //event handler
                                                        NULL,               //no event handler arg
                                                        NULL));             //no event handler instance

    wifi_config_t wifi_config = {};                                                         //wifi configuration                                 
    strncpy((char *)wifi_config.ap.ssid, ssid, sizeof(wifi_config.ap.ssid) - 1);            //copy ssid to wifi configuration ssid
    strncpy((char *)wifi_config.ap.password, pass, sizeof(wifi_config.ap.password) - 1);    //copy password to wifi configuration password
    wifi_config.ap.authmode = WIFI_AUTH_WPA_WPA2_PSK;                                       //wifi authentication mode
    wifi_config.ap.max_connection = 4;                                                      //wifi maximum connection
    wifi_config.ap.beacon_interval = 100;                                                   //wifi beacon interval
    wifi_config.ap.channel = 1;                                                             //wifi channel

    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_AP));                                       //set wifi mode
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_AP, &wifi_config));                         //set wifi configuration
    ESP_ERROR_CHECK(esp_wifi_start());                                                      //start wifi        

    ESP_LOGI(TAG, "wifi_init_softap finished. SSID:%s password:%s channel:%d",              //log wifi initialization finished
             wifi_config.ap.ssid, wifi_config.ap.password, wifi_config.ap.channel);         
}


void wifi_disconnect(void)
{
    attempt_reconnect = false;
    esp_wifi_stop();
    esp_netif_destroy(esp_netif);
}
