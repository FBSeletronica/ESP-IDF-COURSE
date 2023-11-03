/******************************************************************************
* Queue Example
* 
*  This example shows how to use queue to comunication between tasks
*  This code is part of the course "Programe o ESP32 com ESP-IDF 5" by Fábio Souza
*  The course is available on https://cursos.embarcados.com.br
* 
*  This example code Creative Commons Attribution 4.0 International License.
*  When using the code, you must keep the above copyright notice,
*  this list of conditions and the following disclaimer in the source code.
*  (http://creativecommons.org/licenses/by/4.0/)
*
* Author: Fábio Souza
* This code is for teaching purposes only.
* No warranty of any kind is provided.
*******************************************************************************/

#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "driver/gpio.h"
#include "esp_log.h"

#define LED 14

QueueHandle_t xQueue;   // Queue handle

/*Tasks Handles*/
TaskHandle_t xTask1Handle = NULL;
TaskHandle_t xTask2Handle = NULL;

/*Tasks prototipes*/
void vTask1(void *pvParameters);
void vTask2(void *pvParameters);

static const char *TAG = "main: ";

void app_main(void)
{
    bool status = false;                            //variable to store the LED status

    xQueue = xQueueCreate(5, sizeof(int)); // Create a queue with 5 elements of 4 bytes

    xTaskCreate(vTask1, "Task1", configMINIMAL_STACK_SIZE + 1024, NULL, 1, &xTask1Handle);  //create task
    xTaskCreate(vTask2, "Task2", configMINIMAL_STACK_SIZE + 1024, NULL, 1, &xTask2Handle);  //create task

    gpio_set_direction(LED,GPIO_MODE_OUTPUT);   //set the pin as output
    //loop 
    while(1)
    {
        gpio_set_level(LED,status^=1);                         //LED status
        vTaskDelay(pdMS_TO_TICKS(1000));                      //delay of 1 second
    }
}

void vTask1(void *pvParameters)
{
    int count = 0;                                  //counter variable
    while(1)
    {
        if(count <10){                                 //if counter < 10
            xQueueSend(xQueue, &count, portMAX_DELAY); //send data to queue
            count++;                                   //increment counter
        }
        else
        {
            count = 0;                        //reset counter
            vTaskDelay(pdMS_TO_TICKS(5000));  //delay of 5 seconds
        }
        vTaskDelay(pdMS_TO_TICKS(500));      //delay of 500ms
    }
}


void vTask2(void *pvParameters)
{
    int received_count = 0;
    while(1)
    {
        if(xQueueReceive(xQueue, &received_count, pdMS_TO_TICKS(1000)) == pdTRUE) //receive data from queue with 1 second timeout
        {
            ESP_LOGI(TAG,"Received: %d", received_count);         //log message
        }
        else
        {
            ESP_LOGI(TAG,"Timeout");                              //log message
        }        
    }
}
