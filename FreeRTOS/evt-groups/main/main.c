/******************************************************************************
*  Event Group Example
* 
*  This example shows how to use event groups to synchronize tasks.
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
#include <stdio.h>                  // Standard C functions, printf()
#include "freertos/FreeRTOS.h"      // FreeRTOS
#include "freertos/task.h"          // Task functions
#include "freertos/timers.h"        // FreeRTOS timers
#include "freertos/event_groups.h"  // FreeRTOS event groups

#include "driver/gpio.h"        // GPIO functions
#include "esp_log.h"            // ESP32 logging functions

//pin mapping
#define LED1_PIN 21          //GPIO21
#define LED2_PIN 14          //GPIO33
#define BUTTON1_PIN 2        //GPIO2

/* define event bits */
#define TASK_1_BIT        ( 1 << 0 ) //1
#define TASK_2_BIT        ( 1 << 1 ) //10

TaskHandle_t xTask1,xTask2,xTask3;    /*hadle das tasks*/
TimerHandle_t xTimer1,xTimer2; /*handle dos timers*/

/* create event group */
EventGroupHandle_t xEvents;

//tasks prototypes
void vTask1( void * pvParameters );
void vTask2( void * pvParameters );
void vTask3( void * pvParameters );

//callback timer prototypes
void timer1_callback(TimerHandle_t pxTimer );

static const char *TAG = "EVT_GROUPS_TEST: ";


void app_main(void)
{
    gpio_config_t io_conf = {}; // GPIO config structure

    // Configure LED
    io_conf.pin_bit_mask = (1ULL<<LED1_PIN)|(1ULL<<LED2_PIN) ; // Bitmask to select the GPIOs you want to configure
    io_conf.mode = GPIO_MODE_OUTPUT;      // Set the GPIO mode
    io_conf.pull_up_en = 0;               // Disable pull-up mode
    io_conf.pull_down_en = 0;             // Disable pull-down mode
    io_conf.intr_type = GPIO_INTR_DISABLE; // Disable interrupts
    gpio_config(&io_conf);                 // Configure GPIO with the given settings

    // Configure button
    io_conf.pin_bit_mask = (1ULL<<BUTTON1_PIN); // Bitmask to select the GPIOs you want to configure
    io_conf.mode = GPIO_MODE_INPUT;          // Set the GPIO mode
    io_conf.pull_up_en = 1;                  // Enable pull-up mode
    io_conf.pull_down_en = 0;                // Disable pull-down mode
    io_conf.intr_type = GPIO_INTR_DISABLE;   // Interrupt on negative edge
    gpio_config(&io_conf);                   // Configure GPIO with the given settings

    xEvents = xEventGroupCreate();          //create event group
    xTimer1 = xTimerCreate("Timer1", pdMS_TO_TICKS(1000), pdTRUE, (void*)0, timer1_callback);   //create timer1 - periodic mode

    xTaskCreate(vTask1,  "Task1",  configMINIMAL_STACK_SIZE + 1024,  NULL, 1, &xTask1);    
    xTaskCreate(vTask2,  "Task2",  configMINIMAL_STACK_SIZE + 1024,  NULL, 1, &xTask2);    
    xTaskCreate(vTask3,  "Task3",  configMINIMAL_STACK_SIZE + 1024,  NULL, 1, &xTask3);

    xTimerStart(xTimer1, 0); //start timer1

    vTaskDelete(NULL);
}


void vTask1( void * parameter )
{

    while(1)
    {
        xEventGroupWaitBits(xEvents, TASK_1_BIT, pdTRUE, pdTRUE, portMAX_DELAY); /* wait forever until event bit of task 1 is set */
        ESP_LOGI(TAG,"Task 1 saiu do estado de bloqueio");                                       /* log message */

    } 
}

/* this task is similar to sendTask1 */
void vTask2( void * parameter )
{
  int led = 0;
  while(1)
  {
    /* wait forever until event bit of task 2 is set */
    xEventGroupWaitBits(xEvents, TASK_2_BIT, pdTRUE, pdTRUE, portMAX_DELAY);
    ESP_LOGI(TAG,"Task 2 saiu do estado de bloqueio");
    //toggle LED2
    gpio_set_level(LED2_PIN,led^=1);
  }
}

void vTask3( void * pvParameters )
{
  while(1){
    xEventGroupWaitBits(xEvents, TASK_1_BIT|TASK_2_BIT, pdTRUE, pdTRUE, portMAX_DELAY);
    ESP_LOGI(TAG,"Task 3 saiu do estado de bloqueio");
  }
}


//callback do timer
void timer1_callback(TimerHandle_t pxTimer )
{
  static int count = 0;                   

  count++;                               
  gpio_set_level(LED1_PIN , count%2);        

  if(count == 5){
    xEventGroupSetBits(xEvents,TASK_1_BIT);
  }else if(count == 10){
    xEventGroupSetBits(xEvents,TASK_2_BIT);
  }
  else if(count == 15){
      count = 0;
      xEventGroupSetBits(xEvents,TASK_1_BIT|TASK_2_BIT);
  }
}