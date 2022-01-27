/******************************************************************************
* ADC - Calibrated
* This example shows how to configure and use ADC calibrated
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
#include <stdio.h>
#include <stdlib.h>
//freeRTOS includes
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
//ESP32 includes
#include "driver/gpio.h"
#include "driver/adc.h"
#include "esp_adc_cal.h"

//defines
#define DEFAULT_VREF    1100        //Use adc2_vref_to_gpio() to obtain a better estimate
#define NO_OF_SAMPLES   128          //Multisampling

//global variables
static esp_adc_cal_characteristics_t *adc_chars;            //adc_chars 
#if CONFIG_IDF_TARGET_ESP32 //only for ESP32                //if ESP32
static const adc_channel_t channel = ADC_CHANNEL_6;         //GPIO34 if ADC1, GPIO14 if ADC2
static const adc_bits_width_t width = ADC_WIDTH_BIT_12;     //12bit resolution
#elif CONFIG_IDF_TARGET_ESP32S2                             //if ESP32S2
static const adc_channel_t channel = ADC_CHANNEL_6;         // GPIO7 if ADC1, GPIO17 if ADC2
static const adc_bits_width_t width = ADC_WIDTH_BIT_13;     //13bit resolution
#endif
static const adc_atten_t atten = ADC_ATTEN_DB_0;            //0dB attenuation
static const adc_unit_t unit = ADC_UNIT_1;                  //ADC1

//Check if Two Point or Vref are burned into eFuse
static void check_efuse(void)
{
#if CONFIG_IDF_TARGET_ESP32
    //Check if TP is burned into eFuse
    if (esp_adc_cal_check_efuse(ESP_ADC_CAL_VAL_EFUSE_TP) == ESP_OK) {
        printf("eFuse Two Point: Supported\n");
    } else {
        printf("eFuse Two Point: NOT supported\n");
    }
    //Check Vref is burned into eFuse
    if (esp_adc_cal_check_efuse(ESP_ADC_CAL_VAL_EFUSE_VREF) == ESP_OK) {
        printf("eFuse Vref: Supported\n");
    } else {
        printf("eFuse Vref: NOT supported\n");
    }
#elif CONFIG_IDF_TARGET_ESP32S2
    if (esp_adc_cal_check_efuse(ESP_ADC_CAL_VAL_EFUSE_TP) == ESP_OK) {
        printf("eFuse Two Point: Supported\n");
    } else {
        printf("Cannot retrieve eFuse Two Point calibration values. Default calibration values will be used.\n");
    }
#else
#error "This example is configured for ESP32/ESP32S2."
#endif
}

//print the calibration type
static void print_char_val_type(esp_adc_cal_value_t val_type)
{
    if (val_type == ESP_ADC_CAL_VAL_EFUSE_TP) {
        printf("Characterized using Two Point Value\n");
    } else if (val_type == ESP_ADC_CAL_VAL_EFUSE_VREF) {
        printf("Characterized using eFuse Vref\n");
    } else {
        printf("Characterized using Default Vref\n");
    }
}

//main
void app_main(void)
{
    //Check if Two Point or Vref are burned into eFuse
    check_efuse();

    //Configure ADC
    if (unit == ADC_UNIT_1) {
        adc1_config_width(width);                                   //set the resolution
        adc1_config_channel_atten(channel, atten);                  //set the attenuation value
    } else {        
        adc2_config_channel_atten((adc2_channel_t)channel, atten);
    }

    //Characterize ADC
    adc_chars = calloc(1, sizeof(esp_adc_cal_characteristics_t));
    esp_adc_cal_value_t val_type = esp_adc_cal_characterize(unit, atten, width, DEFAULT_VREF, adc_chars);
    print_char_val_type(val_type);

    //Continuously sample ADC1
    while (1) {
        uint32_t adc_reading = 0;                                       //initialize variable for ADC reading
        //Multisampling
        for (int i = 0; i < NO_OF_SAMPLES; i++)                        //read ADC multiple times
        {
            if (unit == ADC_UNIT_1)                                        //if ADC1
            {
                adc_reading += adc1_get_raw((adc1_channel_t)channel);   //read raw value
            } 
            else                                                     //if ADC2
            {
                int raw;                                                //create variable for raw value
                adc2_get_raw((adc2_channel_t)channel, width, &raw);     //read raw value
                adc_reading += raw;                                     //add raw value to variable
            }
        }
        adc_reading /= NO_OF_SAMPLES;                                            //average samples                                          
        //Convert adc_reading to voltage in mV                          
        uint32_t voltage = esp_adc_cal_raw_to_voltage(adc_reading, adc_chars);   //print ADC reading and voltage
        printf("Raw: %d\tVoltage: %dmV\n", adc_reading, voltage);               //print ADC reading and voltage
        vTaskDelay(pdMS_TO_TICKS(1000));                                        //delay 1 sec
    }
}
