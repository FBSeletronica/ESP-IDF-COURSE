/******************************************************************************
* GPIO OUTPUT
* This example shows how to configure and use GPIO output on ESP32
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

// define GPIO pin
#define LED_GPIO 3     // GPIO3

//main function
void app_main(void)
{
    //configure GPIO as output
    gpio_pad_select_gpio(LED_GPIO);                 // select GPIO number
    gpio_set_direction(LED_GPIO, GPIO_MODE_OUTPUT); // set direction as output

    while(1) //infinite loop
    {
        gpio_set_level(LED_GPIO, 1);            // turn on LED
        vTaskDelay(1000 / portTICK_PERIOD_MS);  // delay for 1 second
        gpio_set_level(LED_GPIO, 0);            // turn off LED
        vTaskDelay(1000 / portTICK_PERIOD_MS);  // delay for 1 second
        printf("Blink\n");                      // print message on console
    }
}
