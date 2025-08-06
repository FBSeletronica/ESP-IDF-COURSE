/******************************************************************************
*  Bluetooth Classic  - SPP (Serial Port Profile) Example
*  This example demonstrates how to use Bluetooth Classic SPP to control an LED by sending commands from a Bluetooth client(e.g., a smartphone app).
*  The LED can be turned on or off by sending "led on" or "led off" commands from the client.
*  The button state is also monitored, and its state changes are sent to the client.
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
// FreeRTOS e ESP-IDF
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "driver/gpio.h"
// Bluetooth
#include "esp_bt.h"
#include "esp_bt_main.h"
#include "esp_gap_bt_api.h"
#include "esp_spp_api.h"

//Mapping GPIOs for LED and Button
#define LED_GPIO    2
#define BUTTON_GPIO 0

//Tag for logging
static const char *TAG = "BT_SPP_LED";

// Name of the SPP server used in the Bluetooth connection
#define SPP_SERVER_NAME "ESP32_SPP_SERVER"  

// Handle for the connected client
static uint32_t client_handle = 0;  


/*
Task to send button state changes over Bluetooth
This task checks the button state every 200ms and sends updates if the state changes.
arg: Pointer to task parameters (not used in this case)
This task runs indefinitely, checking the button state and sending messages when the state changes.
It uses the gpio_get_level function to read the button state and esp_spp_write to send messages.
It also uses vTaskDelay to wait for 200 milliseconds between checks.
*/
void send_button_state_task(void *arg) {
    int last_state = 1; // Assume button is not pressed initially
    while (1) {
        int state = gpio_get_level(BUTTON_GPIO);                                    // Read the button state
        if (state != last_state && client_handle != 0) {                            // Only send if the state has changed and a client is connected
            char msg[32];                                                           // Prepare the message to send  
            sprintf(msg, "Button: %s\r\n", state == 0 ? "PRESSED" : "RELEASED");    // Format the message based on the button state
            esp_spp_write(client_handle, strlen(msg), (uint8_t *)msg);              // Send the message over SPP
            last_state = state;                                                     // Update the last state to the current state
        }
        vTaskDelay(pdMS_TO_TICKS(200));                                             // Wait for 200 milliseconds before checking again
    }
}

/*
Function to handle received commands from the client
This function processes commands received from the Bluetooth client.
It checks if the command is to turn the LED on or off and updates the LED state accordingly.
It uses gpio_set_level to control the LED GPIO pin and logs the action using ESP_LOGI.
cmd: Pointer to the command string received from the client
This function is called when data is received from the client, specifically in the ESP_SPP_DATA_IND_EVT case of the spp_callback function.
*/
void handle_rx_command(const char *cmd) {
    if (strncmp(cmd, "led on", 6) == 0) {           // Check if the command is to turn the LED on
        gpio_set_level(LED_GPIO, 1);                // Set the LED GPIO high
        ESP_LOGI(TAG, "LED ON");                    // Log the action
    } else if (strncmp(cmd, "led off", 7) == 0) {   // Check if the command is to turn the LED off
        gpio_set_level(LED_GPIO, 0);                // Set the LED GPIO low
        ESP_LOGI(TAG, "LED OFF");                   // Log the action
    }
}

