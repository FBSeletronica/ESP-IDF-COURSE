/*
 * SPDX-FileCopyrightText: 2023-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */
/* ULP Example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_err.h"
#include "esp_log.h"
#include "driver/gpio.h"
#include "driver/rtc_io.h"
#include "ulp.h"
#include "ulp_main.h"

#define BLINK_GPIO   (21)
#define BUTTON_GPIO   (4)

extern const uint8_t ulp_main_bin_start[] asm("_binary_ulp_main_bin_start");
extern const uint8_t ulp_main_bin_end[]   asm("_binary_ulp_main_bin_end");

static const char *TAG = "app_main";
static TaskHandle_t app_main_task = NULL;

static void init_ulp_program(void);
static void ulp_interrupt_handler(void *args);

void app_main(void)
{
    /* LED Setup */
    uint8_t led_state = 0;
    gpio_reset_pin(BLINK_GPIO);
    /* Set the GPIO as a push/pull output */
    gpio_set_direction(BLINK_GPIO, GPIO_MODE_OUTPUT);

    /* ULP Setup */
    app_main_task = xTaskGetCurrentTaskHandle();
    init_ulp_program();

    while (1) {
        /* Toggle the LED state */
        led_state = !led_state;
        ESP_LOGI(TAG, "Turning the LED[%.2d] %s!", BLINK_GPIO, led_state == true ? "ON" : "OFF");
        gpio_set_level(BLINK_GPIO, led_state);
        if (ulTaskNotifyTake(pdFALSE, pdMS_TO_TICKS(2000)))
            ESP_LOGI(TAG, "ULP has detected 5 button presses on GPIO%.2d.", BUTTON_GPIO);
    }
}

static void init_ulp_program(void)
{
    esp_err_t err = ulp_load_binary(0, ulp_main_bin_start,
            (ulp_main_bin_end - ulp_main_bin_start) / sizeof(uint32_t));
    ESP_ERROR_CHECK(err);

    /* GPIO used for pulse counting. */
    gpio_num_t gpio_num = BUTTON_GPIO;
    int rtcio_num = rtc_io_number_get(gpio_num);
    assert(rtc_gpio_is_valid_gpio(gpio_num) && "GPIO used for pulse counting must be an RTC IO");

     /* Initialize some variables used by ULP program.
     * Each 'ulp_xyz' variable corresponds to 'xyz' variable in the ULP program.
     * These variables are declared in an auto generated header file,
     * 'ulp_main.h', name of this file is defined in component.mk as ULP_APP_NAME.
     * These variables are located in RTC_SLOW_MEM and can be accessed both by the
     * ULP and the main CPUs.
     *
     * Note that the ULP reads only the lower 16 bits of these variables.
     */
    ulp_debounce_counter = 3;
    ulp_debounce_max_count = 3;
    ulp_next_edge = 0;
    ulp_io_number = rtcio_num; /* map from GPIO# to RTC_IO# */
    ulp_edge_count_to_wake_up = 10; /*after 5 buttons press interrupt will be triggered */

    /* Initialize selected GPIO as RTC IO, enable input, enable pullup and disable pulldown */
    rtc_gpio_init(gpio_num);
    rtc_gpio_set_direction(gpio_num, RTC_GPIO_MODE_INPUT_ONLY);
    rtc_gpio_pulldown_dis(gpio_num);
    rtc_gpio_pullup_en(gpio_num);
    rtc_gpio_hold_en(gpio_num);

#if CONFIG_IDF_TARGET_ESP32
    /* Disconnect GPIO12 and GPIO15 to remove current drain through
     * pullup/pulldown resistors on modules which have these (e.g. ESP32-WROVER)
     * GPIO12 may be pulled high to select flash voltage.
     */
    rtc_gpio_isolate(GPIO_NUM_12);
    rtc_gpio_isolate(GPIO_NUM_15);
#endif // CONFIG_IDF_TARGET_ESP32

    /* Set ULP wake up period to T = 20ms.
     * Minimum pulse width has to be T * (ulp_debounce_counter + 1) = 80ms.
     */
    ulp_set_wakeup_period(0, 20000);

    /* Register on WAKE ISR */
    ESP_ERROR_CHECK(ulp_isr_register(ulp_interrupt_handler, NULL));

    /* Start the program */
    err = ulp_run(&ulp_entry - RTC_SLOW_MEM);
    ESP_ERROR_CHECK(err);
}

static void ulp_interrupt_handler(void *args)
{
    BaseType_t task_woken = pdFALSE;
    vTaskNotifyGiveFromISR(app_main_task, &task_woken);
}
