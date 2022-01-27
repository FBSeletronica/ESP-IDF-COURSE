/******************************************************************************
* temperature sensor example
* This example shows how to use the built-in temperature sensor of ESP32-S2.
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
//standard includes
#include <stdio.h>
#include <stdlib.h>
// FreeRTOS includes
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
// ESP32 includes
#include "driver/temp_sensor.h"
#include "esp_log.h"


static const char *TAG = "TempSensor";      //tag for logging

void app_main(void)
{
   
    ESP_LOGI(TAG, "Initializing Temperature sensor");   //log message
    temp_sensor_config_t temp_sensor = TSENS_CONFIG_DEFAULT();  //configure temperature sensor
    temp_sensor.dac_offset = TSENS_DAC_DEFAULT;                // DEFAULT: range:-10℃ ~  80℃, error < 1℃.
    temp_sensor_set_config(temp_sensor);
    temp_sensor_start();

    ESP_LOGI(TAG, "Temperature sensor started");


    while (1) {
        float temp_out;                                            //temperature sensor output variable
        temp_sensor_read_celsius(&temp_out);                      //read temperature sensor output
        ESP_LOGI(TAG, "Temperature out celsius %f°C", temp_out);   //log message
        vTaskDelay(1000 / portTICK_RATE_MS);                       //delay 1 second
    }
}

