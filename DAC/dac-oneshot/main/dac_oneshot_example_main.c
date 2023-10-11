/******************************************************************************
* DAC(Digital to Analog Converter) oneshot example
* This example shows how to configure and use DAC peripheral.
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
#include "driver/dac_oneshot.h"
#include "esp_log.h"

static const char *TAG = "dac_oneshot_example";     // Tag for logging

void app_main(void)
{
    uint32_t val = 0;         

    /* DAC oneshot init */
    dac_oneshot_handle_t chan0_handle;      // DAC channel 0 handle
    dac_oneshot_config_t chan0_cfg = {      // DAC channel 0 configuration
        .chan_id = DAC_CHAN_0,              // DAC channel 0
    };
    ESP_ERROR_CHECK(dac_oneshot_new_channel(&chan0_cfg, &chan0_handle));    // Create DAC channel 0
    
    ESP_LOGI(TAG, "DAC oneshot example start.");                            // Log start of example

   while(1){
        // Ramp up and down the DAC output voltage
        for(val = 0;val<255;val++){
        ESP_ERROR_CHECK(dac_oneshot_output_voltage(chan0_handle, val));
        vTaskDelay(pdMS_TO_TICKS(1));
        }

        // Ramp down the DAC output voltage
        for(val = 255;val>0;val--){
        ESP_ERROR_CHECK(dac_oneshot_output_voltage(chan0_handle, val));
        vTaskDelay(pdMS_TO_TICKS(1));
        }
   }
}
