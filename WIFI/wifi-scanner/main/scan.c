/******************************************************************************
* WIFI Scan Example
* This example shows how to scan for available set of APs.
* This code is part of the course "Programe o ESP32 com ESP-IDF 5" by Fábio Souza
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
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"	
#include "freertos/event_groups.h"
#include "esp_wifi.h"
#include "esp_log.h"
#include "esp_event.h"
#include "nvs_flash.h"

#define DEFAULT_SCAN_LIST_SIZE 10  

static const char *TAG = "WiFi Scan";

static void print_auth_mode(int authmode)
{
    switch (authmode) {
    case WIFI_AUTH_OPEN:
        ESP_LOGI(TAG, "Authmode \tWIFI_AUTH_OPEN");
        break;
    case WIFI_AUTH_OWE:
        ESP_LOGI(TAG, "Authmode \tWIFI_AUTH_OWE");
        break;
    case WIFI_AUTH_WEP:
        ESP_LOGI(TAG, "Authmode \tWIFI_AUTH_WEP");
        break;
    case WIFI_AUTH_WPA_PSK:
        ESP_LOGI(TAG, "Authmode \tWIFI_AUTH_WPA_PSK");
        break;
    case WIFI_AUTH_WPA2_PSK:
        ESP_LOGI(TAG, "Authmode \tWIFI_AUTH_WPA2_PSK");
        break;
    case WIFI_AUTH_WPA_WPA2_PSK:
        ESP_LOGI(TAG, "Authmode \tWIFI_AUTH_WPA_WPA2_PSK");
        break;
    case WIFI_AUTH_WPA2_ENTERPRISE:
        ESP_LOGI(TAG, "Authmode \tWIFI_AUTH_WPA2_ENTERPRISE");
        break;
    case WIFI_AUTH_WPA3_PSK:
        ESP_LOGI(TAG, "Authmode \tWIFI_AUTH_WPA3_PSK");
        break;
    case WIFI_AUTH_WPA2_WPA3_PSK:
        ESP_LOGI(TAG, "Authmode \tWIFI_AUTH_WPA2_WPA3_PSK");
        break;
    default:
        ESP_LOGI(TAG, "Authmode \tWIFI_AUTH_UNKNOWN");
        break;
    }
}

static void print_cipher_type(int pairwise_cipher, int group_cipher)
{
    switch (pairwise_cipher) {
    case WIFI_CIPHER_TYPE_NONE:
        ESP_LOGI(TAG, "Pairwise Cipher \tWIFI_CIPHER_TYPE_NONE");
        break;
    case WIFI_CIPHER_TYPE_WEP40:
        ESP_LOGI(TAG, "Pairwise Cipher \tWIFI_CIPHER_TYPE_WEP40");
        break;
    case WIFI_CIPHER_TYPE_WEP104:
        ESP_LOGI(TAG, "Pairwise Cipher \tWIFI_CIPHER_TYPE_WEP104");
        break;
    case WIFI_CIPHER_TYPE_TKIP:
        ESP_LOGI(TAG, "Pairwise Cipher \tWIFI_CIPHER_TYPE_TKIP");
        break;
    case WIFI_CIPHER_TYPE_CCMP:
        ESP_LOGI(TAG, "Pairwise Cipher \tWIFI_CIPHER_TYPE_CCMP");
        break;
    case WIFI_CIPHER_TYPE_TKIP_CCMP:
        ESP_LOGI(TAG, "Pairwise Cipher \tWIFI_CIPHER_TYPE_TKIP_CCMP");
        break;
    case WIFI_CIPHER_TYPE_AES_CMAC128:
        ESP_LOGI(TAG, "Pairwise Cipher \tWIFI_CIPHER_TYPE_AES_CMAC128");
        break;
    case WIFI_CIPHER_TYPE_SMS4:
        ESP_LOGI(TAG, "Pairwise Cipher \tWIFI_CIPHER_TYPE_SMS4");
        break;
    case WIFI_CIPHER_TYPE_GCMP:
        ESP_LOGI(TAG, "Pairwise Cipher \tWIFI_CIPHER_TYPE_GCMP");
        break;
    case WIFI_CIPHER_TYPE_GCMP256:
        ESP_LOGI(TAG, "Pairwise Cipher \tWIFI_CIPHER_TYPE_GCMP256");
        break;
    default:
        ESP_LOGI(TAG, "Pairwise Cipher \tWIFI_CIPHER_TYPE_UNKNOWN");
        break;
    }

    switch (group_cipher) {
    case WIFI_CIPHER_TYPE_NONE:
        ESP_LOGI(TAG, "Group Cipher \tWIFI_CIPHER_TYPE_NONE");
        break;
    case WIFI_CIPHER_TYPE_WEP40:
        ESP_LOGI(TAG, "Group Cipher \tWIFI_CIPHER_TYPE_WEP40");
        break;
    case WIFI_CIPHER_TYPE_WEP104:
        ESP_LOGI(TAG, "Group Cipher \tWIFI_CIPHER_TYPE_WEP104");
        break;
    case WIFI_CIPHER_TYPE_TKIP:
        ESP_LOGI(TAG, "Group Cipher \tWIFI_CIPHER_TYPE_TKIP");
        break;
    case WIFI_CIPHER_TYPE_CCMP:
        ESP_LOGI(TAG, "Group Cipher \tWIFI_CIPHER_TYPE_CCMP");
        break;
    case WIFI_CIPHER_TYPE_TKIP_CCMP:
        ESP_LOGI(TAG, "Group Cipher \tWIFI_CIPHER_TYPE_TKIP_CCMP");
        break;
    case WIFI_CIPHER_TYPE_SMS4:
        ESP_LOGI(TAG, "Group Cipher \tWIFI_CIPHER_TYPE_SMS4");
        break;
    case WIFI_CIPHER_TYPE_GCMP:
        ESP_LOGI(TAG, "Group Cipher \tWIFI_CIPHER_TYPE_GCMP");
        break;
    case WIFI_CIPHER_TYPE_GCMP256:
        ESP_LOGI(TAG, "Group Cipher \tWIFI_CIPHER_TYPE_GCMP256");
        break;
    default:
        ESP_LOGI(TAG, "Group Cipher \tWIFI_CIPHER_TYPE_UNKNOWN");
        break;
    }
}

/* Initialize Wi-Fi as sta and set scan method */
static void wifi_scan(void)
{
    ESP_ERROR_CHECK(esp_netif_init());                              // Initialize the underlying TCP/IP stack
    ESP_ERROR_CHECK(esp_event_loop_create_default());               // Create default event loop
    esp_netif_t *sta_netif = esp_netif_create_default_wifi_sta();   // Create default Wi-Fi station netif instance
    assert(sta_netif);                                              // Check the netif has been created

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();            // Init Wi-Fi config structure
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));                           // Init Wi-Fi driver

    uint16_t number = DEFAULT_SCAN_LIST_SIZE;                       // Number of APs found in scan
    wifi_ap_record_t ap_info[DEFAULT_SCAN_LIST_SIZE];               // Array to store APs found in scan
    uint16_t ap_count = 0;                                          // Number of APs found in scan
    memset(ap_info, 0, sizeof(ap_info));                            // Clear array

    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));               // Set Wi-Fi mode to station
    ESP_ERROR_CHECK(esp_wifi_start());                               // Start Wi-Fi
    esp_wifi_scan_start(NULL, true);                                 // Start Wi-Fi scan
    ESP_ERROR_CHECK(esp_wifi_scan_get_ap_records(&number, ap_info)); // Get number of APs found in scan
    ESP_ERROR_CHECK(esp_wifi_scan_get_ap_num(&ap_count));               // Get APs found in scan
    ESP_LOGI(TAG, "Total APs scanned = %u", ap_count);                  // Print number of APs found in scan

    // Print APs found in scan
    for (int i = 0; (i < DEFAULT_SCAN_LIST_SIZE) && (i < ap_count); i++) {      
        ESP_LOGI(TAG, "SSID \t\t%s", ap_info[i].ssid);                              // Print SSID
        ESP_LOGI(TAG, "RSSI \t\t%d", ap_info[i].rssi);                              // Print RSSI
        print_auth_mode(ap_info[i].authmode);                                       // Print authentication mode
        if (ap_info[i].authmode != WIFI_AUTH_WEP) {                                 // If authentication mode is not WEP    
            print_cipher_type(ap_info[i].pairwise_cipher, ap_info[i].group_cipher); // Print cipher type
        }
        ESP_LOGI(TAG, "Channel \t\t%d\n", ap_info[i].primary);                      // Print primary channel
    }

}

void app_main(void)
{
    
    esp_err_t ret = nvs_flash_init();   // Initialize NVS

    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND)   // Check if NVS partition has been erased
    {
        ESP_ERROR_CHECK(nvs_flash_erase());  // Erase NVS partition
        ret = nvs_flash_init();              // Initialize NVS
    }
    ESP_ERROR_CHECK( ret );                 // Check if NVS partition has been erased

    wifi_scan();                            // Scan for available APs
}
