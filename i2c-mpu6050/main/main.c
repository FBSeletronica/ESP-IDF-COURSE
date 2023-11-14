/******************************************************************************
*  I2C - mpu6050
* This example shows how to use the I2C peripheral as master to control MPU6050 sensor.
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
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

//ESP32 includes
#include "driver/i2c.h"
#include "esp_log.h"
//mpu6050 includes
#include "mpu6050.h"

//define for i2c 
#define I2C_MASTER_SCL_IO           16                          /*!< GPIO number used for I2C master clock */
#define I2C_MASTER_SDA_IO           15                          /*!< GPIO number used for I2C master data  */
#define I2C_MASTER_NUM              I2C_NUM_0                   /*!< I2C master i2c port number, the number of i2c peripheral interfaces available will depend on the chip */
#define I2C_MASTER_FREQ_HZ          100000                      /*!< I2C master clock frequency */
#define I2C_MASTER_TX_BUF_DISABLE   0                           /*!< I2C master doesn't need buffer */
#define I2C_MASTER_RX_BUF_DISABLE   0                           /*!< I2C master doesn't need buffer */


static const char *TAG = "mpu6050 test";                        /*!< Tag for logging */
static mpu6050_handle_t mpu6050 = NULL;                         /*!< mpu6050 handle */


//entry point
void app_main(void)
{
     mpu6050_acce_value_t acce;                 //store the accelerometer data
     mpu6050_gyro_value_t gyro;                 //store the gyroscope data

     ESP_LOGI(TAG, "Starting I2C ");     //logging

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

    i2c_param_config(I2C_MASTER_NUM, &conf);                                                                        //configure the i2c peripheral
    i2c_driver_install(I2C_MASTER_NUM, conf.mode, I2C_MASTER_RX_BUF_DISABLE, I2C_MASTER_TX_BUF_DISABLE, 0);         //install the i2c peripheral
    ESP_LOGI(TAG, "I2C Started ");   //logging

    ESP_LOGI(TAG, "Initializing MPU6050 ");   //logging
    mpu6050 = mpu6050_create(I2C_MASTER_NUM, MPU6050_I2C_ADDRESS);                                               //create the mpu6050 handle
    mpu6050_config(mpu6050, ACCE_FS_4G, GYRO_FS_500DPS);                                                        //configure the mpu6050
    mpu6050_wake_up(mpu6050);                                                                                   //wake up the mpu6050
    ESP_LOGI(TAG, "MPU6050 Initialized ");   //logging

    while(1)    //infinite loop
    {
        mpu6050_get_acce(mpu6050, &acce);                                                                           //get the accelerometer data
        ESP_LOGI(TAG, "acce_x:%.2f, acce_y:%.2f, acce_z:%.2f\n", acce.acce_x, acce.acce_y, acce.acce_z);            //logging
        //printf("acce_x:%.2f, acce_y:%.2f, acce_z:%.2f\n", acce.acc    e_x, acce.acce_y, acce.acce_z);             //print the accelerometer data for Arduino plotter serial
        mpu6050_get_gyro(mpu6050, &gyro);                                                                           //get the gyroscope data
        ESP_LOGI(TAG, "gyro_x:%.2f, gyro_y:%.2f, gyro_z:%.2f\n", gyro.gyro_x, gyro.gyro_y, gyro.gyro_z);            //logging
        vTaskDelay(100 / portTICK_PERIOD_MS);                                                                         //delay for 100ms
    }

}
