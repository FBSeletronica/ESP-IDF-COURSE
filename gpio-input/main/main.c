/******************************************************************************
* GPIO INPUT
* This example shows how to configure and use GPIO as input on ESP32
* This code is part of the course "Aprenda programar o ESP32 com ESP-IDF" by Fábio Souza
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
// freeRTOS Libraries
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

// ESP32 Libraries
#include "driver/gpio.h"
#include "esp_log.h"

//pins map
#define LED 3       // GPIO3
#define BUTTON 6   //GPIO6

//main function
void app_main(void)
{
    //configure LED GPIO as input
    gpio_pad_select_gpio(LED);                  // select GPIO number
    gpio_set_direction(LED, GPIO_MODE_OUTPUT);  // set direction as output

    //configure button GPIO as input
    gpio_pad_select_gpio(BUTTON);                   // select GPIO number
    gpio_set_direction(BUTTON, GPIO_MODE_INPUT);    // set direction as input 
    gpio_pullup_en(BUTTON);                         // enable pull-up resistor
    gpio_pulldown_dis(BUTTON);                      // disable pull-down resistor

    while(true) //infinite loop
    {
       
        if(gpio_get_level(BUTTON) == 0)             // if button is pressed
        {
            gpio_set_level(LED, 1);                 // turn on LED    
            ESP_LOGI("Button", "Button pressed");   // print message on console
        }
        else                                        // if button is not pressed
        {
            gpio_set_level(LED, 0);                 // turn off LED
            ESP_LOGI("Button", "Button released");  // print message on console
        }

        vTaskDelay(100 / portTICK_PERIOD_MS);       // delay for 100 ms
    }
}
