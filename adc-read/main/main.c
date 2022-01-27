/******************************************************************************
* ADC - Read 
* This example shows how to configure and use ADC peripheral without calibration.
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
//standard libraries
#include <stdio.h>
#include <stdlib.h>
//freertos libraries
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
//esp32 libraries
#include "driver/adc.h"

//adc channel
#define ADC_CHANNEL (ADC1_CHANNEL_6)   //gpio7

//main
void app_main(void)
{
    //configure adc
    adc1_config_width(ADC_WIDTH_BIT_13);                    //adc resolution
    adc1_config_channel_atten(ADC_CHANNEL, ADC_ATTEN_DB_0); //adc attenuation

    while(1)
    {
      int adc_value=0;                                    //store adc value
      int i;                                              //loop counter

      for(i = 0; i< 128; i++)                     
      {
        adc_value += adc1_get_raw(ADC_CHANNEL);           //read adc value
      }
      adc_value/=128;                                                 //average adc value
      int voltage = (adc_value * 1100)/8192;                          //convert adc value to voltage 
      printf("ADC value: %d   Voltage: %dmV\n", adc_value, voltage);  //print adc value and voltage
      vTaskDelay(1000 / portTICK_PERIOD_MS);                          //delay 1 second
    }
    
}
