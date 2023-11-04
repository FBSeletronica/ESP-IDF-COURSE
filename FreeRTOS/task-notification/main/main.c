/******************************************************************************
*  Task Notifications Example
* 
*  This example shows how to use direct task notifications to synchronize an ISR with a task.
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
#include <stdio.h>                  // Standard C functions, printf()
#include "freertos/FreeRTOS.h"      // FreeRTOS
#include "freertos/task.h"          // Task functions

#include "driver/gpio.h"        // GPIO functions
#include "esp_log.h"            // ESP32 logging functions

//pin mapping
#define LED1_PIN 21          //GPIO21
#define LED2_PIN 14          //GPIO33
#define BUTTON1_PIN 2        //GPIO2

static const char *TAG = "Task Notifications example";

//Task handles
TaskHandle_t xTask1Handle = NULL;

//Task prototypes
void vTask1(void *pvParameters);

static void IRAM_ATTR gpio_isr_handler(void* arg)
{
    BaseType_t xHigherPriorityTaskWoken = pdTRUE;

    if(BUTTON1_PIN == (uint32_t) arg)
    {
        //Notify task
        xTaskNotifyFromISR(xTask1Handle, 1, eSetValueWithOverwrite, &xHigherPriorityTaskWoken);
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

    xTaskCreate( vTask1, "Task1", configMINIMAL_STACK_SIZE + 1024, NULL, 2, &xTask1Handle );

    vTaskDelete(NULL);
}


void vTask1(void *pvParameters)
{
    uint32_t ulNotifiedValue;

    while(1)
    {
        //Wait for notification
        ulNotifiedValue = ulTaskNotifyTake(pdTRUE, portMAX_DELAY);

        if(ulNotifiedValue == 1)
        {
            ESP_LOGI(TAG, "Button pressed");
            gpio_set_level(LED1_PIN, 1);
            vTaskDelay(1000 / portTICK_PERIOD_MS);
            gpio_set_level(LED1_PIN, 0);
        }
    }
}