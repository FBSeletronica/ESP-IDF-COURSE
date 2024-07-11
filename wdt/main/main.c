/******************************************************************************
* Watchdog
* This example shows how to use watchdog timer
* This code is part of the course "Programe o ESP32 com ESP-IDF 5" by Fábio Souza
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
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "driver/gpio.h"
#include "esp_task_wdt.h"
#include "rom/ets_sys.h"
#include "esp_log.h"

static const char *TAG = "Test WDT"; 

int32_t i = 0;

#define BUTTON1_PIN 2        // GPIO2

// ISR handler
static void IRAM_ATTR gpio_isr_handler(void* arg)   
{
   i++;
   if(i > 100) while(1);  // Infinite loop if 'i' exceeds 100
}


void task1(void *pvParameters)
{
    esp_task_wdt_add(NULL);  // Add the current task to the Watchdog Timer
    while(1)
    {   
        esp_task_wdt_reset();  // Reset the Watchdog Timer
        for(int x = 0; x <= 1000; x++){
            ets_delay_us(1000);  // Delay for 1 millisecond
        }
        ESP_LOGI(TAG, "Task1 running: %li", i);  // Log the value of 'i'
        vTaskDelay(pdMS_TO_TICKS(1000));  // Delay for 1000 milliseconds
    }
}

void app_main(void)
{
    esp_err_t err;
    esp_task_wdt_config_t twdt_config = {0};

    twdt_config.timeout_ms = 2500;  // Set Watchdog Timer timeout to 2500 ms
    
#if !CONFIG_ESP_TASK_WDT_INIT
    err = esp_task_wdt_init(&twdt_config);  // Initialize the Watchdog Timer
#else
    err = esp_task_wdt_reconfigure(&twdt_config);  // Reconfigure the Watchdog Timer if already initialized
#endif

    // Initialize button
    gpio_reset_pin(BUTTON1_PIN);  // Select BUTTON1_PIN as GPIO
    gpio_set_direction(BUTTON1_PIN, GPIO_MODE_INPUT);  // Set as input
    gpio_pullup_en(BUTTON1_PIN);  // Enable pull-up
    gpio_pulldown_dis(BUTTON1_PIN);  // Disable pull-down
    gpio_set_intr_type(BUTTON1_PIN, GPIO_INTR_NEGEDGE);  // Interrupt on negative edge

    gpio_install_isr_service(ESP_INTR_FLAG_LEVEL1);  // Install interrupt service routine
    gpio_isr_handler_add(BUTTON1_PIN, gpio_isr_handler, (void*) BUTTON1_PIN);  // Add ISR handler for button

    ESP_LOGI(TAG, "Create Task1");  // Log the creation of Task1
    xTaskCreate(task1, "Task1", 2048, NULL, 3, NULL);  // Create Task1 with a stack size of 2048 bytes and priority 3
}
