/******************************************************************************
*  I2C Scan
* This example shows how to use the I2C peripheral to scan for I2C devices.
* This code is part of the course "Aprenda programar o ESP32 com ESP-IDF" by Fábio Souza
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
//FreeRTOS includes
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
//ESP32 includes
#include <driver/i2c.h>
#include <esp_log.h>


static char tag[] = "i2cscanner";  //tag for logging

//define for i2c 
#define I2C_MASTER_SCL_IO           9                           /*!< GPIO number used for I2C master clock */
#define I2C_MASTER_SDA_IO           8                           /*!< GPIO number used for I2C master data  */
#define I2C_MASTER_NUM              0                           /*!< I2C master i2c port number, the number of i2c peripheral interfaces available will depend on the chip */
#define I2C_MASTER_FREQ_HZ          10000                       /*!< I2C master clock frequency */
#define I2C_MASTER_TX_BUF_DISABLE   0                           /*!< I2C master doesn't need buffer */
#define I2C_MASTER_RX_BUF_DISABLE   0                           /*!< I2C master doesn't need buffer */
#define ACK_CHECK_EN                0x1                         /*!< I2C master will check ack from slave*/
#define ACK_CHECK_DIS               0x0                         /*!< I2C master will not check ack from slave */
#define ACK_VAL                     0x0                         /*!< I2C ack value */
#define NACK_VAL                    0x1                         /*!< I2C nack value */


//main function
void app_main()
{
   	ESP_LOGI(tag, "Starting I2C ");     //logging

    //i2c configuration structure
	i2c_config_t conf = {
	.mode = I2C_MODE_MASTER,                    //set as master mode
	.sda_io_num = I2C_MASTER_SDA_IO,            //set SDA pin
    .sda_pullup_en = GPIO_PULLUP_ENABLE,        //enable pullup on SDA
	.scl_io_num = I2C_MASTER_SCL_IO,            //set SCL pin
    .scl_pullup_en = GPIO_PULLUP_ENABLE,        //enable pullup on SCL
	.master.clk_speed = I2C_MASTER_FREQ_HZ,     //set clock frequency
    .clk_flags = I2C_SCLK_SRC_FLAG_FOR_NOMAL    //set clock source
    };

	i2c_param_config(I2C_MASTER_NUM, &conf);        //set configuration to i2c peripheral                                      
	i2c_driver_install(I2C_MASTER_NUM,conf.mode,I2C_MASTER_RX_BUF_DISABLE,I2C_MASTER_TX_BUF_DISABLE, 0); //install master driver

    ESP_LOGI(tag, "I2C Started ");   //logging

	//scan for i2c devices
    ESP_LOGI(tag, "Scanning I2C Addresses... ");        //log message

    //scan i2c addresses from 0 to 127
	for (int i=0; i< 128; i++) 
    {
		i2c_cmd_handle_t cmd = i2c_cmd_link_create();                           //create a command link
		i2c_master_start(cmd);                                                  //start the command link
		i2c_master_write_byte(cmd, (i << 1) | I2C_MASTER_WRITE, ACK_CHECK_EN);  //write the address to the slave
		i2c_master_stop(cmd);                                                   //stop the command link

		esp_err_t esp_error_code = i2c_master_cmd_begin(I2C_MASTER_NUM, cmd, pdMS_TO_TICKS(10));   //send the command link

        if (esp_error_code == ESP_OK)                            //if the command link is successful
            ESP_LOGI(tag, "Found=0x%x", i);                      //log the address
		i2c_cmd_link_delete(cmd);                                //delete the command link
	}

    ESP_LOGI(tag, "Scanner finished");                         //log message

   
}