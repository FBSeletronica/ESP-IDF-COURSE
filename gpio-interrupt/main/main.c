/******************************************************************************
* GPIO INPUT - Interrupt
* This example shows how to configure and use GPIO input with external interrupt on ESP32.
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

//freeRTOS includes
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"

//ESP32 includes
#include "driver/gpio.h"
#include "esp_log.h"

//pin mapping
#define LED_PIN_1 1     //GPIO1
#define LED_PIN_2 2     //GPIO2
#define BUTTON_PIN 6    //GPIO6
#define BUTTON_PIN2 7   //GPIO7

//queue handle
xQueueHandle gpio_evt_queue = NULL;                 //queue to handle gpio events

//ISR handler
static void IRAM_ATTR gpio_isr_handler(void* arg)   
{
    uint32_t gpio_num = (uint32_t) arg;                    //get the GPIO number
    xQueueSendFromISR(gpio_evt_queue , &gpio_num, NULL); //send the GPIO number to the queue
}

//button task
void buttonTask(void *pvpameters)
{
    uint32_t gpio_num;                          //variable to store the GPIO number
    uint8_t led1 = 0, led2 = 0;                 //variables to store the state of the LEDs

    while (1)
    {
        xQueueReceive(gpio_evt_queue, &gpio_num, portMAX_DELAY);                                //wait for a value on queue
        ESP_LOGI("buttonTask", "GPIO[%d] intr, val: %d\n", gpio_num, gpio_get_level(gpio_num)); //print message on console

        if (gpio_num == BUTTON_PIN)                     //if BUTTON_PIN 
        {
            gpio_set_level(LED_PIN_1, led1 ^= 1);       //toggle LED_PIN_1
        }
        else if (gpio_num == BUTTON_PIN2)               //else if BUTTON_PIN2 
        {
            gpio_set_level(LED_PIN_2, led2 ^= 1);       //toggle LED_PIN_2
        }
    }
}

//main function
void app_main(void)
{
    //initialize LED_PIN_1 and LED_PIN_2 as output
    gpio_pad_select_gpio(LED_PIN_1);                    //select LED_PIN_1 as GPIO
    gpio_set_direction(LED_PIN_1, GPIO_MODE_OUTPUT);    //set as output
    gpio_pad_select_gpio(LED_PIN_2);                    //select LED_PIN_2 as GPIO
    gpio_set_direction(LED_PIN_2, GPIO_MODE_OUTPUT);    //set as output    

    //initialize button
    gpio_pad_select_gpio(BUTTON_PIN);                   //select BUTTON_PIN as GPIO
    gpio_set_direction(BUTTON_PIN, GPIO_MODE_INPUT);    //set as input
    gpio_pullup_en(BUTTON_PIN);                         //enable pull-up
    gpio_pulldown_dis(BUTTON_PIN);                      //disable pull-down
    gpio_set_intr_type(BUTTON_PIN, GPIO_INTR_NEGEDGE);  //interrupt on negative edge

    //initialize button2
    gpio_pad_select_gpio(BUTTON_PIN2);                  //select BUTTON_PIN2 as GPIO
    gpio_set_direction(BUTTON_PIN2, GPIO_MODE_INPUT);   //set as input
    gpio_pullup_en(BUTTON_PIN2);                        //enable pull-up
    gpio_pulldown_dis(BUTTON_PIN2);                     //disable pull-down
    gpio_set_intr_type(BUTTON_PIN2, GPIO_INTR_NEGEDGE); //interrupt on negative edge

    gpio_evt_queue  = xQueueCreate(5, sizeof(uint32_t));         //create queue
    xTaskCreate(buttonTask, "buttonTask", 2048, NULL, 2, NULL); //create task

    
    gpio_install_isr_service(ESP_INTR_FLAG_LEVEL1);                  //install interrupt service routine
    gpio_isr_handler_add(BUTTON_PIN, gpio_isr_handler, (void*) BUTTON_PIN);  //add ISR handler for button
    gpio_isr_handler_add(BUTTON_PIN2, gpio_isr_handler, (void*) BUTTON_PIN2); //add ISR handler for button2

}
