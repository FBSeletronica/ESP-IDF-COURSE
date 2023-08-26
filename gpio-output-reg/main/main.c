/******************************************************************************
* GPIO Registers
* This example shows how to configure and use GPIO registers.
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
#include "driver/gpio.h"
#include "soc/gpio_reg.h"

//pin mapping
#define LED1 (1ULL<<14)    //GPIO1
#define LED2 (1ULL<<33)    //GPIO2

//main task
void app_main(void)
{
    REG_WRITE(GPIO_ENABLE_REG, LED1|LED2); //enable GPIO1 and GPIO2 

    while(true)
    {
        REG_WRITE(GPIO_OUT_W1TS_REG, LED1|LED2);   //set GPIO1 and GPIO2
        vTaskDelay(1000 / portTICK_PERIOD_MS);     //delay 1 secon
        REG_WRITE(GPIO_OUT_W1TC_REG, LED1|LED2);   //clear GPIO1 and GPIO2
        vTaskDelay(1000 / portTICK_PERIOD_MS);     //delay 1 secon
    }
}
