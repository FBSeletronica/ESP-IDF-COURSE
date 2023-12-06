/******************************************************************************
* MQTT Last Will and Testament (LWT)
* This example shows how to use MQTT Last Will and Testament (LWT) feature.
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
#include "esp_random.h"

#include "wifi.h"                //wifi component
#include "mqtt_app.h"            //mqtt component

//tag for logging
static const char *TAG = "MQTT Example";

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
    ESP_LOGI(TAG, "ESP_WIFI_MODE_STA iniciado...");                     //log wifi station mode started 

    mqtt_app_start();                                                   //start mqtt client 
    ESP_LOGI(TAG, "MQTT iniciado...");                                  //log mqtt started

    mqtt_app_subscribe("asdfg/led1",0);                                 //subscribe to topic esp32/led with qos 0
    mqtt_app_subscribe("asdfg/led2",0);                                 //subscribe to topic esp32/led2 with qos 0

    while(1)
    {
        static int i = 0;                                                   //static variable to count iterations
        int temperature = esp_random() % 40;                                 //generate random number between 0 and 40
        char temperature_str[10];                                           //string to store temperature      
        sprintf(temperature_str,"%d",temperature);                          //convert temperature to string
        if(mqtt_is_connected())                                             //if mqtt is connected
        {
            mqtt_app_publish("asdfg/temperature",temperature_str,1,1);      //publish message "25" to topic "esp32/temperature" with qos 0 and retain 0
            ESP_LOGI(TAG, "Temperatura: %d",temperature);                   //log temperature
        }
       vTaskDelay(10000 / portTICK_PERIOD_MS);                             //delay 1s
       
       if(i++ == 3) break;
    }   

    mqtt_app_stop();                                                      //stop mqtt client                                  
}
