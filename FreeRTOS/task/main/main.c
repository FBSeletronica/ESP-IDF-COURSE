#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "esp_log.h"

static const char *TAG = "TEST TASK: ";

#define LED_PIN 2   //pino do LED

/* Protótipo da Task*/
void vTaskBlink(void *pvParameters ); 

/*app_main*/
void app_main(void)
{

    ESP_LOGI(TAG,"Iniciando o programa");

    xTaskCreate(
        &vTaskBlink,    //Função que implementa a task
        "TaskBlink",    //Nome da task
        2048,           //Tamanho da stack
        NULL,           //Parâmetros da task
        1,              //Prioridade da task
        NULL            //Referência para a task
        );
}

/* Task Blink LED */
void vTaskBlink(void *pvParameters )
{
    ESP_LOGI(TAG,"Iniciando a task Blink");
    gpio_reset_pin(LED_PIN);                        //reseta o pino
    gpio_set_direction(LED_PIN,GPIO_MODE_OUTPUT);   //configura o pino como saída

    //loop infinito
    while(1)
    {
        gpio_set_level(LED_PIN,1);                  //liga o LED
        ESP_LOGI(TAG,"LED ligado");                 //envia mensagem para o monitor serial
        vTaskDelay(pdMS_TO_TICKS(1000));            //delay de 1 segundo
        gpio_set_level(LED_PIN,0);                  //desliga o LED
        ESP_LOGI(TAG,"LED desligado");              //envia mensagem para o monitor serial
        vTaskDelay(pdMS_TO_TICKS(1000));            //delay de 1 segundo
    }

    vTaskDelete(NULL);                              //deleta a task
}