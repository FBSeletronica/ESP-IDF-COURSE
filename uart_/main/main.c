/******************************************************************************
* UART
* This example shows how to use UART periferial to comunicat whit the PC.
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

//standard libraries
#include <stdio.h>
#include "string.h"
//esp libraries
#include "driver/uart.h"
#include "esp_log.h"

//pin mapping
#define TXD_PIN 17
#define RXD_PIN 18

#define RX_BUF_SIZE 1024            //rx buffer size

static const char *TAG = "MAIN";    //tag for logging

void app_main(void)
{
    ESP_LOGI(TAG, "Starting UART example");     //log message

    uart_config_t uart_config = {               //uart configuration
        .baud_rate = 9600,                      //baud rate
        .data_bits = UART_DATA_8_BITS,          //data bits
        .parity = UART_PARITY_DISABLE,          //parity
        .stop_bits = UART_STOP_BITS_1,          //stop bits
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE   //flow control
      };

    uart_param_config(UART_NUM_1, &uart_config);                                                //configure uart
    uart_set_pin(UART_NUM_1, TXD_PIN, RXD_PIN, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);         //set pins
    uart_driver_install(UART_NUM_1, RX_BUF_SIZE, 0, 0, NULL, 0);                                //install uart driver

    //test uart write
    char message[100] = "test UART esp32\r\n";                          //message to send
    uart_write_bytes(UART_NUM_1, message, sizeof(message));             //write message
    ESP_LOGI(TAG,"sending: %s", message);                               //log message

    for(int i = 0; i<5; i++)                                            //loop 5 times
    {
        int random = esp_random();                                      //get random number
        sprintf(message, "%d\r\n", random);                             //convert to string
        uart_write_bytes(UART_NUM_1, message, strlen(message));         //write message on uart
        ESP_LOGI(TAG,"sending: %s", message);                           //log message
    }

    //test uart read
    ESP_LOGI(TAG,"receiving");                                              //log message
    char incoming_message[RX_BUF_SIZE];                                     //buffer for incoming message

    //read message
    while (true)
    {
        memset(incoming_message, 0, sizeof(incoming_message));                                                  //clear buffer
        int len = uart_read_bytes(UART_NUM_1, (uint8_t *)incoming_message, RX_BUF_SIZE, pdMS_TO_TICKS(500));    //read message
        if(len> 0){                                                         //if message was received   
            incoming_message[len] = '\0';                                   //add null terminator
            ESP_LOGI(TAG,"Received: %s", incoming_message);                 //log message on monitor
    }    
  }


}