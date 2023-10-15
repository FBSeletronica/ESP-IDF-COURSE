/******************************************************************************
* Task Parameters
* This example shows how to pass parameters to a task when it is created.
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
static const char *TAG2 = "TASK BLINK";           //tag to identify the log messages
static const char *TAG3 = "TASK COUNTER";           //tag to identify the log messages

#define LED1_PIN 14                          //led pin
#define LED2_PIN 21                          //led pin

//led blink task parameters
#define BLINK1_PERIOD 1000                   //blink period in milliseconds
#define BLINK2_PERIOD 500                    //blink period in milliseconds

//led structure
typedef struct{
    int led_pin;                             //led pin
    int blink_period;                        //blink period 
}led_t;

led_t led1 = {LED1_PIN,BLINK1_PERIOD};       //led 1 parameters
led_t led2 = {LED2_PIN,BLINK2_PERIOD};       //led 2 parameters


/*Task Blink prototipe */
void vTaskBlink(void *pvParameters );
void vTaskCounter(void *pvParameters );

/*task handle*/
TaskHandle_t xTaskBlink1Handle = NULL;
TaskHandle_t xTaskBlink2Handle = NULL;
TaskHandle_t xTaskCounterHandle = NULL;

/*app_main*/
void app_main(void)
{

    ESP_LOGI(TAG1,"Iniciando o programa");     //log message

    xTaskCreate(
        vTaskBlink,                 //function with the code of the task
        "TaskBlink1",               //Task name
        2048,                       //stack size
        (void*)&led1,                      //task parameters
        1,                          //task priority
        &xTaskBlink1Handle          //task handle
        );  
    
    xTaskCreate(
        vTaskBlink,                 //function with the code of the task
        "TaskBlink2",               //Task name
        2048,                       //stack size
        (void*)&led2,                      //task parameters
        1,                          //task priority
        &xTaskBlink2Handle          //task handle
        );

    xTaskCreate(
        vTaskCounter,                //function with the code of the task
        "TaskCounter",               //Task name
        2048,                        //stack size
        (void*)1000,                 //task parameters
        2,                           //task priority
        &xTaskCounterHandle          //task handle
        );

    int i = 0;                                       //counter variable
    while(1){
        ESP_LOGI(TAG1,"Counter: %d",i++);  //log message
        vTaskDelay(pdMS_TO_TICKS(1000));   //delay of 1 second
    }
}

/* Task Blink LED */
void vTaskBlink(void *pvParameters )
{

    int led_pin = ((led_t*)pvParameters)->led_pin;              //get the led pin from the task parameters
    int blink_period = ((led_t*)pvParameters)->blink_period;    //get the blink period from the task parameters
    bool status = false;                                        //variable to store the LED status
    
    ESP_LOGI(TAG2,"Iniciando a task Blink");         //log message
    
    /*LED PIN configuration*/
    gpio_set_direction(led_pin,GPIO_MODE_OUTPUT);   //set the pin as output

    //loop 
    while(1)
    {
        gpio_set_level(led_pin,status^=1);                              //LED status
        ESP_LOGI(TAG2,"[%s] - LED[%d]: %d",pcTaskGetName(NULL),led_pin,status);         //log message
        vTaskDelay(pdMS_TO_TICKS(blink_period));                                //delay of 1 second
    }
}


/* Task Counter */
void vTaskCounter(void *pvParameters )
{
    int i = (int)pvParameters;                        //get the counter value from the task parameters

    while(1){
        ESP_LOGI(TAG3,"Counter: %d",i++);             //log message
        vTaskDelay(pdMS_TO_TICKS(1000));              //delay of 1 second
    }
}