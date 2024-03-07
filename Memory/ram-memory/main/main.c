/******************************************************************************
* RAM memory organization
* This example shows how get the free memory in the RAM and IRAM
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
#include <string.h>
#include "esp_heap_caps.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"

static const char *TAG = "RAM Memory";  //define the TAG for the log

int x = 42;     //alocatted in .data
char y[50];     //alocatted in .bss
 
void app_main()
{   
  //just to compiler aloccate the memory  
  printf("x = %d y = %s\n", x,y);       

  //get the free heap size using FreeRTOS API  
  ESP_LOGI(TAG, "DRAM(xPortGetFreeHeapSize) = %d bytes",xPortGetFreeHeapSize());

  //get the free heap size using heap_caps API   
  int DRAM = heap_caps_get_free_size(MALLOC_CAP_8BIT);
  ESP_LOGI(TAG,"DRAM = %d bytes",DRAM);

  //get the free IRAM size using heap_caps API   
  int IRAM = heap_caps_get_free_size(MALLOC_CAP_32BIT) - heap_caps_get_free_size(MALLOC_CAP_8BIT);
  ESP_LOGI(TAG,"IRAM = %d bytes",IRAM);
}