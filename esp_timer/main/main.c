/******************************************************************************
*  ESP Timer - High Resolution Timer
* 
*  This example shows how to use the ESP32 timer to generate a high resolution timer.
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

#include "esp_timer.h"
#include "driver/gpio.h"        // GPIO functions
#include "esp_log.h"            // ESP32 logging functions

#define LED1_PIN 14             // GPIO14 - LED1

static const char* TAG = "ESP Timer Example";

void timer1_callback(void *arg)
{
    static bool led = 0;
    gpio_set_level(LED1_PIN, led^=1);
}

void app_main(void)
{

    gpio_set_direction(LED1_PIN, GPIO_MODE_OUTPUT);

    const esp_timer_create_args_t timer_args = {
        .callback = &timer1_callback,
        .name = "timer1"
    };

    esp_timer_handle_t timer1;
    ESP_ERROR_CHECK(esp_timer_create(&timer_args, &timer1));
    ESP_ERROR_CHECK(esp_timer_start_periodic(timer1, 1)); // 100 us

    vTaskDelay(1000 / portTICK_PERIOD_MS);

    esp_timer_stop(timer1);
    esp_timer_delete(timer1);

}
