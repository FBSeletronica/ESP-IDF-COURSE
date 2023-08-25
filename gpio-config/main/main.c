/******************************************************************************
* GPIO config
* This example shows how to configure and use GPIO with gpio_config function.
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
#define LED1_PIN 21          //GPIO21
#define LED2_PIN 33         //GPIO33
#define BUTTON1_PIN 2        //GPIO2
#define BUTTON2_PIN 3        //GPIO3

static const char* TAG = "BUTTON_TEST";

//queue handle
static QueueHandle_t gpio_evt_queue = NULL;             //queue to handle gpio events

//ISR handler
static void IRAM_ATTR gpio_isr_handler(void* arg)
{
    uint32_t gpio_num = (uint32_t) arg;                     //get the GPIO number
    xQueueSendFromISR(gpio_evt_queue , &gpio_num, NULL);    //send the GPIO number to the queue
}

//button task    
void buttonTask(void *pvpameters)
{
    uint32_t gpio_num;                                //variable to store the GPIO number
    uint8_t led1 = 0,led2=0;                         //auxiliar variables

    while(true) 
    {
        xQueueReceive(gpio_evt_queue , &gpio_num, portMAX_DELAY);       //wait for data in queue
        ESP_LOGI(TAG , "GPIO[%li] intr, val: %d\n", gpio_num, gpio_get_level(gpio_num));   //print message

        if (gpio_num == BUTTON1_PIN)             //if button 
        {
            gpio_set_level(LED1_PIN, led1^=1); //toggle LED1
        }
        else if (gpio_num == BUTTON2_PIN)       //if button2
        {
            gpio_set_level(LED2_PIN, led2^=1); //toggle LED2
        }
    }
}

//main function
void app_main(void)
{
    gpio_config_t io_config = {};                              //GPIO configuration structure

    //gpio_pad_select_gpio(LED);
    //gpio_set_direction(LED, GPIO_MODE_OUTPUT);
    //gpio_pad_select_gpio(LED_PIN_2);
    //gpio_set_direction(LED_PIN_2, GPIO_MODE_OUTPUT);
    io_config.pin_bit_mask = (1ULL<<LED1_PIN) | (1ULL<<LED2_PIN); //set GPIO1 and GPIO2 as output
    io_config.mode = GPIO_MODE_OUTPUT;                      //set GPIO1 and GPIO2 as output
    io_config.pull_up_en = GPIO_PULLUP_DISABLE;             //disable pull-up
    io_config.pull_down_en = GPIO_PULLDOWN_DISABLE;         //disable pull-down
    io_config.intr_type = GPIO_INTR_DISABLE;                //disable interrupt
    gpio_config(&io_config);                                //configure GPIO1 and GPIO2


    //gpio_pad_select_gpio(BUTTON);
    //gpio_set_direction(BUTTON, GPIO_MODE_INPUT);
    //gpio_pullup_en(BUTTON);
    //gpio_pulldown_dis(BUTTON);
    //gpio_set_intr_type(BUTTON, GPIO_INTR_NEGEDGE);
    
    io_config.pin_bit_mask = (1ULL<<BUTTON1_PIN)|(1ULL<<BUTTON2_PIN); //set GPIO6 and GPIO7 as input
    io_config.mode = GPIO_MODE_INPUT;                               //set GPIO6 and GPIO7 as input
    io_config.pull_up_en = GPIO_PULLUP_ENABLE;                      //enable pull-up
    io_config.pull_down_en = GPIO_PULLDOWN_DISABLE;                 //disable pull-down
    io_config.intr_type = GPIO_INTR_NEGEDGE;                        //enable interrupt on negative edge
    gpio_config(&io_config);                                        //configure GPIO6 and GPIO7
   
    gpio_evt_queue = xQueueCreate(3, sizeof(uint32_t));             //create queue to handle gpio events
    xTaskCreate(buttonTask, "buttonTask", 2048, NULL, 2, NULL);     //create button task

    gpio_install_isr_service(ESP_INTR_FLAG_LEVEL1);                         //install gpio isr service
    gpio_isr_handler_add(BUTTON1_PIN, gpio_isr_handler, (void*) BUTTON1_PIN);  //add isr handler for button1
    gpio_isr_handler_add(BUTTON2_PIN, gpio_isr_handler, (void*) BUTTON2_PIN); //add isr handler for button2
}
