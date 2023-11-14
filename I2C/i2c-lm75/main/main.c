/******************************************************************************
*  I2C LM75 Read Temperature
* This example shows how to read temperature from LM75 using I2C
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
#include "driver/i2c.h"
#include "esp_log.h"

static const char *tag = "LM75A";

//define for i2c 
#define I2C_MASTER_SCL_IO           9           //gpio number for i2c clock - SCL      
#define I2C_MASTER_SDA_IO           8           //gpio number for i2c data - SDA         
#define I2C_MASTER_NUM              I2C_NUM_0   //i2c controller number                
#define I2C_MASTER_FREQ_HZ          400000      //i2c clock frequency               
#define I2C_MASTER_TX_BUF_DISABLE   0           //i2c master TX do not need buffer                 
#define I2C_MASTER_RX_BUF_DISABLE   0           //i2c master RX do not need buffer                

//define for LM75A
#define LM75A_ADDRESS  0X48                     //LM75A address
#define LM75_REG_TEMP  0x00                     //LM75A temperature register

void app_main()
{
        
   	ESP_LOGI(tag, "Starting I2C ");             //start logging

    //i2c configuration structure
	i2c_config_t i2c_config = {
        .mode = I2C_MODE_MASTER,                    //set as master mode
        .sda_io_num = I2C_MASTER_SDA_IO,            //set SDA pin
        .sda_pullup_en = GPIO_PULLUP_ENABLE,        //enable pullup on SDA
        .scl_io_num = I2C_MASTER_SCL_IO,            //set SCL pin
        .scl_pullup_en = GPIO_PULLUP_ENABLE,        //enable pullup on SCL
        .master.clk_speed = I2C_MASTER_FREQ_HZ,     //set clock frequency
    };
    i2c_param_config(I2C_MASTER_NUM, &i2c_config);        //set configuration to i2c peripheral                                      
	i2c_driver_install(I2C_MASTER_NUM,i2c_config.mode,I2C_MASTER_RX_BUF_DISABLE,I2C_MASTER_TX_BUF_DISABLE, 0); //install master driver
    ESP_LOGI(tag, "I2C Started");   //logging

    while(1)
    {   
        uint8_t raw_temp[2];                                                         //array for temperature raw data
        i2c_cmd_handle_t cmd = i2c_cmd_link_create();                                //create i2c command link
        i2c_master_start(cmd);                                                       //start i2c communication
        i2c_master_write_byte(cmd, (LM75A_ADDRESS << 1) | I2C_MASTER_READ, true);    //send address of LM75A with read bit
        i2c_master_read(cmd, (uint8_t *)&raw_temp, 2, I2C_MASTER_ACK);               //read 2 bytes from LM75A
        i2c_master_stop(cmd);                                                        //stop i2c communication
        i2c_master_cmd_begin(I2C_MASTER_NUM, cmd, 1000 / portTICK_PERIOD_MS);        //execute i2c command
        i2c_cmd_link_delete(cmd);                                                    //delete i2c command link
        

        //check if temperature is negative
        bool isNeg = false;
        if (raw_temp[0] & 0x80)                 //check if MSB is 1
        {
            isNeg = true;                       //set isNeg to true
            raw_temp[0] = raw_temp[0] & 0x7f;   //set MSB to 0
        }

        //calculate temperature
        int16_t data = (raw_temp[0] << 8 | raw_temp[1]) >> 5;   //combine 2 bytes and shift 5 bits to the right
        float temperature = (data * 0.125);                     //calculate temperature in celsius 

        if(isNeg) temperature *= (-1);                           //if temperature is negative, multiply by -1

        ESP_LOGI(tag, "Temperature: %.2f", temperature);    //logging
        vTaskDelay(1000 / portTICK_PERIOD_MS);              //delay 1 second
    }
}