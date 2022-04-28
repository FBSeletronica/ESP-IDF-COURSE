/******************************************************************************
* WIFI EXAMPLE - Connect Manual
* This example shows how to connect to a WiFi network make a http request
* This code is part of the course "Aprenda programar o ESP32 com ESP-IDF" by Fábio Souza
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

//freertos includes
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "freertos/semphr.h"

//esp-idf includes
#include "esp_system.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_log.h"
#include "nvs_flash.h"

//http client includes
#include "esp_http_client.h"

#include "http_client.h"
#include "wifi.h"

//tag for logging
static const char *TAG = "WIFI Example";

xSemaphoreHandle wifi_semaphore;

void http_client_task(void *pvParameters)
{

  xSemaphoreTake(wifi_semaphore, portMAX_DELAY);

  while(1)
  {
    ESP_LOGI(TAG, "http_client_task");
    http_client_request();                              //make http request
    vTaskDelay(pdMS_TO_TICKS(10000));
  }
}

//main func
void app_main(void)
{
    vTaskDelay(1000);                   //delay for wait USB CDC initialization
    //Initialize NVS
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
      ESP_ERROR_CHECK(nvs_flash_erase());
      ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);

    ESP_LOGI(TAG, "ESP_WIFI_MODE_STA");
    wifi_semaphore = xSemaphoreCreateBinary();
    wifi_init_sta();    
    xTaskCreate(&http_client_task, "http_client_task", 4096, NULL, 5, NULL);

}
