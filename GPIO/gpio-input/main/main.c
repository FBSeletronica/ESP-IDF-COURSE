/******************************************************************************
* GPIO INPUT
* This example shows how to configure and use GPIO as input on ESP32
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
#include "esp_log.h"

//pins map
#define LED1_PIN 21     //GPIO21
#define LED2_PIN 33     //GPIO33
#define BUTTON_PIN 2    //GPIO2

const static char *TAG = "BUTTON TEST: ";   //for Log messages

//main function
void app_main(void)
{
    //local variables
    bool button_state = 1;  //variable to store button state
    bool i = 0;             //variable to store LED state

    //configure LED_PIN_1 GPIO as input
    gpio_reset_pin(LED1_PIN);                          //reset pin and set as GPIO
    gpio_set_direction(LED1_PIN, GPIO_MODE_OUTPUT);   //set LED_PIN_1 as output

    //configure button_PIN GPIO as input
    gpio_reset_pin(BUTTON_PIN);                         //reset pin and set as GPIO
    gpio_set_direction(BUTTON_PIN, GPIO_MODE_INPUT);    //set BUTTON_PIN as input
    gpio_set_pull_mode(BUTTON_PIN, GPIO_PULLUP_ONLY);   //set pull-up resistor

    while(true) //infinite loop
    {
        int new_state = gpio_get_level(BUTTON_PIN);   //read button state and save in new_state variable

        if (new_state != button_state)              //if button state is different from previous state
        {
            button_state = new_state;               //update button state
            if (button_state == 0)                  //if button is pressed
            {
                gpio_set_level(LED1_PIN, i^=1);      //toggle LED state
                ESP_LOGI(TAG,"Button pressed");   //print message on console
            }
            else                                    //if button is not pressed
            {
                ESP_LOGI(TAG,"Button released");    //print message on console
            }
        }
        vTaskDelay(100 / portTICK_PERIOD_MS);       // delay for 100 ms
    }
}
