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
#define GLED_GPIO (BLED_GPIO + 1)
#define RLED_GPIO (GLED_GPIO + 1)

static esp_err_t s_init(void);
static esp_err_t s_release(void);
static bool IRAM_ATTR s_timer_cb(gptimer_handle_t timer,
					   const gptimer_alarm_event_data_t *edata,
					   void *user_data);

static const char *s_TAG = "main_app";
static gptimer_handle_t gptimer = NULL;

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
	esp_err_t rc;
	
	//Configuring LED GPIOs
	const gpio_config_t gpio_handle = {
		.pin_bit_mask = (1LLU << RLED_GPIO) | (1LLU << GLED_GPIO) | (1LLU << BLED_GPIO),
		.mode = GPIO_MODE_OUTPUT,
		.pull_up_en = GPIO_PULLUP_DISABLE,
		.pull_down_en = GPIO_PULLDOWN_DISABLE,
		.intr_type = GPIO_INTR_DISABLE
	};
	ESP_LOGI(s_TAG, "Configuring LED GPIOs");
	if ((rc = gpio_config(&gpio_handle)))
		return rc;

	//Configuring gptimer
	const gptimer_config_t timer_config = {
		.clk_src = GPTIMER_CLK_SRC_DEFAULT,
		.direction = GPTIMER_COUNT_UP,
		.resolution_hz = 1000000 //1MHz, tick=1us
	};
	ESP_LOGI(s_TAG, "Getting a new timer");
	if ((rc = gptimer_new_timer(&timer_config, &gptimer)))
		return rc;

	ESP_LOGI(s_TAG, "Registering Callback");
	const gptimer_event_callbacks_t timer_callbacks = {
		.on_alarm = s_timer_cb
	};
	if ((rc = gptimer_register_event_callbacks(gptimer, &timer_callbacks, NULL)))
		return rc;

	ESP_LOGI(s_TAG, "Enabling timer");
	if ((rc = gptimer_enable(gptimer)))
		return rc; //Timer has not started yet!
	
	ESP_LOGI(s_TAG, "Configuring the alarm");
	gptimer_alarm_config_t alarm_config = {
		.reload_count = 0,
		.alarm_count = 100000, //eq to 0.1s
		.flags.auto_reload_on_alarm = true //will reset the count on alarm
	};
	if ((rc = gptimer_set_alarm_action(gptimer, &alarm_config)))
		return rc;
	
	ESP_LOGI(s_TAG, "Starting timer with period of %uus", (unsigned) alarm_config.alarm_count);
	rc = gptimer_start(gptimer);
	
	return rc;
}

esp_err_t s_release(void)
{
	esp_err_t rc = ESP_OK;

	//Clearing gptimer
	ESP_LOGI(s_TAG, "Clering gptimer");
	rc += gptimer_stop(gptimer);
	rc += gptimer_disable(gptimer);
	rc += gptimer_del_timer(gptimer);

	//Reseting LED GPIOs
	ESP_LOGI(s_TAG, "Reseting LED GPIOs");
	rc += gpio_set_level(BLED_GPIO, 0);
	rc += gpio_set_level(GLED_GPIO, 0);
	rc += gpio_set_level(RLED_GPIO, 0);

	if (rc)
		rc = ESP_FAIL;

	return rc;
}

bool s_timer_cb(gptimer_handle_t timer,
				const gptimer_alarm_event_data_t *edata,
				void *user_data)
{
	static DRAM_ATTR unsigned char counter = 0;

	gpio_set_level(BLED_GPIO + counter++, 0);
	counter = counter % 3;
	gpio_set_level(BLED_GPIO + counter, 1);
	return false;
}

