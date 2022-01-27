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
#define LED 3 //GPIO3
#define BUTTON 6 //GPIO6
#define BUTTON2 7 //GPIO7

//queue handle
xQueueHandle gpio_evt_queue = NULL;                 //queue to handle gpio events

//ISR handler
static void IRAM_ATTR gpio_isr_handler(void* arg)   
{
    uint8_t gpio_num = (uint8_t) arg;                    //get the GPIO number
    xQueueSendFromISR(gpio_evt_queue , &gpio_num, NULL); //send the GPIO number to the queue
}

//button task
void buttonTask(void *pvpameters)
{
    uint8_t i=0;      //auxiliar variable
    uint8_t gpio_num; //variable to store the GPIO number

    while(true) //infinite loop
    {
        xQueueReceive(gpio_evt_queue , &gpio_num, portMAX_DELAY);    //wait for data in queue
        ESP_LOGI("TASK BUTTON","Button (GPIO %d) pressed", gpio_num); //print message
        gpio_set_level(LED,i^=1);                                     //toggle LED
    }
}

//main function
void app_main(void)
{
    //initialize LED
    gpio_pad_select_gpio(LED);                  //select GPIO3
    gpio_set_direction(LED, GPIO_MODE_OUTPUT);  //set as output

    //initialize button
    gpio_pad_select_gpio(BUTTON);                   //select GPIO6
    gpio_set_direction(BUTTON, GPIO_MODE_INPUT);    //set as input
    gpio_pullup_en(BUTTON);                         //enable pull-up
    gpio_pulldown_dis(BUTTON);                      //disable pull-down
    gpio_set_intr_type(BUTTON, GPIO_INTR_NEGEDGE);  //interrupt on negative edge

    //initialize button2
    gpio_pad_select_gpio(BUTTON2);                  //select GPIO7
    gpio_set_direction(BUTTON2, GPIO_MODE_INPUT);   //set as input
    gpio_pullup_en(BUTTON2);                        //enable pull-up
    gpio_pulldown_dis(BUTTON2);                     //disable pull-down
    gpio_set_intr_type(BUTTON2, GPIO_INTR_NEGEDGE); //interrupt on negative edge

    gpio_evt_queue  = xQueueCreate(5, sizeof(uint8_t));         //create queue
    xTaskCreate(buttonTask, "buttonTask", 2048, NULL, 2, NULL); //create task

    
    gpio_install_isr_service(ESP_INTR_FLAG_LEVEL1);                  //install interrupt service routine
    gpio_isr_handler_add(BUTTON, gpio_isr_handler, (void*) BUTTON);  //add ISR handler for button
    gpio_isr_handler_add(BUTTON2, gpio_isr_handler, (void*) BUTTON2); //add ISR handler for button2

}
