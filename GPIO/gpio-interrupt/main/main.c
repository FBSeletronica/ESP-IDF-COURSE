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
#define LED1_PIN 21          //GPIO21
#define LED2_PIN 33          //GPIO33
#define LED3_PIN 14          //GPIO14
#define BUTTON1_PIN 2        //GPIO2
#define BUTTON2_PIN 3        //GPIO3

static const char* TAG = "BUTTON_TEST";                 //tag for ESP_LOGx

//queue handle
static QueueHandle_t gpio_evt_queue = NULL;                 //queue to handle gpio events

//ISR handler
static void IRAM_ATTR gpio_isr_handler(void* arg)   
{
    uint32_t gpio_num = (uint32_t) arg;                    //get the GPIO number
    xQueueSendFromISR(gpio_evt_queue , &gpio_num, NULL); //send the GPIO number to the queue
}

//button task
void buttonTask(void *pvpameters)
{
    uint32_t gpio_num;                  //gpio number variable
    uint8_t led1 = 0, led2 = 0;         //led state variables
    TickType_t last_button_press = 0;   //last button press time variable

    while (true) 
    {
        xQueueReceive(gpio_evt_queue, &gpio_num, portMAX_DELAY);        //wait for gpio event from ISR
        ESP_LOGI(TAG, "GPIO[%li] intr \n", gpio_num);                   //LOG the GPIO number       
        
        TickType_t current_time = xTaskGetTickCount();                  //get current time

        if (current_time - last_button_press >= pdMS_TO_TICKS(250))     //check if 250ms has passed since last button press
        {
            last_button_press = current_time;                           //update last button press time

            if (gpio_num == BUTTON1_PIN)                                //check if button1 was pressed
            {
                gpio_set_level(LED1_PIN, led1 ^= 1);                    //toggle led1
            }
            else if (gpio_num == BUTTON2_PIN)                           //check if button2 was pressed
            {
                gpio_set_level(LED2_PIN, led2 ^= 1);                    //toggle led2
            }
        }
    }
}

//main function
void app_main(void)
{
    //initialize LED_PIN_1 and LED_PIN_2 as output
    gpio_reset_pin(LED1_PIN);                          //reset pin and set as GPIO
    gpio_set_direction(LED1_PIN, GPIO_MODE_OUTPUT);    //set as output
    gpio_reset_pin(LED2_PIN);                          //reset pin and set as GPIO
    gpio_set_direction(LED2_PIN, GPIO_MODE_OUTPUT);    //set as output   
    gpio_reset_pin(LED3_PIN);                          //reset pin and set as GPIO
    gpio_set_direction(LED3_PIN, GPIO_MODE_OUTPUT);    //set as output 

    //turn off led1, led2 and led3
    gpio_set_level(LED1_PIN, 0);                            //turn off led1
    gpio_set_level(LED2_PIN, 0);                            //turn off led2
    gpio_set_level(LED3_PIN, 0);                            //turn off led3

    //initialize button
    gpio_reset_pin(BUTTON1_PIN);                   //select BUTTON_PIN as GPIO
    gpio_set_direction(BUTTON1_PIN, GPIO_MODE_INPUT);    //set as input
    gpio_pullup_en(BUTTON1_PIN);                         //enable pull-up
    gpio_pulldown_dis(BUTTON1_PIN);                      //disable pull-down
    gpio_set_intr_type(BUTTON1_PIN, GPIO_INTR_NEGEDGE);  //interrupt on negative edge

    //initialize button2
    gpio_reset_pin(BUTTON2_PIN);                  //select BUTTON_PIN2 as GPIO
    gpio_set_direction(BUTTON2_PIN, GPIO_MODE_INPUT);   //set as input
    gpio_pullup_en(BUTTON2_PIN);                        //enable pull-up
    gpio_pulldown_dis(BUTTON2_PIN);                     //disable pull-down
    gpio_set_intr_type(BUTTON2_PIN, GPIO_INTR_NEGEDGE); //interrupt on negative edge

    gpio_evt_queue  = xQueueCreate(1, sizeof(uint32_t));         //create queue to handle gpio event from ISR
    xTaskCreate(buttonTask, "buttonTask", 2048, NULL, 2, NULL);  //create button task

    gpio_install_isr_service(ESP_INTR_FLAG_LEVEL1);                  //install interrupt service routine
    gpio_isr_handler_add(BUTTON1_PIN, gpio_isr_handler, (void*) BUTTON1_PIN);  //add ISR handler for button
    gpio_isr_handler_add(BUTTON2_PIN, gpio_isr_handler, (void*) BUTTON2_PIN); //add ISR handler for button2

    while(true){
        //toogle led3
        gpio_set_level(LED3_PIN, 1);                //turn on led3
        vTaskDelay(pdMS_TO_TICKS(500));             //delay 500ms
        gpio_set_level(LED3_PIN, 0);                //turn off led3
        vTaskDelay(pdMS_TO_TICKS(500));             //delay 500ms
    }

}
