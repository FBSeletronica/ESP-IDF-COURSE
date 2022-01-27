/******************************************************************************
* LED Control - PWM fade
* This example shows how to configure and use LEDC peripheral with PWM fade.
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
#include "driver/ledc.h"


void app_main(void) // Main
{
  ledc_timer_config_t timer_conf = {     //ledc timer struct             

      .speed_mode      = LEDC_LOW_SPEED_MODE,   //timer mode
      .timer_num       = LEDC_TIMER_0,          //timer number
      .freq_hz         = 1000,                  //frequency in Hz
      .duty_resolution = LEDC_TIMER_12_BIT,     //duty resolution
      .clk_cfg         = LEDC_AUTO_CLK          //clock source 
  };
  ledc_timer_config(&timer_conf);          //apply the configuration
  

  ledc_channel_config_t channel_conf ={         //ledc channel struct
        .speed_mode = LEDC_LOW_SPEED_MODE,      //speed mode
        .channel    = LEDC_CHANNEL_0,           //channel number
        .timer_sel  = LEDC_TIMER_0,             //select timer
        .intr_type  = LEDC_INTR_DISABLE,        //interrupt disabled
        .gpio_num   = GPIO_NUM_1,               //GPIO number
        .duty       = 0,                        //duty cycle
  };
  ledc_channel_config(&channel_conf);         //apply the configuration
 
  //iniatilize fade service
  ledc_fade_func_install(0);                 //install the fade function
  
  while(1)
  {     //fade routine    
        ledc_set_fade_time_and_start(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0,4096, 1000, LEDC_FADE_WAIT_DONE); //set fade time and start fade
        vTaskDelay(1000 / portTICK_PERIOD_MS);                                                             //delay 1 second
        ledc_set_fade_time_and_start(LEDC_LOW_SPEED_MODE,LEDC_CHANNEL_0,0,1000,LEDC_FADE_WAIT_DONE);       //set fade time and start fade
        vTaskDelay(1000 / portTICK_PERIOD_MS);                                                             //delay 1 second
  }

}

