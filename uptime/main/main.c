/******************************************************************************
* UPTIME
* This example shows how to get the uptime
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

#include "esp_timer.h"
#include "esp_log.h"

void app_main(void)
{
    // Get the current uptime in microseconds
    int64_t uptime = esp_timer_get_time();

    // Log the uptime in milliseconds
    ESP_LOGI(__func__, "Uptime: %lli ms", uptime / 1000);

    // Delay for 5000 milliseconds (5 seconds)
    vTaskDelay(pdMS_TO_TICKS(5000));

    // Get the current uptime again in microseconds
    uptime = esp_timer_get_time();

    // Log the uptime again in milliseconds
    ESP_LOGI(__func__, "Uptime: %lli ms", uptime / 1000);
}

