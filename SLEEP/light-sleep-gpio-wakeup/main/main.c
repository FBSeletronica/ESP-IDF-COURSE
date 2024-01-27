/******************************************************************************
* Light Sleep with GPIO Wakeup
* This example shows how to configure a GPIO pin to wake up the chip from light sleep mode.
* This code is part of the course "Programe o ESP32 com ESP-IDF" by Fábio Souza
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
#include <stdlib.h>
#include "esp_timer.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_sleep.h"
#include "esp_log.h"
#include "driver/gpio.h"

#define GPIO_WAKEUP_PIN         39              //gpio pin to use for wakeup

static const char *TAG = "Light Sleep Test";    //console log tag

void app_main(void)
{
      
    /* Initialize GPIO */
    gpio_config_t config = {
            .pin_bit_mask = BIT64(GPIO_WAKEUP_PIN),     //gpio pin to use for wakeup
            .mode = GPIO_MODE_INPUT,                    //set gpio pin to input mode
            .pull_down_en = false,                      //disable pull down resistor
            .pull_up_en = false,                        //disable pull up resistor
            .intr_type = GPIO_INTR_DISABLE              //disable interrupt
    };
    gpio_config(&config);                               //configure gpio pin                       

    ESP_LOGI(TAG, "Enabling GPIO wakeup");                          //console log
    gpio_wakeup_enable(GPIO_WAKEUP_PIN, GPIO_INTR_LOW_LEVEL);       //config gpio wakeup pin and trigger type
    esp_sleep_enable_gpio_wakeup();                                 //enable gpio wakeup

    //wait for gpio wakeup pin to go high
    ESP_LOGI(TAG, "Waiting for GPIO%d to go high...", GPIO_WAKEUP_PIN);
    while (gpio_get_level(GPIO_WAKEUP_PIN) == 0)        //wait for gpio wakeup pin to go high
    {
        vTaskDelay(pdMS_TO_TICKS(10));                                  //delay 10ms
    }
    ESP_LOGI(TAG, "gpio wakeup source is ready");                       //console log

    vTaskDelay(pdMS_TO_TICKS(100));                                    //delay 100 ms (to allow user to see console log

 

    while(1)
    {
        ESP_LOGI(TAG, "Entering light sleep");                       //console log
        vTaskDelay(pdMS_TO_TICKS(100));                              //delay 100 ms (to allow user to see console log
        int64_t t_before_us = esp_timer_get_time();                 //get timestamp before entering sleep mode
        esp_light_sleep_start();                                    //enter light sleep mode. The chip will wake up when gpio pin goes high            
        int64_t t_after_us = esp_timer_get_time();                  //get timestamp after exiting sleep mode

        const char* wakeup_reason;                                  //get wakeup reason
        switch (esp_sleep_get_wakeup_cause()) 
        {
            case ESP_SLEEP_WAKEUP_GPIO:                               //gpio wakeup
                wakeup_reason = "gpio";                              //set wakeup reason
                break;
            default:
                wakeup_reason = "other";                            //set to other wakeup reason                
                break;
        }

        //log wakeup reason and sleep duration
        ESP_LOGI(TAG, "Returned from light sleep, reason: %s, t=%lld ms, slept for %lld ms\n",
                 wakeup_reason, t_after_us / 1000, (t_after_us - t_before_us) / 1000);        

        //wait for gpio wakeup pin to go high
        ESP_LOGI(TAG, "Waiting for GPIO%d to go high...", GPIO_WAKEUP_PIN);
        while (gpio_get_level(GPIO_WAKEUP_PIN) == 0)                          //wait for gpio wakeup pin to go high
        {
            vTaskDelay(pdMS_TO_TICKS(10));                                      //delay 10ms
        }
    }
}
