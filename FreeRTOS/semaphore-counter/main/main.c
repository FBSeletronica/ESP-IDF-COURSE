/******************************************************************************
*  Counting Semaphore Example
* 
*  This example shows how to use counting semaphores to synchronize an ISR with a task.
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
#include "freertos/semphr.h"    // Semaphore functions

#include "driver/gpio.h"        // GPIO functions
#include "esp_log.h"            // ESP32 logging functions

//pin mapping
#define LED1_PIN 21          //GPIO21
#define LED2_PIN 33          //GPIO33
#define LED3_PIN 14          //GPIO14
#define LED4_PIN 13          //GPIO15
#define BUTTON1_PIN 2        //GPIO2
#define BUTTON2_PIN 3        //GPIO3

static const char *TAG = "Semaphore Counting: "; // Logging tag
SemaphoreHandle_t xCoutingSemaphore = NULL;    // Semaphore handle

// Task to be created.
void led_task(void *pvParameter);

// Interrupt handler for button
static void IRAM_ATTR gpio_isr_handler(void* arg)
{
    BaseType_t xHigherPriorityTaskWoken = pdTRUE;
    
    if(BUTTON1_PIN == (uint32_t) arg)                   // Check if the interrupt is for button1
    {
        // Notify the task
        xSemaphoreGiveFromISR(xCoutingSemaphore, &xHigherPriorityTaskWoken);   // Give the semaphore
    }
    
}

void app_main(void)
{
    gpio_config_t io_conf = {}; // GPIO config structure

    // Configure LED
    io_conf.pin_bit_mask = (1ULL<<LED1_PIN) ; // Bitmask to select the GPIOs you want to configure
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
    io_conf.intr_type = GPIO_INTR_NEGEDGE;   // Interrupt on negative edge
    gpio_config(&io_conf);                   // Configure GPIO with the given settings

    //install gpio isr service and add isr handler for button1 and button2
    gpio_install_isr_service(ESP_INTR_FLAG_LEVEL1);                         //install gpio isr service
    gpio_isr_handler_add(BUTTON1_PIN, gpio_isr_handler, (void*) BUTTON1_PIN);  //add isr handler for button1
    
    // Create a binary semaphore
    xCoutingSemaphore = xSemaphoreCreateCounting(255, 0); // Create a counting semaphore

    // Create a task to wait for the semaphore
    xTaskCreate(&led_task, "led_task", configMINIMAL_STACK_SIZE+1024, NULL, 5, NULL);

    vTaskDelete(NULL);  // Delete this task
    
}


void led_task(void *pvParameter)
{
    UBaseType_t x = 0; // LED status

    while(1)
    {
        // Wait for the semaphore
        if(xSemaphoreTake(xCoutingSemaphore, portMAX_DELAY) == pdTRUE) // Wait for the semaphore
        {
            x = uxSemaphoreGetCount(xCoutingSemaphore); // Get the semaphore count
            gpio_set_level(LED1_PIN, x%2);    // Toggle LED
            ESP_LOGI(TAG, "Counting: %d", x);       // Log LED status
            vTaskDelay(1000 / portTICK_PERIOD_MS); // Delay
        }
    }
}