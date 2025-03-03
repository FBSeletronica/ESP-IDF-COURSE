/******************************************************************************
*  I2S example
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
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/i2s_std.h"
#include "esp_log.h"

//audio configuration
#define SAMPLE_RATE    (16000)  // 16KHz sample rate 
#define BITS_PER_SAMPLE (16)    // 16 bits per sample
#define BUFFER_SIZE (1024)      // Buffer size in bytes 

//I2S configuration
#define I2S_PORT I2S_NUM_0      // I2S port number
#define I2S_SCK_PIN 34          // I2S BCLK pin (Bit Clock)
#define I2S_WS_PIN 35           // I2S WS pin (Word Select or LR Clock)
#define I2S_SD_PIN 33           // I2S SD pin (Serial Data)

static const char *TAG = "I2S Read Example";    // Tag for logging
i2s_chan_handle_t rx_channel;                   // I2S channel handle

void i2s_init()
{
    ESP_LOGI(TAG, "Initializing I2S...");
    i2s_chan_config_t rx_channel_config = I2S_CHANNEL_DEFAULT_CONFIG(I2S_PORT, I2S_ROLE_MASTER);    // I2S channel configuration
    ESP_ERROR_CHECK(i2s_new_channel(&rx_channel_config,NULL, &rx_channel));                         // Create a new I2S channel 

    i2s_std_config_t rx_std_config = {
        .clk_cfg = I2S_STD_CLK_DEFAULT_CONFIG(SAMPLE_RATE),
        .slot_cfg = I2S_STD_PHILIP_SLOT_DEFAULT_CONFIG(BITS_PER_SAMPLE,I2S_SLOT_MODE_MONO),
        .gpio_cfg = {
            .mclk = I2S_GPIO_UNUSED,    // MCLK is not used in this example
            .bclk = I2S_SCK_PIN,        // BCLK pin
            .ws = I2S_WS_PIN,           // WS pin
            .dout = I2S_GPIO_UNUSED,    // DOUT is not used in this example
            .din = I2S_SD_PIN           // SD pin
        },
    };

    ESP_ERROR_CHECK(i2s_channel_init_std_mode(rx_channel, &rx_std_config));    // Initialize I2S channel in standard mode
    ESP_ERROR_CHECK(i2s_channel_enable(rx_channel));                           // Enable I2S channel
    ESP_LOGI(TAG, "I2S initialized");
}

void i2s_read_task(void *pvParameters)
{
    int16_t buffer[BUFFER_SIZE / 2];    // Buffer to store the data read from the I2S
    size_t bytes_read;                  // Number of bytes read from the I2S

    while(1)
    {
        ESP_ERROR_CHECK(i2s_channel_read(rx_channel, buffer, BUFFER_SIZE, &bytes_read, portMAX_DELAY));    // Read data from the I2S

        ESP_LOGI(TAG, "Bytes read: %d", bytes_read);  // Print the number of bytes read from the I2S
        
        //Print the data read from the I2S
        for(size_t i = 0; i < bytes_read/2; i+=8)    // The data read from the I2S is 16 bits, so we need to divide by 2
        {
            printf("%d\n", buffer[i]);              // Print the data read from the I2S
        }

    }
}

void app_main(void)
{
    vTaskDelay(10000 / portTICK_PERIOD_MS);                                 // Delay to open the serial port
    i2s_init();                                                             // Initialize I2S
    xTaskCreate(i2s_read_task, "i2s_read_task", 4096, NULL, 5, NULL);       // Create I2S read task
}
