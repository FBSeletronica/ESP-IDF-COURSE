/******************************************************************************
* ESP-NOW Sender
* This example shows how to use ESP-NOW to send data from one ESP32 to another
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

#define BUTTON_PIN 2

#define TAG "ESP-NOW Sender"

//MAC address of the receiver
uint8_t receiver_mac[6] = {0x34,0xB4,0x72,0x6B,0xD4,0xB2};  //change this to the MAC address of your receiver

//callback function to check if the message was sent
void espnow_send_cb(const uint8_t *mac_addr, esp_now_send_status_t status)
{
    char msg_buffer[50];

    if(status == ESP_NOW_SEND_SUCCESS)
    {
        sprintf(msg_buffer, "Message sent to " MACSTR, MAC2STR(mac_addr));
        ESP_LOGI(TAG, "%s", msg_buffer);
    }
    else
    {
        sprintf(msg_buffer, "Error sending the message to " MACSTR, MAC2STR(mac_addr));
        ESP_LOGE(TAG, "%s", msg_buffer);
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
    ESP_ERROR_CHECK(esp_now_register_send_cb(espnow_send_cb));

    //add peer
    esp_now_peer_info_t peer;
    memset(&peer, 0, sizeof(esp_now_peer_info_t));
    memcpy(peer.peer_addr, receiver_mac, 6);
    esp_now_add_peer(&peer);

    //init GPIO
    //configure button_PIN GPIO as input
    gpio_reset_pin(BUTTON_PIN);                         //reset pin and set as GPIO
    gpio_set_direction(BUTTON_PIN, GPIO_MODE_INPUT);    //set BUTTON_PIN as input
    gpio_set_pull_mode(BUTTON_PIN, GPIO_PULLUP_ONLY);   //set pull-up resistor

    char send_msg_buffer[100];
    uint8_t ledStatus = 0;
    uint8_t button_state = 1;                           //variable to store button state

    while(1)
    {
        int new_state = gpio_get_level(BUTTON_PIN);   //read button state and save in new_state variable

        if (new_state != button_state)              //if button state is different from previous state
        {
            button_state = new_state;               //update button state
            if (button_state == 0)                  //if button is pressed
            {
                ESP_LOGI(TAG,"Button pressed");   //print message on console
                //send message
                ledStatus = !ledStatus;
                sprintf(send_msg_buffer, "LED %s", ledStatus ? "ON" : "OFF");
                esp_now_send(receiver_mac, (uint8_t *)send_msg_buffer, strlen(send_msg_buffer));
            }
            else                                    //if button is not pressed
            {
                ESP_LOGI(TAG,"Button released");    //print message on console
            }
        }
        vTaskDelay(100 / portTICK_PERIOD_MS);       // delay for 100 ms
    }

    //if you need to stop ESP-NOW and Wi-Fi, use the following code
    ESP_ERROR_CHECK(esp_now_deinit());
    ESP_ERROR_CHECK(esp_wifi_stop());
}
