/******************************************************************************
* Queue from ISR Example
* 
*  This example shows how to use queue to comunication between ISR and a task
*  This code is part of the course "Programe o ESP32 com ESP-IDF 5" by Fábio Souza
*  The course is available on https://cursos.embarcados.com.br
* 
*  This example code Creative Commons Attribution 4.0 International License.
*  When using the code, you must keep the above copyright notice,
*  this list of conditions and the following disclaimer in the source code.
*  (http://creativecommons.org/licenses/by/4.0/)
*
* Author: Fábio Souza
* This code is for teaching purposes only.
* No warranty of any kind is provided.
*******************************************************************************/
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "driver/gpio.h"
#include "esp_log.h"

//pin mapping
#define LED1_PIN 21          //GPIO21
#define LED2_PIN 33          //GPIO33
#define LED3_PIN 14          //GPIO14
#define LED4_PIN 13          //GPIO15
#define BUTTON1_PIN 2        //GPIO2
#define BUTTON2_PIN 3        //GPIO3

static const char* TAG = "BUTTON_TEST";                 //tag for ESP_LOGx

QueueHandle_t gpio_evt_queue = NULL;            // Queue handle
void buttonTask(void *pvParameter); // Task 1 function declaration
void ledTask(void *pvParameter);    // Task 2 function declaration

// Interrupt handler
static void IRAM_ATTR gpio_isr_handler(void *arg)
{
    uint32_t gpio_num = (uint32_t) arg;                     //get the GPIO number
    BaseType_t xHigherPriorityTaskWoken = pdTRUE;
    xQueueSendFromISR(gpio_evt_queue , &gpio_num, NULL);    //send the GPIO number that generated the interrupt to the queue

    if(xHigherPriorityTaskWoken == pdTRUE)                  //check if the interrupt raised the task priority
    {
        portYIELD_FROM_ISR();                               //force context switch
    }
}

void app_main(void)
{

    gpio_config_t io_conf = {}; // GPIO config structure

    // Configure LED
    io_conf.pin_bit_mask = (1ULL<<LED1_PIN) | (1ULL<<LED2_PIN) | (1ULL<<LED3_PIN)| (1ULL<<LED4_PIN); // Bitmask to select the GPIOs you want to configure
    io_conf.mode = GPIO_MODE_OUTPUT;      // Set the GPIO mode
    io_conf.pull_up_en = 0;               // Disable pull-up mode
    io_conf.pull_down_en = 0;             // Disable pull-down mode
    io_conf.intr_type = GPIO_INTR_DISABLE; // Disable interrupts
    gpio_config(&io_conf);                 // Configure GPIO with the given settings

    // Configure button
    io_conf.pin_bit_mask = (1ULL<<BUTTON1_PIN)|(1ULL<<BUTTON2_PIN); // Bitmask to select the GPIOs you want to configure
    io_conf.mode = GPIO_MODE_INPUT;          // Set the GPIO mode
    io_conf.pull_up_en = 1;                  // Enable pull-up mode
    io_conf.pull_down_en = 0;                // Disable pull-down mode
    io_conf.intr_type = GPIO_INTR_NEGEDGE;   // Interrupt on negative edge
    gpio_config(&io_conf);                   // Configure GPIO with the given settings

    //install gpio isr service and add isr handler for button1 and button2
    gpio_install_isr_service(ESP_INTR_FLAG_LEVEL1);                         //install gpio isr service
    gpio_isr_handler_add(BUTTON1_PIN, gpio_isr_handler, (void*) BUTTON1_PIN);  //add isr handler for button1
    gpio_isr_handler_add(BUTTON2_PIN, gpio_isr_handler, (void*) BUTTON2_PIN); //add isr handler for button2

    gpio_evt_queue = xQueueCreate(2, sizeof(uint32_t));             //create queue to handle gpio events
    xTaskCreate(buttonTask, "buttonTask", 2048, NULL, 2, NULL);     //create button task
    xTaskCreate(ledTask, "ledTask", 2048, NULL, 1, NULL);           //create led task

    while(true){
        //toogle led3
        gpio_set_level(LED3_PIN, 1);                //turn on led3
        vTaskDelay(pdMS_TO_TICKS(500));             //delay 500ms
        gpio_set_level(LED3_PIN, 0);                //turn off led3
        vTaskDelay(pdMS_TO_TICKS(500));             //delay 500ms
    }
}

void ledTask(void *pvParameter)
{
    uint8_t led = 0;                   //led state variable

    while (true) 
    {
        led ^= 1;                      //toggle led3
        gpio_set_level(LED4_PIN, led); //set led3 state
        vTaskDelay(pdMS_TO_TICKS(100)); //delay 100ms
    }
}


void buttonTask(void *pvParameter)
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
