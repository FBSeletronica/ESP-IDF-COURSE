/******************************************************************************
* UART events 
* This example shows how to use the UART driver to handle special UART events.
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
#include "freertos/queue.h"
#include "driver/uart.h"
#include <string.h>
#include "esp_log.h"

static const char *TAG = "uart_events"; // Tag for logging

#define PATTERN_CHR_NUM    (3)         

#define BUF_SIZE (1024)
#define RD_BUF_SIZE (BUF_SIZE)

QueueHandle_t uart0_queue;

void uart_event_task(void *pvParameters)
{
    uart_event_t event;                             // Variable to store UART events
    size_t buffered_size;                           // Variable to store UART buffer size
    uint8_t* dtmp = (uint8_t*) malloc(RD_BUF_SIZE); // Variable to store UART data
    while(1) 
    {
        //Waiting for UART event.
        if(xQueueReceive(uart0_queue, (void * )&event, (TickType_t)portMAX_DELAY))  // Wait for UART event
        {
            bzero(dtmp, RD_BUF_SIZE);                       
            ESP_LOGI(TAG, "uart[%d] event:", UART_NUM_0);       // Print UART event
            switch(event.type) 
            {
                //Event of UART receving data
                /*We'd better handler data event fast, there would be much more data events than
                other types of events. If we take too much time on data event, the queue might
                be full.*/
                case UART_DATA:                                                         // Data event
                    ESP_LOGI(TAG, "[UART DATA]: %d", event.size);                       // Print UART data
                    uart_read_bytes(UART_NUM_0, dtmp, event.size, portMAX_DELAY);       // Read UART data
                    ESP_LOGI(TAG, "[DATA EVT]: %s",dtmp);                                       // Print UART data
                    uart_write_bytes(UART_NUM_0, (const char*) dtmp, event.size);       // Write UART data
                    break;
                //Event of HW FIFO overflow detected
                case UART_FIFO_OVF:                                                      // FIFO overflow event
                    ESP_LOGI(TAG, "hw fifo overflow");                                   // Print FIFO overflow event               
                    uart_flush_input(UART_NUM_0);                                       // Flush UART buffer
                    xQueueReset(uart0_queue);                                           // Reset UART queue
                    break;
                //Event of UART ring buffer full
                case UART_BUFFER_FULL:                                                  // Buffer full event
                    ESP_LOGI(TAG, "ring buffer full");
                    uart_flush_input(UART_NUM_0);                                       // Flush UART buffer
                    xQueueReset(uart0_queue);                                           // Reset UART queue
                    break;
                //Event of UART RX break detected
                case UART_BREAK:
                    ESP_LOGI(TAG, "uart rx break");                                     // Print UART break event
                    break;
                //Event of UART parity check error
                case UART_PARITY_ERR:
                    ESP_LOGI(TAG, "uart parity error");                                 // Print UART parity error event
                    break;
                //Event of UART frame error
                case UART_FRAME_ERR:
                    ESP_LOGI(TAG, "uart frame error");                                  // Print UART frame error event
                    break;
                //UART_PATTERN_DET
                case UART_PATTERN_DET:
                    uart_get_buffered_data_len(UART_NUM_0, &buffered_size);             // Get UART buffer size
                    int pos = uart_pattern_pop_pos(UART_NUM_0);                         // Get UART pattern position
                    ESP_LOGI(TAG, "[UART PATTERN DETECTED] pos: %d, buffered size: %d", pos, buffered_size);    // Print UART pattern position
                    if (pos == -1)                                            // If UART pattern position is -1, there are no data available
                    {
                        uart_flush_input(UART_NUM_0);                        // Flush UART buffer
                    } 
                    else                                                    // If UART pattern position is not -1, there are data available
                    {
                        uart_read_bytes(UART_NUM_0, dtmp, pos, 100 / portTICK_PERIOD_MS);            // Read UART data
                        uint8_t pat[PATTERN_CHR_NUM + 1];                                            // Variable to store UART pattern
                        memset(pat, 0, sizeof(pat));                                                 // Clear UART pattern buffer
                        uart_read_bytes(UART_NUM_0, pat, PATTERN_CHR_NUM, 100 / portTICK_PERIOD_MS); // Read UART pattern
                        ESP_LOGI(TAG, "read data: %s", dtmp);                                        // Print UART data   
                        ESP_LOGI(TAG, "read pat : %s", pat);                                         // Print UART pattern
                    }
                    break;
                //Others
                default:
                    ESP_LOGI(TAG, "uart event type: %d", event.type);                               // Print UART event type
                    break;
            }
        }
    }
    free(dtmp);
    dtmp = NULL;
    vTaskDelete(NULL);
}

void app_main(void)
{

    //Configure UART parameters
    uart_config_t uart_config = {
        .baud_rate = 115200,                        // Baudrate
        .data_bits = UART_DATA_8_BITS,              // 8 bits
        .parity = UART_PARITY_DISABLE,              // Disable parity        
        .stop_bits = UART_STOP_BITS_1,              // 1 stop bit
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,      // Disable flow control
        .source_clk = UART_SCLK_DEFAULT,            // Default clock
    };

    //Install UART driver, and get the queue.
    uart_driver_install(UART_NUM_0, BUF_SIZE * 2, BUF_SIZE * 2, 5, &uart0_queue, 0);
    uart_param_config(UART_NUM_0, &uart_config);

    //Set uart pattern detect function.
    uart_enable_pattern_det_baud_intr(UART_NUM_0, '*', PATTERN_CHR_NUM, 9, 0, 0);   
    uart_pattern_queue_reset(UART_NUM_0, 20);

    //Create a task to handler UART event from ISR
    xTaskCreate(uart_event_task, "uart_event_task", 2048, NULL, 12, NULL);

    vTaskDelete(NULL);  // Delete this task
}
