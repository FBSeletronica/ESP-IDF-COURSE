/******************************************************************************
* DHT11 - Temperature and Humidity Sensor
* This example shows how to dht11 sensor.
* Component Repository: https://github.com/UncleRus/esp-idf-lib
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
// include the header file
#include <stdio.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

//dht11 sensor library
#include <dht.h>

// define the sensor pin and the sensor type
static const dht_sensor_type_t sensor_type = DHT_TYPE_DHT11;
static const gpio_num_t dht_gpio = 15;


// entry point of the program
void app_main(void) 
{
  //auxiliary variables  
  int16_t temperatura = 0;
  int16_t umidade = 0;

  //infinite loop
  while(1) 
  {
    //read the sensor  
    if ( dht_read_data(sensor_type, dht_gpio, &umidade, &temperatura) == ESP_OK) //if the sensor is read successfully
    {
      printf("Umidade: %d%%\t Temperatura: %d°C\n", umidade/10, temperatura/10);  //print the sensor data
    } 
    else //if the sensor is not read successfully
    {
      printf("Não foi possível ler os dados do sensor\n"); //print the error message
    }
    vTaskDelay(5000 / portTICK_PERIOD_MS);  //delay 5 seconds
  }
}