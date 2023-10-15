/******************************************************************************
* Creating Tasks and deleting
* This example shows how to create task and delete it
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

static const char *TAG1 = "TASK APP MAIN";                //tag to identify the log messages
static const char *TAG2 = "TASK BLINK: ";           //tag to identify the log messages
static const char *TAG3 = "TASK COUNTER: ";           //tag to identify the log messages

#define LED_PIN 14                          //led pin

/*Task Blink prototipe */
void vTaskBlink(void *pvParameters );
void vTaskCounter(void *pvParameters );

/*task handle*/
TaskHandle_t xTaskBlinkHandle = NULL;
TaskHandle_t xTaskCounterHandle = NULL;

/*app_main*/
void app_main(void)
{

    ESP_LOGI(TAG1,"Iniciando o programa");     //log message

    xTaskCreate(
        vTaskBlink,                 //function with the code of the task
        "TaskBlink",                //Task name
        2048,                       //stack size
        NULL,                       //task parameters
        1,                          //task priority
        &xTaskBlinkHandle            //task handle
        );
    xTaskCreate(
        vTaskCounter,                 //function with the code of the task
        "TaskCounter",                //Task name
        2048,                       //stack size
        NULL,                       //task parameters
        2,                          //task priority
        &xTaskCounterHandle                        //task handle
        );

    int i = 0;                                       //counter variable
    while(1){
        ESP_LOGI(TAG1,"Counter: %d",i++);  //log message
        vTaskDelay(pdMS_TO_TICKS(1000));            //delay of 1 second
    }
}

/* Task Blink LED */
void vTaskBlink(void *pvParameters )
{
    bool status = false;                            //variable to store the LED status
    
    ESP_LOGI(TAG2,"Iniciando a task Blink");         //log message
    
    /*LED PIN configuration*/
    gpio_set_direction(LED_PIN,GPIO_MODE_OUTPUT);   //set the pin as output

    //loop 
    while(1)
    {
        gpio_set_level(LED_PIN,status^=1);                              //LED status
        ESP_LOGI(TAG2,"LED[%d]: %d", LED_PIN,status);         //log message
        vTaskDelay(pdMS_TO_TICKS(1000));                                //delay of 1 second
    }
}


/* Task Counter */
void vTaskCounter(void *pvParameters )
{
    int i = 1000;                                         //counter variable
    while(1){
        ESP_LOGI(TAG3,"Counter: %d",i++);         //log message
        
        if(i == 1010){
            vTaskDelete(xTaskBlinkHandle);              //delete the task
            gpio_set_level(LED_PIN,0);

        }
        else if(i == 1015){
            xTaskCreate(                    //create the task again
                vTaskBlink,                 //function with the code of the task
                "TaskBlink",                //Task name
                2048,                       //stack size
                NULL,                       //task parameters
                1,                          //task priority
                &xTaskBlinkHandle            //task handle
                );
            `
            vTaskDelete(xTaskCounterHandle);              //delete the task
        }

        vTaskDelay(pdMS_TO_TICKS(1000));              //delay of 1 second
    }
}