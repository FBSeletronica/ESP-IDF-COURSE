/******************************************************************************
* Creating Task
* This example shows how to create a task in ESP32 using ESP-IDF
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
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "esp_log.h"

static const char *TAG = "TEST TASK: ";     //tag to identify the log messages

#define LED_PIN 14                          //led pin

/*Task Blink prototipe */
void vTaskBlink(void *pvParameters ); 

/*task handle*/
TaskHandle_t xTaskBlinkHandle = NULL;

/*app_main*/
void app_main(void)
{

    ESP_LOGI(TAG,"Iniciando o programa");     //log message

    xTaskCreate(
        vTaskBlink,                 //function with the code of the task
        "TaskBlink",                //Task name
        2048,                       //stack size
        NULL,                       //task parameters
        1,                          //task priority
        &xTaskBlinkHandle            //task handle
        );

    int i = 0;                                       //counter variable
    while(1){
        ESP_LOGI(TAG,"Task Main - Counter: %d",i);  //log message
        vTaskDelay(pdMS_TO_TICKS(1000));            //delay of 1 second
    }
}

/* Task Blink LED */
void vTaskBlink(void *pvParameters )
{
    bool status = false;                            //variable to store the LED status
    
    ESP_LOGI(TAG,"Iniciando a task Blink");         //log message
    
    /*LED PIN configuration*/
    //gpio_reset_pin(LED_PIN);                        //reset the pin
    gpio_set_direction(LED_PIN,GPIO_MODE_OUTPUT);   //set the pin as output

    //loop 
    while(1)
    {
        gpio_set_level(LED_PIN,status^=1);                              //LED status
        ESP_LOGI(TAG,"TASK Blink LED[%d]: %d", LED_PIN,status);         //log message
        vTaskDelay(pdMS_TO_TICKS(1000));                                //delay of 1 second
    }
}