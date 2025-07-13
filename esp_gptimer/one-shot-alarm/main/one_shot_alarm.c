/******************************************************************************
* one_shot_alarm.c: Example of a one-shot alarm using GPTimer
*
* This example shows how to configure a GPTimer to generate a one-shot alarm.
*
* This code is part of the course "Academia ESP32 Profissional" by Fábio Souza
* The course is available on https://cursos.embarcados.com.br
*
* This example code Creative Commons Attribution 4.0 International License.
* When using the code, you must keep the above copyright notice,
* this list of conditions and the following disclaimer in the source code.
* (http://creativecommons.org/licenses/by/4.0/)

* Author: Fábio Souza e Vinicius Silva <silva.viniciusr@gmail.com>
* This code is for teaching purposes only.
* No warranty of any kind is provided.
*******************************************************************************/

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "driver/gptimer.h"
#include "driver/gpio.h"
#include "esp_err.h"
#include "esp_log.h"

#define BLED_GPIO    12
#define BUTTON_GPIO   2

static const char *TAG = "main_app";
static gptimer_handle_t gptimer = NULL;
static SemaphoreHandle_t timer_sem = NULL;
static bool timer_active = false;

// Forward declarations
static esp_err_t s_init(void);
static bool IRAM_ATTR s_timer_cb(gptimer_handle_t timer, const gptimer_alarm_event_data_t *edata, void *user_data);
static void led_control_task(void *arg);

// Main application entry point
void app_main(void)
{
    timer_sem = xSemaphoreCreateBinary();
    if (timer_sem == NULL) {
        ESP_LOGE(TAG, "Failed to create semaphore");
        return;
    }

    // Start task for LED control (waits for timer event)
    xTaskCreate(led_control_task, "led_control_task", 2048, NULL, 5, NULL);

    // Initialize GPIO and timer
    ESP_ERROR_CHECK(s_init());

    // Loop to poll button
    while (true) {
        // Detect press and wait for release
		if (!gpio_get_level(BUTTON_GPIO) && !timer_active) {
			vTaskDelay(pdMS_TO_TICKS(20));  // debounce
			if (!gpio_get_level(BUTTON_GPIO)) {
				ESP_LOGI(TAG, "Button pressed. Turning LED ON and starting one-shot timer.");

				gpio_set_level(BLED_GPIO, 1);
				timer_active = true;

				gptimer_stop(gptimer);
				gptimer_set_raw_count(gptimer, 0); 
				gptimer_start(gptimer);

				// wait for release
				while (!gpio_get_level(BUTTON_GPIO)) {
					vTaskDelay(pdMS_TO_TICKS(10));
				}
			}
		}
        vTaskDelay(pdMS_TO_TICKS(10));  // polling interval
    }
}

// Task to wait for timer event and turn off LED
static void led_control_task(void *arg)
{
    while (1) {
        if (xSemaphoreTake(timer_sem, portMAX_DELAY) == pdTRUE) {
            gpio_set_level(BLED_GPIO, 0);
            timer_active = false;
            ESP_LOGI(TAG, "Timer expired. Turning LED OFF.");
        }
    }
}



static esp_err_t s_init(void)
{
    esp_err_t ret;

    // LED GPIO
    gpio_config_t led_cfg = {
        .pin_bit_mask = 1ULL << BLED_GPIO,
        .mode = GPIO_MODE_OUTPUT,
        .pull_up_en = GPIO_PULLUP_DISABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_DISABLE
    };
    if ((ret = gpio_config(&led_cfg)) != ESP_OK) return ret;

    // Button GPIO
    gpio_config_t btn_cfg = {
        .pin_bit_mask = 1ULL << BUTTON_GPIO,
        .mode = GPIO_MODE_INPUT,
        .pull_up_en = GPIO_PULLUP_ENABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_DISABLE
    };
    if ((ret = gpio_config(&btn_cfg)) != ESP_OK) return ret;

    //Configuring gptimer
	const gptimer_config_t timer_config = {
		.clk_src = GPTIMER_CLK_SRC_DEFAULT,
		.direction = GPTIMER_COUNT_UP,
		.resolution_hz = 1000000, 	//1MHz, tick=1us
	};
	ESP_LOGI(TAG, "Getting a new timer");
	if ((ret = gptimer_new_timer(&timer_config, &gptimer)))
		return ret;

	ESP_LOGI(TAG, "Registering Callback");
	const gptimer_event_callbacks_t timer_callbacks = {
		.on_alarm = s_timer_cb
	};
	if ((ret = gptimer_register_event_callbacks(gptimer, &timer_callbacks, NULL)))
		return ret;

	ESP_LOGI(TAG, "Enabling timer");
	if ((ret = gptimer_enable(gptimer)))
		return ret; //Timer has not started yet!
	
	ESP_LOGI(TAG, "Configuring the alarm");
	gptimer_alarm_config_t alarm_config = {
		.reload_count = 0,
		.alarm_count = 2000000, //2 seconds
		.flags.auto_reload_on_alarm = false, //One-shot
	};
	if ((ret = gptimer_set_alarm_action(gptimer, &alarm_config)))
		return ret;
	
	return ret;
}

// GPTimer callback
bool IRAM_ATTR s_timer_cb(gptimer_handle_t timer,
                          const gptimer_alarm_event_data_t *edata,
                          void *user_data)
{
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;
    xSemaphoreGiveFromISR(timer_sem, &xHigherPriorityTaskWoken);
    return xHigherPriorityTaskWoken == pdTRUE;
}