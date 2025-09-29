/******************************************************************************
* USB HID Example
* This example shows how to configure and use USB HID with ESP-IDF.
* 
* This code is part of the course "Academia ESP32 Profissional" by Fábio Souza
* The course is available on https://cursos.embarcados.com.br
*
* This example code Creative Commons Attribution 4.0 International License.
* When using the code, you must keep the above copyright notice,
* this list of conditions and the following disclaimer in the source code.
* (http://creativecommons.org/licenses/by/4.0/)

* Authors: Fábio Souza e Vinicius Silva
* This code is for teaching purposes only.
* No warranty of any kind is provided.
*******************************************************************************/
#include <stdlib.h>
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "hid_keyboard.h"

static const char *TAG = "USB HID EXAMPLE";

#define BUTTON_UP (GPIO_NUM_7) // UP button pin
#define BUTTON_DOWN (GPIO_NUM_4) // DOWN button pin
#define BUTTON_LEFT (GPIO_NUM_6) // LEFT button pin
#define BUTTON_RIGHT (GPIO_NUM_5) // RIGHT button pin
#define BUTTON_A (GPIO_NUM_2) // A button pin
#define BUTTON_B (GPIO_NUM_3) // B button pin

#define BUTTON_COUNT 6  // Total number of buttons

// Array of button GPIOs for easy iteration 
static const gpio_num_t button_pins[BUTTON_COUNT] = {
    BUTTON_UP,
    BUTTON_DOWN,
    BUTTON_LEFT,
    BUTTON_RIGHT,
    BUTTON_A,
    BUTTON_B
};

// Function to send key presses based on button state
static void app_send_keys(uint8_t button_state)
{
    // Send key presses based on button state
     if (button_state & (1 << 0)) {
        ESP_LOGI(TAG, "UP pressed");
        hid_keyboard_send((uint8_t[6]) { HID_KEY_ARROW_UP } );
    }
    if (button_state & (1 << 1)) {
        ESP_LOGI(TAG, "DOWN pressed");
        hid_keyboard_send((uint8_t[6]) {HID_KEY_ARROW_DOWN });
    }
    if (button_state & (1 << 2)) {
        ESP_LOGI(TAG, "LEFT pressed");
        hid_keyboard_send((uint8_t[6]) {HID_KEY_ARROW_LEFT});
    }
    if (button_state & (1 << 3)) {
        ESP_LOGI(TAG, "RIGHT pressed");
        hid_keyboard_send((uint8_t[6]) {HID_KEY_ARROW_RIGHT});
    }
    if (button_state & (1 << 4)) {
        ESP_LOGI(TAG, "A pressed");
        hid_keyboard_send((uint8_t[6]) { HID_KEY_A });
    }
    if (button_state & (1 << 5)) {
        ESP_LOGI(TAG, "B pressed");
        hid_keyboard_send((uint8_t[6]) { HID_KEY_B });
    }

    
/*
    ESP_LOGI(TAG, "Sending keycode 'c'");
    hid_keyboard_send((uint8_t[6]) { HID_KEY_C });

    ESP_LOGI(TAG, "Sending keycode '[SPACE]'");
    hid_keyboard_send((uint8_t[6]) { HID_KEY_SPACE });

    ESP_LOGI(TAG, "Sending keycode 'shift_left + A'");
    hid_keyboard_send((uint8_t[6]) { HID_KEY_SHIFT_LEFT, HID_KEY_A });

    ESP_LOGI(TAG, "Sending keycode 'shift_left + B'");
    hid_keyboard_send((uint8_t[6]) { HID_KEY_SHIFT_LEFT, HID_KEY_B });

    ESP_LOGI(TAG, "Sending keycode 'shift_left + C'");
    hid_keyboard_send((uint8_t[6]) { HID_KEY_SHIFT_LEFT, HID_KEY_C });

    ESP_LOGI(TAG, "Sending keycode '[SPACE]'");
    hid_keyboard_send((uint8_t[6]) { HID_KEY_SPACE });

    */
}

// Function to read the state of all buttons and return a bitmask
static uint8_t read_buttons(void) {
    uint8_t state = 0;                                  // Bitmask to hold button states
    for (int i = 0; i < BUTTON_COUNT; i++) {            // Iterate over each button
        if (gpio_get_level(button_pins[i]) == 0) {      // Active low: button pressed
            state |= (1 << i);                          // Set the corresponding bit in the state
        }
    }
    return state;                                       // Return the combined button state
}

void app_main(void)
{
    // Initialize button that will trigger HID reports
    const gpio_config_t boot_button_config = {
        .pin_bit_mask = BIT64(BUTTON_UP)| BIT64(BUTTON_DOWN) | BIT64(BUTTON_LEFT) | BIT64(BUTTON_RIGHT) | BIT64(BUTTON_A) | BIT64(BUTTON_B),
        .mode = GPIO_MODE_INPUT,
        .intr_type = GPIO_INTR_DISABLE,
        .pull_up_en = true,
        .pull_down_en = false,
    };
    ESP_ERROR_CHECK(gpio_config(&boot_button_config));

    // Initialize USB HID
    ESP_LOGI(TAG, "USB initialization");
    ESP_ERROR_CHECK(hid_keyboard_init());       // Initialize USB HID
    ESP_LOGI(TAG, "USB initialization DONE");   


    // Main loop
    static uint8_t last_button_state = 0;
    while (1) {
        if (tud_mounted()) {                                                        // Check if USB is mounted
            uint8_t button_state = read_buttons();                                  // Read current button states      

            if (button_state != last_button_state) {                                 // If button state has changed
                last_button_state = button_state;                                    // Update last known state    

                if (button_state != 0) {                                             // If any button is pressed                                  
                    ESP_LOGI(TAG, "Button state changed: 0x%02X", button_state);     // Log the new button state
                    app_send_keys(button_state);                                     // Send corresponding key presses                        
                }
            }
        }
        vTaskDelay(pdMS_TO_TICKS(50));                                         // Delay to debounce and avoid rapid firing
    }
}


