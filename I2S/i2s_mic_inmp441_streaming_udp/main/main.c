/******************************************************************************
*  I2S streaming example
* This example shows how to read the INMP441 I2S microphone and send the audio stream by UDP Socket.
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
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/i2s_std.h"
#include "esp_wifi.h"
#include "nvs_flash.h"
#include "esp_netif.h"
#include "esp_event.h"
#include "lwip/sockets.h"
#include "esp_log.h"


#define WIFI_SSID "XXXXXXXXXXXXXXXXXX"      // ⬅️ Change this to your WiFi SSID
#define WIFI_PASS "XXXXXXXXXXXXXXXXXX"      // ⬅️ Change this to your WiFi password

//UDP configuration
#define UDP_IP "192.168.0.73"       // ⬅️ IP address of the UDP server	(⚠️ Adjust this IP address to your server)
#define UDP_PORT 12345              // Port of the UDP server

//audio configuration
#define SAMPLE_RATE    (16000)  // 16KHz sample rate 
#define BITS_PER_SAMPLE (16)    // 16 bits per sample
#define BUFFER_SIZE (1024)      // Buffer size in bytes 

//I2S configuration
#define I2S_PORT I2S_NUM_0      // I2S port number
#define I2S_SCK_PIN 34          // I2S BCLK pin (Bit Clock)
#define I2S_WS_PIN 35           // I2S WS pin (Word Select or LR Clock)
#define I2S_SD_PIN 33           // I2S SD pin (Serial Data)


static const char *TAG = "I2S UDP Stream Example";    // Tag for logging
i2s_chan_handle_t rx_channel;                       // I2S channel handle
int udp_socket;                                     // UDP socket
struct sockaddr_in udp_server;                      // UDP server address    

void wifi_init() {
    
    // Initialize NVS
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);

    // Initialize the TCP/IP network interface
    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());
    esp_netif_create_default_wifi_sta();

    // Initialize Wi-Fi
    wifi_init_config_t wifi_init_cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&wifi_init_cfg));

    // Set the Wi-Fi operating mode
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));

    // Configure the Wi-Fi connection and connect to the network
    wifi_config_t wifi_config = {
        .sta = {
            .ssid = WIFI_SSID,
            .password = WIFI_PASS,
        },
    };

    // Set the Wi-Fi configuration
    ESP_ERROR_CHECK(esp_wifi_set_config(ESP_IF_WIFI_STA, &wifi_config));
    ESP_ERROR_CHECK(esp_wifi_start());
    ESP_ERROR_CHECK(esp_wifi_connect());
}

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

void udp_socket_init() {
    udp_socket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);  // Create a UDP socket
    if (udp_socket < 0)                                     // Check if the socket was created
    {
        ESP_LOGE(TAG, "Error to create UDP socket!");           // Print an error message
        return;
    }

    // Configure the UDP server address
    memset(&udp_server, 0, sizeof(udp_server));                 // Clear the UDP server structure
    udp_server.sin_family = AF_INET;                            // Set the address family
    udp_server.sin_port = htons(UDP_PORT);                      // Set the port
    inet_pton(AF_INET, UDP_IP, &udp_server.sin_addr.s_addr);    // Set the IP address

    ESP_LOGI(TAG, "UDP socket created. Sending to %s:%d", UDP_IP, UDP_PORT);    // Print a message with the IP and port
}

void i2s_read_task(void *pvParameters) {
    int16_t buffer[BUFFER_SIZE / 2];    // Buffer to store the data read from the I2S
    size_t bytes_read;                  // Number of bytes read from the I2S

    while (1) {       
        if (i2s_channel_read(rx_channel, buffer, BUFFER_SIZE, &bytes_read, portMAX_DELAY) == ESP_OK) {
            ESP_LOGI(TAG, "Bytes read: %d", bytes_read);  // Print the number of bytes read from the I2S
            sendto(udp_socket, buffer, bytes_read, 0, (struct sockaddr *)&udp_server, sizeof(udp_server));
        } else {
            ESP_LOGE(TAG, "Error reading I2S data");  // Print an error message
        }
    }
}
void app_main() {
    vTaskDelay(10000 / portTICK_PERIOD_MS);                             // Delay to open the serial monitor
    wifi_init();                                                        // Initialize Wi-Fi
    i2s_init();                                                         // Initialize I2S
    udp_socket_init();                                                  // Initialize UDP
    xTaskCreate(i2s_read_task, "i2s_read_task", 4096, NULL, 5, NULL);   // Create the audio stream task
}