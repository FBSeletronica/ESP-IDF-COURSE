//Exemplo 1
/*
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

static const char *TAG = "GDB_Exemplo";

void funcao_com_erro() {
    ESP_LOGI(TAG, "Entrando na função com erro...");
    int *ponteiro_nulo = NULL;
    *ponteiro_nulo = 42;  // Causa um erro fatal
}

void tarefa_principal(void *pvParameters) {
    ESP_LOGI(TAG, "Iniciando tarefa principal...");
    vTaskDelay(pdMS_TO_TICKS(2000));
    funcao_com_erro();
}

void app_main() {
    ESP_LOGI(TAG, "Iniciando aplicação...");
    xTaskCreate(tarefa_principal, "Tarefa Principal", 2048, NULL, 5, NULL);
}

*/

//Exemplo 2

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/temperature_sensor.h"  // temperature sensor driver
#include "esp_log.h"

static const char *TAG = "Temp_Sensor_Example";

void app_main(void)
{
    ESP_LOGI(TAG, "Install temperature sensor, expected temp ranger range: 10~50 ℃");               // log message
    temperature_sensor_handle_t temp_sensor = NULL;                                                 // temperature sensor handler
    temperature_sensor_config_t temp_sensor_config = TEMPERATURE_SENSOR_CONFIG_DEFAULT(10, 50);     // temperature sensor configuration
    ESP_ERROR_CHECK(temperature_sensor_install(&temp_sensor_config, &temp_sensor));                 // install temperature sensor  
    ESP_LOGI(TAG, "Enable temperature sensor");                                                     // log message
    ESP_ERROR_CHECK(temperature_sensor_enable(temp_sensor));                                        // enable temperature sensor

    ESP_LOGI(TAG, "Read temperature");   // log message
    int cnt = 20;                       // counter to read temperature sensor 20 times
    float tsens_value;                  // temperature sensor value

    while (cnt--) {
        ESP_ERROR_CHECK(temperature_sensor_get_celsius(temp_sensor, &tsens_value));     // read temperature sensor
        ESP_LOGI(TAG, "Temperature value %.02f ℃", tsens_value);                       //log message
        vTaskDelay(pdMS_TO_TICKS(1000));                                                // delay 1 second
    }
}
