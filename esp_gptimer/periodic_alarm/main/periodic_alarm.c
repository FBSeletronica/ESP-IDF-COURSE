/******************************************************************************
* periodic_alarm.c: Example of a periodic alarm callback
*
* This example shows how to configure a periodic gptimer with user
* defined callback function.
*
* This code is part of the course "Academia ESP32 Profissional" by Fábio Souza
* The course is available on https://cursos.embarcados.com.br
*
* This example code Creative Commons Attribution 4.0 International License.
* When using the code, you must keep the above copyright notice,
* this list of conditions and the following disclaimer in the source code.
* (http://creativecommons.org/licenses/by/4.0/)

* Author: Vinicius Silva <silva.viniciusr@gmail.com>
* This code is for teaching purposes only.
* No warranty of any kind is provided.
*******************************************************************************/

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gptimer.h"
#include "driver/gpio.h"
#include "esp_err.h"
#include "esp_log.h"

#define BLED_GPIO (12)
static const char *TAG = "main_app";
static gptimer_handle_t gptimer = NULL;

static esp_err_t s_init(void);
static esp_err_t s_release(void);
static bool IRAM_ATTR s_timer_cb(gptimer_handle_t timer,
					   const gptimer_alarm_event_data_t *edata,
					   void *user_data);


// Main application entry point
void app_main(void)
{
	//Initilize timer and LED gpios
	ESP_ERROR_CHECK(s_init());

	//Run for 30 seconds
	vTaskDelay(pdMS_TO_TICKS(30000));

	//Release timer and clear LED gpios
	ESP_ERROR_CHECK(s_release());
}

esp_err_t s_init(void)
{
	esp_err_t ret;
	
	//Configuring LED GPIOs
	const gpio_config_t gpio_handle = {
		.pin_bit_mask = (1LLU << BLED_GPIO),
		.mode = GPIO_MODE_OUTPUT,
		.pull_up_en = GPIO_PULLUP_DISABLE,
		.pull_down_en = GPIO_PULLDOWN_DISABLE,
		.intr_type = GPIO_INTR_DISABLE
	};
	ESP_LOGI(TAG, "Configuring LED GPIOs");
	if ((ret = gpio_config(&gpio_handle)))
		return ret;

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
		.alarm_count = 100000, //eq to 0.1s
		.flags.auto_reload_on_alarm = true //will reset the count on alarm
	};
	if ((ret = gptimer_set_alarm_action(gptimer, &alarm_config)))
		return ret;
	
	ESP_LOGI(TAG, "Starting timer with period of %uus", (unsigned) alarm_config.alarm_count);
	ret = gptimer_start(gptimer);
	
	return ret;
}

esp_err_t s_release(void)
{
	esp_err_t ret;

	//Stop GPtimer
    ESP_LOGI(TAG, "Stopping GPTimer");
    ret = gptimer_stop(gptimer);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "gptimer_stop failed (%s)", esp_err_to_name(ret));
        return ret;
    }

	//Disable GPTimer
    ESP_LOGI(TAG, "Disabling GPTimer");
    ret = gptimer_disable(gptimer);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "gptimer_disable failed (%s)", esp_err_to_name(ret));
        return ret;
    }

	//Delete GPTimer
    ESP_LOGI(TAG, "Deleting GPTimer");
    ret = gptimer_del_timer(gptimer);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "gptimer_del_timer failed (%s)", esp_err_to_name(ret));
        return ret;
    }

	// Resetting LED GPIO state
    ESP_LOGI(TAG, "Resetting LED GPIO state");
    ret = gpio_set_level(BLED_GPIO, 0);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "gpio_set_level failed (%s)", esp_err_to_name(ret));
        return ret;
    }

    // Reset GPIO configuration to default state
 	ret = gpio_reset_pin(BLED_GPIO);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "gpio_reset_pin failed (%s)", esp_err_to_name(ret));
        return ret;
    } 

    return ESP_OK;
}

bool s_timer_cb(gptimer_handle_t timer,
				const gptimer_alarm_event_data_t *edata,
				void *user_data)
{
	static DRAM_ATTR bool led_state = 0;

	//Toggle LED state
	led_state ^= 1;
	gpio_set_level(BLED_GPIO, led_state);
	return false;
}

