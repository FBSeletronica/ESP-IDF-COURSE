/******************************************************************************
* Hello World
* This example shows how to write a simple program in ESP-IDF
* This code is part of the course "Aprenda programar o ESP32 com ESP-IDF" by Fábio Souza
* The course is available on https://cursos.embarcados.com.br
*
*  This example code Creative Commons Attribution 4.0 International License.
*  When using the code, you must keep the above copyright notice,
*  this list of conditions and the following disclaimer in the source code.
*  (http://creativecommons.org/licenses/by/4.0/)

*  Author: Fábio Souza
*  This code is for teaching purposes only.
*  No warranty of any kind is provided.
*******************************************************************************/

#include <stdio.h>

// freeRTOS Libraries
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"


//main function  
void app_main(void)
{ 
  while(1) //infinite loop
  {
    printf("Hello World!\n");              // print message on console 
    vTaskDelay(1000 / portTICK_PERIOD_MS); //delay for 1 second
  }
}
