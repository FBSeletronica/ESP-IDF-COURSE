/*
 * SPDX-FileCopyrightText: 2022-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */

#include <stdlib.h>
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "hid_keyboard.h"

#define APP_BUTTON (GPIO_NUM_0) // Use BOOT signal by default
static const char *TAG = "example";

static void app_send_keys(void);

void app_main(void)
{
    // Initialize button that will trigger HID reports
    const gpio_config_t boot_button_config = {
        .pin_bit_mask = BIT64(APP_BUTTON),
        .mode = GPIO_MODE_INPUT,
        .intr_type = GPIO_INTR_DISABLE,
        .pull_up_en = true,
        .pull_down_en = false,
    };
    ESP_ERROR_CHECK(gpio_config(&boot_button_config));

    ESP_LOGI(TAG, "USB initialization");
    ESP_ERROR_CHECK(hid_keyboard_init());
    ESP_LOGI(TAG, "USB initialization DONE");

    while (1) {
        if (tud_mounted()) {
            static bool send_hid_data = false;
            if (send_hid_data) {
                app_send_keys();
            }
            send_hid_data = !gpio_get_level(APP_BUTTON);
        }
        vTaskDelay(pdMS_TO_TICKS(100));
    }
}

void app_send_keys(void)
{
    ESP_LOGI(TAG, "Sending keycode 'a'");
    hid_keyboard_send((uint8_t[6]) { HID_KEY_A });

    ESP_LOGI(TAG, "Sending keycode 'b'");
    hid_keyboard_send((uint8_t[6]) { HID_KEY_B });

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
}
