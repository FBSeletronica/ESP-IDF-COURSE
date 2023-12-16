/******************************************************************************
* ESP-NOW Get Mac Address
* This example shows how to get the MAC address of the ESP32
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

#include <stdio.h>
#include "esp_mac.h"
#include "esp_log.h"

const char *TAG = "ESP MAC Address";

void app_main(void)
{
    uint8_t mac_address[6];         
    //get mac address
    esp_efuse_mac_get_default(mac_address);

    //print mac address
    ESP_LOGI(TAG, "MAC Address: %02x:%02x:%02x:%02x:%02x:%02x", mac_address[0],mac_address[1],mac_address[2],mac_address[3],mac_address[4],mac_address[5]);
    
}
