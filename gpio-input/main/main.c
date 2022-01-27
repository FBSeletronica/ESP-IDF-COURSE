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
#define LED_PIN_1 1     //GPIO1
#define LED_PIN_2 2     //GPIO2
#define BUTTON_PIN 6    //GPIO6

//main function
void app_main(void)
{
    //configure LED_PIN_1 GPIO as input
    gpio_pad_select_gpio(LED_PIN_1);                    //select LED_PIN_1 as GPIO
    gpio_set_direction(LED_PIN_1, GPIO_MODE_OUTPUT);   //set LED_PIN_1 as output

    //configure button_PIN GPIO as input
    gpio_pad_select_gpio(BUTTON_PIN);                   //select BUTTON_PIN as GPIO
    gpio_set_direction(BUTTON_PIN, GPIO_MODE_INPUT);    //set BUTTON_PIN as input
    gpio_set_pull_mode(BUTTON_PIN, GPIO_PULLUP_ONLY);   //set pull-up resistor

    while(true) //infinite loop
    {
       
        if(gpio_get_level(BUTTON_PIN) == 0)         // if button is pressed
        {
            gpio_set_level(LED_PIN_1, 1);           // turn on LED    
            ESP_LOGI("Button", "Button pressed");   // print message on console
        }
        else                                        // if button is not pressed
        {
            gpio_set_level(LED_PIN_1, 0);                 // turn off LED
            ESP_LOGI("Button", "Button released");  // print message on console
        }

        vTaskDelay(100 / portTICK_PERIOD_MS);       // delay for 100 ms
    }
}
