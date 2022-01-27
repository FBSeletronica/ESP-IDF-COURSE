/******************************************************************************
* ADC
* This example shows how to configure and use ADC peripheral.
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

//freeRTOS includes
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

//ESP32 includes
#include <driver/adc.h>

#define CALIBRATED_MODE     //comment this line to use uncalibrated mode
#define ADC_CHANNEL (ADC1_CHANNEL_8)    


#ifndef CALIBRATED_MODE  //uncalibrated mode

void app_main(void)
{
    adc1_config_width(ADC_WIDTH_BIT_13);                        //set the ADC1 resolution
    adc1_config_channel_atten(ADC_CHANNEL, ADC_ATTEN_DB_11);    //config ADC1 channel and attenuation
    while(1) {
        printf("ADC: %d\n", adc1_get_raw(ADC_CHANNEL));     //print the ADC1 value
        vTaskDelay(1000 / portTICK_PERIOD_MS);              //delay 1 second
    }

}

#else   //CALIBRATED_MODE

#include "esp_adc_cal.h"                //ESP32 ADC calibration
#define DEFAULT_VREF    1100            //Use adc2_vref_to_gpio() to obtain a better estimate
#define NO_OF_SAMPLES   64              //Multisampling

static esp_adc_cal_characteristics_t *adc_chars;        //ADC characteristics
static const adc_channel_t channel = ADC_CHANNEL_8;     //ADC channel   
static const adc_bits_width_t width = ADC_WIDTH_BIT_13; //ADC resolution
static const adc_atten_t atten = ADC_ATTEN_DB_11;       //ADC attenuation
static const adc_unit_t unit = ADC_UNIT_1;              //ADC unit

//Check if Two Point or Vref are burned into eFuse
static void check_efuse(void)
{
    if (esp_adc_cal_check_efuse(ESP_ADC_CAL_VAL_EFUSE_TP) == ESP_OK) {  //check the TP efuse for calibration data
        printf("eFuse Two Point: Supported\n");                         //efuse calibration data is available
    } else {                                                            //efuse calibration data is not available,
        printf("Cannot retrieve eFuse Two Point calibration values. Default calibration values will be used.\n");
    }
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
    if (unit == ADC_UNIT_1) {                           //check if ADC1 is to be used
        adc1_config_width(width);                       //set the ADC1 resolution
        adc1_config_channel_atten(channel, atten);      //config ADC1 channel and attenuation
    } else {                                            
        adc2_config_channel_atten((adc2_channel_t)channel, atten); //config ADC2 channel and attenuation
    }

    //Characterize ADC
    adc_chars = calloc(1, sizeof(esp_adc_cal_characteristics_t));   //allocate memory for the ADC characteristics
    esp_adc_cal_value_t val_type = esp_adc_cal_characterize(unit, atten, width, DEFAULT_VREF, adc_chars); //calibrate ADC
    print_char_val_type(val_type);                                 //print the calibration type

    //Continuously sample ADC1
    while (1) {
        uint32_t adc_reading = 0;                               //initialize variable for ADC reading
        //Multisampling
        for (int i = 0; i < NO_OF_SAMPLES; i++) {                        //read ADC multiple times
            if (unit == ADC_UNIT_1) {                                   //check if ADC1 is to be used
                adc_reading += adc1_get_raw((adc1_channel_t)channel);   //read ADC1
            } else {                                                    //ADC2 is to be used
                int raw;                                                //initialize variable for ADC reading
                adc2_get_raw((adc2_channel_t)channel, width, &raw);     //read ADC2
                adc_reading += raw;                                     //add ADC reading
            }
        }
        adc_reading /= NO_OF_SAMPLES;                       //average samples

        //Convert adc_reading to voltage in mV
        uint32_t voltage = esp_adc_cal_raw_to_voltage(adc_reading, adc_chars);  //convert ADC reading to voltage in mV
        printf("Raw: %d\tVoltage: %dmV\n", adc_reading, voltage);               //print ADC reading and voltage
        vTaskDelay(pdMS_TO_TICKS(1000));                                        //delay 1 second
    }
}
#endif  //CALIBRATED_MODE