/******************************************************************************
* LED Control - PWM
* This example shows how to configure and use LEDC peripheral with PWM.
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


#include<stdio.h>
//freeRTOS includes
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

//ESP32 includes
#include "driver/ledc.h"

//main task
void app_main(void)
{
    uint16_t duty = 0;    //duty cycle value

    //Prepare and then apply the LEDC PWM timer configuration
    ledc_timer_config_t timer_conf = {      //ledc timer struct
    .speed_mode = LEDC_LOW_SPEED_MODE,      //timer mode      
    .timer_num = LEDC_TIMER_0,              //timer number  
    .freq_hz = 5000,                        //frequency in Hz
    .duty_resolution = LEDC_TIMER_10_BIT,   //duty resolution
    .clk_cfg = LEDC_AUTO_CLK                //clock source
    };
    ledc_timer_config(&timer_conf);        //apply the configuration    

    //Prepare and then apply the LEDC PWM channel configuration
    ledc_channel_config_t channel_conf= {   //ledc channel struct
    .channel = LEDC_CHANNEL_0,              //channel number    
    .speed_mode = LEDC_LOW_SPEED_MODE,      //speed mode
    .timer_sel = LEDC_TIMER_0,              //select timer 
    .intr_type = LEDC_INTR_DISABLE,         //interrupt disabled
    .gpio_num = GPIO_NUM_1,                 //GPIO number
    .duty = 0                              //duty cycle
    };
    ledc_channel_config(&channel_conf);     //apply the configuration

    //init the duty cycle value
    ledc_set_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0, 511);    //set duty cycle value
    ledc_update_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0);      //update duty cycle value


    while(1) 
    {

        //fade routine
        if(duty<1023) //if duty cycle is less than 1023
        {
            ledc_set_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0, duty);   //set duty cycle value
            ledc_update_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0);      //update duty cycle value
            duty++;                                                     //increment duty cycle value
        }
        else                                            
        {
            duty=0;                                    //reset duty cycle value
        }
        vTaskDelay(10 / portTICK_PERIOD_MS);            //delay 10 ms
    }   
}

