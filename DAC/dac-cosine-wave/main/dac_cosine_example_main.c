/******************************************************************************
* DAC - Cosine Wave
* This example shows how to configure and use DAC peripheral with a cosine wave feature.
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

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/dac_cosine.h"
#include "esp_adc/adc_oneshot.h"
#include "esp_log.h"

static const char *TAG = "dac_cosine_example";

void app_main(void)
{
    dac_cosine_handle_t chan0_handle;
    dac_cosine_config_t cos0_cfg = {
        .chan_id = DAC_CHAN_0,                  // DAC channel 0
        .freq_hz = 1000,                        // 1 kHz
        .clk_src = DAC_COSINE_CLK_SRC_DEFAULT,  // default clock source
        .offset = 0,                            // 0 V    
        .phase = DAC_COSINE_PHASE_0,            // 0 degrees
        .atten = DAC_COSINE_ATTEN_DEFAULT,      // default attenuation
        .flags.force_set_freq = false,          // do not force frequency
    };
    
    ESP_LOGI(TAG, "Initializing DAC cosine wave generator...");         

    ESP_ERROR_CHECK(dac_cosine_new_channel(&cos0_cfg, &chan0_handle));  // create DAC channel 0
    ESP_ERROR_CHECK(dac_cosine_start(chan0_handle));                    // start DAC channel 0

    ESP_LOGI(TAG, "DAC cosine wave generator initialized.");
}
