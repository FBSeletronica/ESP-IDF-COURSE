/******************************************************************************
* dynamic_alarm.c: Example of a dynamic alarm callback
*
* This example shows how to configure a dynamic gptimer with user
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

#include <stdint.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gptimer.h"
#include "driver/gpio.h"
#include "esp_err.h"
#include "esp_log.h"
#include "portmacro.h"

#define BLED_GPIO (12)
#define GLED_GPIO (BLED_GPIO + 1)
#define RLED_GPIO (GLED_GPIO + 1)

#define ALARM_MAX  (100000)
#define ALARM_MIN  (1000)
#define ALARM_STEP (1000)

static esp_err_t s_init(void* user_data);
static esp_err_t s_release(void);
static bool IRAM_ATTR s_timer_cb(gptimer_handle_t timer,
					   const gptimer_alarm_event_data_t *edata,
					   void *user_data);

static const char *s_TAG = "main_app";
static gptimer_handle_t gptimer = NULL;

typedef struct {
	uint64_t delta_t;
	TaskHandle_t user_task;
} user_data_t;

void app_main(void)
{
	user_data_t my_data = {
		.delta_t = ALARM_MAX,
		.user_task = xTaskGetCurrentTaskHandle()
	};

	bool decrementing = true;

	//Initilize timer and LED gpios
	ESP_ERROR_CHECK(s_init(&my_data));

	//Change alarm period on notification
	while (1) {
		if (ulTaskNotifyTake(pdFALSE, portMAX_DELAY)) {
			if (decrementing) {
				my_data.delta_t -= ALARM_STEP;
				if (my_data.delta_t <= ALARM_MIN)
					decrementing = false;
			}
			else {
				my_data.delta_t += ALARM_STEP;
				if (my_data.delta_t >= ALARM_MAX)
					decrementing = true;
			}
		}
	}

	//Release timer and clear LED gpios
	ESP_ERROR_CHECK(s_release());
}

esp_err_t s_init(void* user_data)
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

	//Configuring periodic gptimer
	const gptimer_config_t timer_config = {
		.clk_src = GPTIMER_CLK_SRC_DEFAULT,
		.direction = GPTIMER_COUNT_UP,
		.resolution_hz = 1000000 //1MHz, tick=1us
	};
	ESP_LOGI(s_TAG, "Getting a new timer");
	if ((rc = gptimer_new_timer(&timer_config, &gptimer)))
		return rc;

	const gptimer_event_callbacks_t timer_callbacks = {
		.on_alarm = s_timer_cb
	};
	ESP_LOGI(s_TAG, "Registering Callback");
	if ((rc = gptimer_register_event_callbacks(gptimer, &timer_callbacks, user_data)))
		return rc;

	ESP_LOGI(s_TAG, "Enabling timer");
	if ((rc = gptimer_enable(gptimer)))
		return rc; //Timer has not started yet!
	
	gptimer_alarm_config_t alarm_config = {
		.reload_count = 0,
		.alarm_count = ((user_data_t*) user_data)->delta_t,
		.flags.auto_reload_on_alarm = false
	};
	ESP_LOGI(s_TAG, "Starting timer");
	if ((rc = gptimer_set_alarm_action(gptimer, &alarm_config)))
		return rc;
	
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
	BaseType_t task_woken = pdFALSE;
	static DRAM_ATTR unsigned char counter = 0;

	gpio_set_level(BLED_GPIO + counter++, 0);
	counter = counter % 3;
	gpio_set_level(BLED_GPIO + counter, 1);

	gptimer_alarm_config_t alarm_config = {
		.alarm_count = edata->alarm_value + ((user_data_t*) user_data)->delta_t
	};
	gptimer_set_alarm_action(timer, &alarm_config);
	
	vTaskNotifyGiveFromISR(((user_data_t*) user_data)->user_task, &task_woken);
	return task_woken;
}

