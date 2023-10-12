/******************************************************************************
* temperature sensor example
* This example shows how to use the built-in temperature sensor of ESP32-S2.
* This code is part of the course "Programar o ESP32 com ESP-IDF 5" by Fábio Souza
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
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/temperature_sensor.h"  // temperature sensor driver
#include "esp_log.h"

static const char *TAG = "Temp_Sensor_Example";

void app_main(void)
{
    ESP_LOGI(TAG, "Install temperature sensor, expected temp ranger range: 10~50 ℃");               // log message
    temperature_sensor_handle_t temp_sensor = NULL;                                                 // temperature sensor handler
    temperature_sensor_config_t temp_sensor_config = TEMPERATURE_SENSOR_CONFIG_DEFAULT(10, 50);     // temperature sensor configuration
    ESP_ERROR_CHECK(temperature_sensor_install(&temp_sensor_config, &temp_sensor));                 // install temperature sensor  
    ESP_LOGI(TAG, "Enable temperature sensor");                                                     // log message
    ESP_ERROR_CHECK(temperature_sensor_enable(temp_sensor));                                        // enable temperature sensor

    ESP_LOGI(TAG, "Read temperature");   // log message
    int cnt = 20;                       // counter to read temperature sensor 20 times
    float tsens_value;                  // temperature sensor value

    while (cnt--) {
        ESP_ERROR_CHECK(temperature_sensor_get_celsius(temp_sensor, &tsens_value));     // read temperature sensor
        ESP_LOGI(TAG, "Temperature value %.02f ℃", tsens_value);                       //log message
        vTaskDelay(pdMS_TO_TICKS(1000));                                                // delay 1 second
    }
}
