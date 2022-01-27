/******************************************************************************
* DAC
* This example shows how to configure and use DAC peripheral.
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
// FreeRTOS includes
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

// ESP32 includes
#include "driver/dac.h"


int i = 0;  

void app_main()
{
    dac_output_enable(DAC_CHANNEL_1);       //enable DAC channel 1 pin 17 on ES32-S2
    dac_output_voltage(DAC_CHANNEL_1, 0);   //set DAC channel 1 to 0V

    while(1)
    {
        //increases voltage
        for(i = 0; i< 255; i++)                 
        {
            dac_output_voltage(DAC_CHANNEL_1, i);   //set DAC channel 1 to i 
            vTaskDelay(1);                          //delay 1 ms
        }
        //decreases voltage
        for(i = 255; i> 0; i--)
        {
            dac_output_voltage(DAC_CHANNEL_1, i);  //set DAC channel 1 to i
            vTaskDelay(1);                         //delay 1 ms
        }
    }
}