/******************************************************************************
* ADC oneshot - (uncalibrated)
* This example shows how to configure and use ADC peripheral. 
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
#include <stdlib.h>
#include <string.h>

//freertos includes
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "esp_log.h"

//adc includes
#include "esp_adc/adc_oneshot.h"
#include "esp_adc/adc_cali.h"
#include "esp_adc/adc_cali_scheme.h"

const static char *TAG = "ADC Test";

static int adc_raw;     //ADC Raw Data
static int voltage;     //ADC Voltage

void app_main(void)
{
    //-------------ADC1 Init---------------//
    adc_oneshot_unit_handle_t adc1_handle;                              //ADC1 Handle
    adc_oneshot_unit_init_cfg_t init_config1 = {                        //ADC1 Config
        .unit_id = ADC_UNIT_1,                                          //ADC1
    };
    ESP_ERROR_CHECK(adc_oneshot_new_unit(&init_config1, &adc1_handle)); //ADC1 Init

    //-------------ADC1 Config---------------//
    adc_oneshot_chan_cfg_t config = {                                                       //ADC1 Channel Config
        .bitwidth = ADC_BITWIDTH_DEFAULT,                                                   //ADC1 Bitwidth (Default)
        .atten = ADC_ATTEN_DB_11,                                                           //ADC1 Attenuation 
    };
    ESP_ERROR_CHECK(adc_oneshot_config_channel(adc1_handle, ADC_CHANNEL_0, &config));       //ADC1 Channel Config


    while (1)
    {
        ESP_ERROR_CHECK(adc_oneshot_read(adc1_handle, ADC_CHANNEL_0, &adc_raw));                        //ADC1 Read
        ESP_LOGI(TAG, "ADC%d Channel[%d] Raw Data: %d ", ADC_UNIT_1 + 1, ADC_CHANNEL_0, adc_raw);       //Print ADC1 Raw Data
        voltage = (adc_raw * 1100)/8192;                                                                //Calculate Voltage
        ESP_LOGI(TAG, "ADC%d Channel[%d] Voltage: %d mV", ADC_UNIT_1 + 1, ADC_CHANNEL_0, voltage);      //Print Voltage
        vTaskDelay(pdMS_TO_TICKS(1000));                                                                //Delay 1s
    }
}
