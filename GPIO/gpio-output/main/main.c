/******************************************************************************
* GPIO OUTPUT
* This example shows how to configure and use GPIO output on ESP32
* This code is part of the course "Porgrame o ESP32 com ESP-IDF 5" by Fábio Souza
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

//pin definitions
#define LED_PIN_1 21        //define LED_PIN_1 as GPIO21
#define LED_PIN_2 33        //define LED_PIN_2 as GPIO33

void app_main(void)
{
    //configure GPIO1 as output
    gpio_reset_pin(LED_PIN_1);                          //reset GPIO to default state
    gpio_set_direction(LED_PIN_1, GPIO_MODE_OUTPUT);    //set LED_PIN_1 as output

    gpio_reset_pin(LED_PIN_2);                         //reset GPIO to default state
    gpio_set_direction(LED_PIN_2, GPIO_MODE_OUTPUT);    //set LED_PIN_2 as output

    while(1) //infinite loop
    {
        gpio_set_level(LED_PIN_1, 1);                   //turn LED_PIN_1 on
        gpio_set_level(LED_PIN_2, 0);                   //turn LED_PIN_2 off
        printf("LED ON\n");                             //print message on console
        vTaskDelay(1000 / portTICK_PERIOD_MS);          //delay 1 second
        gpio_set_level(LED_PIN_1, 0);                   //turn LED_PIN_1 off
        gpio_set_level(LED_PIN_2, 1);                   //turn LED_PIN_2 on    
        printf("LED OFF\n");                            //print message on console
        vTaskDelay(1000 / portTICK_PERIOD_MS);          //delay 1 second
    }
}
