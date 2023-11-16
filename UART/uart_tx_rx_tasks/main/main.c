/******************************************************************************
* UART TX AND RX Asyncronous Task
* This example demonstrates how two asynchronous tasks can use the same UART interface for communication.
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
#include "driver/uart.h"
#include "driver/gpio.h"
#include "string.h"
#include "esp_system.h"
#include "esp_log.h"

static const int RX_BUF_SIZE = 512;        // UART buffer size

int sendData(const char* logName, const char* data);
void tx_task(void *pvParameters);
void rx_task(void *pvParameters);

void app_main(void)
{
    // Configure parameters of an UART driver,
    const uart_config_t uart_config = {
        .baud_rate = 115200,                        // baudrate
        .data_bits = UART_DATA_8_BITS,              // data bit
        .parity = UART_PARITY_DISABLE,              // without parity
        .stop_bits = UART_STOP_BITS_1,              // stop bit
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,      // without flow control
        .source_clk = UART_SCLK_DEFAULT,            // use default clock source
    };
    uart_param_config(UART_NUM_0, &uart_config); 
    uart_driver_install(UART_NUM_0, RX_BUF_SIZE * 2, 0, 0, NULL, 0);    // install UART driver
                           // configure UART parameters

    // Create tasks
    xTaskCreate(rx_task, "uart_rx_task", configMINIMAL_STACK_SIZE + 1024*2, NULL, 2, NULL);
    xTaskCreate(tx_task, "uart_tx_task", configMINIMAL_STACK_SIZE + 1024, NULL, 1, NULL);
}

// Send data to UART
int sendData(const char* logName, const char* data)
{
    const int len = strlen(data);                                   // data length
    const int txBytes = uart_write_bytes(UART_NUM_0, data, len);    // write data to UART
    ESP_LOGI(logName, "Wrote %d bytes - Mesage: %s", txBytes,data);                   // log
    return txBytes;                                                 // return number of bytes sent
}

// TX task
void tx_task(void *pvParameters)
{
    int cnt = 0;
    char data[64];
    static const char *TX_TASK_TAG = "TX_TASK";                     // task name
    esp_log_level_set(TX_TASK_TAG, ESP_LOG_INFO);                   // set log level

    while (1) 
    {
        sprintf(data, "Counting: %d\r\n", cnt);                     // create data to send
        sendData(TX_TASK_TAG, data);                  // send data
        cnt++;
        vTaskDelay(pdMS_TO_TICKS(1000));                       // Delay 1s       
    }
}

// RX task
void rx_task(void *pvParameters)
{
    static const char *RX_TASK_TAG = "RX_TASK";            // task name
    esp_log_level_set(RX_TASK_TAG, ESP_LOG_INFO);          // set log level

    uint8_t* data = (uint8_t*) malloc(RX_BUF_SIZE+1);      // allocate memory for data

    while (1) 
    {
        const int rxBytes = uart_read_bytes(UART_NUM_0, data, RX_BUF_SIZE, pdMS_TO_TICKS(1000));    // read data from UART
        if (rxBytes > 0)                                                        // if data received
        {                                                                   
            data[rxBytes] = '\0';                                               // add null terminator      
            ESP_LOGI(RX_TASK_TAG, "Read %d bytes: '%s'", rxBytes, data);        // log data
        }
    }
}