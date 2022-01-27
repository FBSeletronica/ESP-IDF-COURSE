/******************************************************************************
* DAC - Cosine Wave
* This example shows how to configure and use DAC peripheral with a cosine wave feature.
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

//main
void app_main(void)
{
    dac_cw_config_t dac_cw_config = {       //configure DAC with a cosine wave
        .en_ch = DAC_CHANNEL_1,             //enable DAC channel 1 pin 17 on ES32-S2
        .scale = DAC_CW_SCALE_2,            //scale DAC to 1/2 of the DAC range
        .phase = DAC_CW_PHASE_0,            //Phase shift +0° 
        .freq = 1100,                       // Range: 130(130Hz) ~ 55000(100KHz).
        .offset = 48                        // Range: 0 ~ 255.
    };
    
    dac_cw_generator_config(&dac_cw_config);    // config the cosine wave generator function in DAC module. 
    dac_output_enable(DAC_CHANNEL_1);           //enable DAC channel 1 pin 17 on ES32-S2
    dac_cw_generator_enable();                  //Enable cosine wave generator output.
    
}