/******************************************************************************
* ds18b20 - 1-Wire Digital Thermometer
* This is an example application for the Maxim Integrated DS18B20 Programmable Resolution 1-Wire Digital Thermometer device.
* Components Repositories: 
    ds18b20: https://github.com/UncleRus/esp-idf-lib

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

#include <inttypes.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <ds18x20.h>
#include <esp_log.h>
#include <esp_err.h>

static const gpio_num_t SENSOR_GPIO = CONFIG_EXAMPLE_ONEWIRE_GPIO;

// Use address of your own sensor here!
// You can find out the address of your sensor by running ds18x20_multi example
static const onewire_addr_t SENSOR_ADDR = CONFIG_EXAMPLE_DS18X20_ADDR;

static const char *TAG = "ds18x20_test";

void test(void *pvParameter)
{
    // Make sure that the internal pull-up resistor is enabled on the GPIO pin
    // so that one can connect up a sensor without needing an external pull-up.
    // (Note: The internal (~47k) pull-ups of the ESP do appear to work, at
    // least for simple setups (one or two sensors connected with short leads),
    // but do not technically meet the pull-up requirements from the ds18x20
    // datasheet and may not always be reliable. For a real application, a proper
    // 4.7k external pull-up resistor is recommended instead!)
    gpio_set_pull_mode(SENSOR_GPIO, GPIO_PULLUP_ONLY);

    float temperature;
    esp_err_t res;
    while (1)
    {
        res = ds18x20_measure_and_read(SENSOR_GPIO, SENSOR_ADDR, &temperature);
        if (res != ESP_OK)
            ESP_LOGE(TAG, "Could not read from sensor %08" PRIx32 "%08" PRIx32 ": %d (%s)",
                    (uint32_t)(SENSOR_ADDR >> 32), (uint32_t)SENSOR_ADDR, res, esp_err_to_name(res));
        else
            ESP_LOGI(TAG, "Sensor %08" PRIx32 "%08" PRIx32 ": %.2f°C",
                    (uint32_t)(SENSOR_ADDR >> 32), (uint32_t)SENSOR_ADDR, temperature);

        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

void app_main()
{
    xTaskCreate(test, TAG, configMINIMAL_STACK_SIZE * 4, NULL, 5, NULL);
}
