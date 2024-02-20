/******************************************************************************
* DHT11 - Temperature and Humidity Sensor
* This example shows how to read the temperature and humidity from the DHT11 sensor
* and display the data on the OLED display
* Components Repositories: 
    dht11: https://github.com/UncleRus/esp-idf-lib
    SSD1306: https://github.com/nopnop2002/esp-idf-ssd1306
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
//include the libraries
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"

//include the DHT11 library
#include "dht.h"
//include the OLED library
#include "ssd1306.h"

//OLED GPIO Mapping
#define OLED_SDA_GPIO 8         //define the SDA GPIO
#define OLED_SCL_GPIO 9         //define the SCL GPIO
#define OLED_RESET_GPIO -1      //define the RESET GPIO

//DHT11 configuration
static const dht_sensor_type_t sensor_type = DHT_TYPE_DHT11;
static const gpio_num_t dht_gpio = 15;

//Tag for the log
static const char *tag = "DHT11";

void app_main(void)
{
    int16_t temperature = 0;        //temperature variable
    int16_t humidity = 0;           //humidity variable
    char lineChar[20];              //lineChar variable

    SSD1306_t oled;                 //oled variable

    //initialize the OLED
    ESP_LOGI(tag, "Panel is 128x64");                   
    ESP_LOGI(tag, "INTERFACE is i2c");
    ESP_LOGI(tag, "CONFIG_SDA_GPIO=%d",OLED_SDA_GPIO);
    ESP_LOGI(tag, "CONFIG_SCL_GPIO=%d",OLED_SCL_GPIO);
    ESP_LOGI(tag, "CONFIG_RESET_GPIO=%d",OLED_RESET_GPIO);
    i2c_master_init(&oled,OLED_SDA_GPIO,OLED_SCL_GPIO,OLED_RESET_GPIO);     //initialize the i2c master driver

    //initialize the OLED
    ssd1306_init(&oled,128,64);                         //initialize the OLED
    ssd1306_clear_screen(&oled,false);                  //clear the OLED
    ssd1306_contrast(&oled,0xFF);                       //set the contrast
    ssd1306_display_text_x3(&oled,0,"DTH11",5,false);   //display the text
    vTaskDelay(pdMS_TO_TICKS(3000));                    //delay     

    ssd1306_clear_screen(&oled,false);                  //clear the OLED  
    ssd1306_contrast(&oled,0xFF);                       //set the contrast

    while(1)
    {
        if(dht_read_data(sensor_type,dht_gpio,&humidity,&temperature)==ESP_OK)      //read the data from the sensor
        {
            //LOG 
            ESP_LOGI(tag, "humidity: %d %%" ,humidity);                          //print the humidity   
            ESP_LOGI(tag, "Temperature: %d C", temperature);                     //print the temperature

            //display the data on the OLED
            sprintf(lineChar,"Temp: %d C",temperature/10);                       //format the temperature data   
            ssd1306_display_text(&oled,0,lineChar,sizeof(lineChar),false);      //display the temperature data
            sprintf(lineChar,"Humi: %d %%",humidity/10);                        //format the humidity data
            ssd1306_display_text(&oled,1,lineChar,sizeof(lineChar),false);      //display the humidity data
        }
        else
        {
            ESP_LOGE(tag, "Could not read data from sensor");                   //print the error message
        }

        vTaskDelay(pdMS_TO_TICKS(5000));                                        //delay             
    }
}
