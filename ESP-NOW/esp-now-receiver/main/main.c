/******************************************************************************
* ESP-NOW Receiver
* This example shows how to use ESP-NOW to receive data from other ESP32 and control a LED
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
#include <stdlib.h>
#include <string.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "esp_wifi.h"
#include "nvs_flash.h"
#include "esp_now.h"
#include "esp_mac.h"

#include "driver/gpio.h"
#include "esp_log.h"

#define LED_PIN 14

#define TAG "ESP-NOW Receiver"

//callback function to receive data
void espnow_recv_cb(const esp_now_recv_info_t *esp_now_info, const uint8_t *data, int data_len)
{
  ESP_LOGI(TAG, "got message from " MACSTR, MAC2STR(esp_now_info->src_addr));
  printf("message: %.*s\n", data_len, data);

  //if message is "on", turn on LED
  if (strncmp((char *)data, "LED ON", data_len) == 0)
  {
    gpio_set_level(LED_PIN, 1);
  }
  //if message is "off", turn off LED
  else if (strncmp((char *)data, "LED OFF", data_len) == 0)
  {
    gpio_set_level(LED_PIN, 0);
  }
}

void app_main(void)
{
        
    //init NVS 
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES) 
    {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);

    //init WiFi 
    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));
    ESP_ERROR_CHECK(esp_wifi_set_storage(WIFI_STORAGE_RAM));
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
    ESP_ERROR_CHECK(esp_wifi_start());

    //init ESP-NOW
    ESP_ERROR_CHECK(esp_now_init());
    ESP_ERROR_CHECK(esp_now_register_recv_cb(espnow_recv_cb));
    
    //init GPIO
    //configure LED as output
    gpio_reset_pin(LED_PIN);
    gpio_set_direction(LED_PIN, GPIO_MODE_OUTPUT);

    //if you need to stop ESP-NOW and WiFi, use the following commands
    //ESP_ERROR_CHECK(esp_now_deinit());
    //ESP_ERROR_CHECK(esp_wifi_stop());
}