/*
Callback function for SPP events
This function handles various SPP events such as initialization, client connection, data reception, and disconnection.
event: The SPP event that occurred
param: Pointer to the parameters associated with the event
This function is registered with the SPP API to handle events related to the SPP connection.
*/
void spp_callback(esp_spp_cb_event_t event, esp_spp_cb_param_t *param) {
    switch (event) {
        case ESP_SPP_INIT_EVT:                                                              // SPP initialization event
            esp_bt_gap_set_device_name("ESP32_LED_BT");                                     // Set the Bluetooth device name    
            esp_bt_gap_set_scan_mode(ESP_BT_CONNECTABLE, ESP_BT_GENERAL_DISCOVERABLE);      // Set the scan mode to connectable and discoverable
            esp_spp_start_srv(ESP_SPP_SEC_NONE, ESP_SPP_ROLE_SLAVE, 0, SPP_SERVER_NAME);    // Start the SPP server with no security, as a slave, on channel 0 with the specified server name
            break;

        case ESP_SPP_START_EVT:                                                             // SPP server started event
            ESP_LOGI(TAG, "Server SPP initialized");                                        // Log that the SPP server has been initialized 
            break;

        case ESP_SPP_SRV_OPEN_EVT:                                                          // SPP server open event    
            client_handle = param->srv_open.handle;                                         // Store the handle of the connected client
            ESP_LOGI(TAG, "Client connected");                                              // Log the client connection    
            break;

        case ESP_SPP_CLOSE_EVT:                                                             // SPP connection closed event
            ESP_LOGI(TAG, "Client disconnected");                                           // Log the disconnection of the client
            client_handle = 0;                                                              // Reset the client handle to indicate no active connection
            break;

        case ESP_SPP_DATA_IND_EVT: {                                                        // SPP data indication event    
            char received[128] = {0};                                                       // Buffer to store received data            
            int len = param->data_ind.len;                                                  // Get the length of the received data    
            if (len >= sizeof(received)) len = sizeof(received) - 1;                        // Ensure the buffer is not overflowed   
            memcpy(received, param->data_ind.data, len);                                    // Copy the received data into the buffer       
            received[len] = '\0';                                                           // Null-terminate the string to make it a valid C string
            ESP_LOGI(TAG, "Received: %s", received);                                        // Log the received data
            handle_rx_command(received);                                                    // Call the function to handle the received command
            break;
        }

        default:                                                                            // Handle other SPP events
            ESP_LOGI(TAG, "Unhandled SPP event: %d", event);                                // Log unhandled SPP events     
            break;
    }
}

void app_main(void)
{
    // Initialize NVS (Non-Volatile Storage) for storing configuration data
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);

    // Initialize GPIOs for LED and Button
    gpio_set_direction(LED_GPIO, GPIO_MODE_OUTPUT);
    gpio_set_direction(BUTTON_GPIO, GPIO_MODE_INPUT);
    gpio_set_pull_mode(BUTTON_GPIO, GPIO_PULLUP_ONLY);

     // Initialize Bluetooth controller and SPP
    esp_bt_controller_config_t bt_cfg = BT_CONTROLLER_INIT_CONFIG_DEFAULT();    // Default configuration for the Bluetooth controller
    ESP_ERROR_CHECK(esp_bt_controller_init(&bt_cfg));                           // Initialize the Bluetooth controller with the default configuration
    ESP_ERROR_CHECK(esp_bt_controller_enable(ESP_BT_MODE_CLASSIC_BT));          // Enable the Bluetooth controller in Classic Bluetooth mode
    ESP_ERROR_CHECK(esp_bluedroid_init());                                      // Initialize the Bluedroid stack   
    ESP_ERROR_CHECK(esp_bluedroid_enable());                                    // Enable the Bluedroid stack

    ESP_ERROR_CHECK(esp_spp_register_callback(spp_callback));                   // Register the SPP callback function to handle SPP events

    // Configure SPP with default settings
    esp_spp_cfg_t bt_spp_cfg = {
    .mode = ESP_SPP_MODE_CB,    // Use callback mode for SPP
    .enable_l2cap_ertm = false, // Disable L2CAP enhanced retransmission mode
    .tx_buffer_size = 0,        // Use default TX buffer size
    };
    ESP_ERROR_CHECK(esp_spp_enhanced_init(&bt_spp_cfg));    // Initialize SPP with the specified configuration

    // Create a FreeRTOS task to send button state changes over Bluetooth
    xTaskCreate(send_button_state_task, "btn_task", 2048, NULL, 10, NULL);
    
}
