/******************************************************************************
*  I2S example - old version of I2S driver
* This example shows how to read the INMP441 I2S microphone and send the data to the serial port.
* The INMP441 is a high performance, low power, digital output, omnidirectional MEMS microphone with bottom port.
* 
* This code is part of the course "Academia ESP32 Profissional" by Fábio Souza
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
#include "esp_log.h"
#include "driver/i2s.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"


#define I2S_PORT I2S_NUM_0   // I2S port number 
#define SAMPLE_RATE 16000    // Sample rate de 16kHz
#define BITS_PER_SAMPLE 16   // bits for each sample 
#define BUFFER_SIZE 1024     // Buffer size for I2S reading

// I2S pins for INMP441 microphone
#define I2S_SCK_PIN   34
#define I2S_WS_PIN    35
#define I2S_SD_PIN    33

static const char *TAG = "I2S_READ";    // Tag for logging


/*
* Function to initialize the I2S peripheral
*/
void i2s_init() {
    i2s_config_t i2s_config = {
        .mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_RX),    //Operation mode 
        .sample_rate = SAMPLE_RATE,                             //Sample rate
        .bits_per_sample = I2S_BITS_PER_SAMPLE_16BIT,           //Bits per sample
        .channel_format = I2S_CHANNEL_FMT_ONLY_LEFT,            //Mono
        .communication_format = I2S_COMM_FORMAT_I2S,            //I2S format
        .intr_alloc_flags = ESP_INTR_FLAG_LEVEL1,               //Interrupt level 1
        .dma_buf_count = 8,                                     //Number of buffers
        .dma_buf_len = BUFFER_SIZE,                             //Buffer size        
    };

    i2s_pin_config_t pin_config = {
        .bck_io_num = I2S_SCK_PIN,
        .ws_io_num = I2S_WS_PIN,
        .data_out_num = I2S_PIN_NO_CHANGE,
        .data_in_num = I2S_SD_PIN,
        .mck_io_num = I2S_PIN_NO_CHANGE
    };

    ESP_ERROR_CHECK(i2s_driver_install(I2S_PORT, &i2s_config, 0, NULL));    // Install and start I2S driver
    ESP_ERROR_CHECK(i2s_set_pin(I2S_PORT, &pin_config));                    // Set the I2S pin configuration
}

/*
* Task to read the I2S data
*/
void audio_capture_task(void *pvParameters) 
{
    int16_t buffer[BUFFER_SIZE];    // buffer to read data into 
    size_t bytes_read;              // number of bytes read

    while (1) 
    {
        esp_err_t err = i2s_read(I2S_PORT, buffer, BUFFER_SIZE, &bytes_read, portMAX_DELAY); // Read the data

        int samplesRead = bytes_read / sizeof(int16_t);         // Number of samples read

        if (err == ESP_OK)                                      // If read was successful
        {
            for (int i = 0; i < samplesRead; i+=8)               // Print the samples
            {
                printf("%d\n", buffer[i]);                      
            }
        } else {
            ESP_LOGE(TAG, "ERROR TO READ I2S DATA: %d", err);   // Log error
        } 
    }
}

void app_main() {
    vTaskDelay(10000 / portTICK_PERIOD_MS);                                     // Delay to open the serial port
    ESP_LOGI(TAG, "Initializing I2S...");                                       // Log message
    i2s_init();                                                                 // Initialize I2S
    xTaskCreate(audio_capture_task, "audio_capture_task", 4096, NULL, 5, NULL); // Create the task to read the I2S data
}
