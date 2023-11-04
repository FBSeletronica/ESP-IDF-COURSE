/******************************************************************************
*  Software Timers
* 
*  This example shows how to use software timers one-shot and periodic mode.
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
#include <stdio.h>              // Standard C functions, printf()
#include "freertos/FreeRTOS.h"  // FreeRTOS
#include "freertos/task.h"      // Task functions
#include "freertos/timers.h"

#include "driver/gpio.h"        // GPIO functions
#include "esp_log.h"            // ESP32 logging functions

//pin mapping
#define LED1_PIN 21          //GPIO21
#define LED2_PIN 14          //GPIO33
#define BUTTON1_PIN 2        //GPIO2

TaskHandle_t xTask1;           /*hadle da task1*/
TimerHandle_t xTimer1,xTimer2; /*handle dos timers*/

// Task functions
void task1(void *pvParameter);

// Timer callback functions
void timer1_callback(TimerHandle_t xTimer);
void timer2_callback(TimerHandle_t xTimer);

void app_main(void)
{
    gpio_config_t io_conf = {}; // GPIO config structure

    // Configure LED
    io_conf.pin_bit_mask = (1ULL<<LED1_PIN)|(1ULL<<LED2_PIN) ; // Bitmask to select the GPIOs you want to configure
    io_conf.mode = GPIO_MODE_OUTPUT;      // Set the GPIO mode
    io_conf.pull_up_en = 0;               // Disable pull-up mode
    io_conf.pull_down_en = 0;             // Disable pull-down mode
    io_conf.intr_type = GPIO_INTR_DISABLE; // Disable interrupts
    gpio_config(&io_conf);                 // Configure GPIO with the given settings

    // Configure button
    io_conf.pin_bit_mask = (1ULL<<BUTTON1_PIN); // Bitmask to select the GPIOs you want to configure
    io_conf.mode = GPIO_MODE_INPUT;          // Set the GPIO mode
    io_conf.pull_up_en = 1;                  // Enable pull-up mode
    io_conf.pull_down_en = 0;                // Disable pull-down mode
    io_conf.intr_type = GPIO_INTR_DISABLE;   // Interrupt on negative edge
    gpio_config(&io_conf);                   // Configure GPIO with the given settings

    xTimer1 = xTimerCreate("Timer1", pdMS_TO_TICKS(1000), pdTRUE, (void*)0, timer1_callback);   //create timer1 - periodic mode
    xTimer2 = xTimerCreate("Timer2", pdMS_TO_TICKS(5000), pdFALSE, (void*)0, timer2_callback);   //create timer2 - one-shot mode

    xTaskCreate(task1, "task1", configMINIMAL_STACK_SIZE + 1024, NULL, 1, &xTask1);             //create task1

    xTimerStart(xTimer1, 0);    //start timer1

    vTaskDelete(NULL);          //delete task

}

/*
* Task1
*/
void task1(void *pvParameter)
{
    uint8_t debouncingTime = 0;         //debouncing time counter
    
    while(1)
    {
        if(gpio_get_level(BUTTON1_PIN ) == 0)   //if button is pressed
        {
            debouncingTime++;                   //increment debouncing time counter
            if(debouncingTime >= 10)            //if debouncing time is greater than 100ms (10*10ms)
            {
                gpio_set_level(LED2_PIN , 1);           //turn on LED2
                debouncingTime = 0;                 //reset debouncing time counter
                xTimerStart(xTimer2, 0);            //start timer2
                ESP_LOGI("xTimer2","Timer 2 Start"); //print message on serial monitor
            }
        }
        else{
        debouncingTime = 0;                          //reset debouncing time counter
        }
        vTaskDelay(pdMS_TO_TICKS(10));                   //delay of 10ms
    }
}

/*
* Timer1 callback]
*/
void timer1_callback(TimerHandle_t xTimer)
{
    static uint8_t ledState = 0;    //LED state variable

    gpio_set_level(LED1_PIN, ledState^=1); //set LED1 according to ledState
    ESP_LOGI("xTimer1","Timer 1 Callback"); //print message on serial monitor
}

/*
* Timer2 callback
*/
void timer2_callback(TimerHandle_t xTimer)
{
    gpio_set_level(LED2_PIN, 0);            //turn off LED2
    ESP_LOGI("xTimer2","Timer 2 Callback"); //print message on serial monitor
}