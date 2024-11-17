/*
 * SPDX-FileCopyrightText: 2010-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: CC0-1.0
 */

#include <stdio.h>
#include <inttypes.h>
#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_chip_info.h"
#include "esp_flash.h"
#include "esp_system.h"
#include "esp_log.h"


static const char *TAG = "CoreDump";

void tarefa_com_pouca_pilha(void *pvParameters) {
    ESP_LOGI(TAG, "Iniciando tarefa...");
    char buffer[4096];  // Estouro de pilha
    buffer[0] = 'A';
    ESP_LOGI(TAG, "Tarefa executada.");
    vTaskDelete(NULL);
}

void tarefa_bloqueante(void *pvParameters) {
    ESP_LOGI(TAG, "Iniciando tarefa bloqueante...");
    while (1) {
        for (volatile int i = 0; i < 1000000000; i++) {}  // Loop infinito
    }
}

void app_main(void)
{
   


    //Ponteiro Nulo
    // int *ponteiro_nulo = NULL;
    // *ponteiro_nulo = 42;  // Causa um erro fatal

    //Stack overflow
    xTaskCreate(tarefa_com_pouca_pilha, "Tarefa Stack", 10, NULL, 5, NULL);

    // Tentativa de divisão por zero
    // ESP_LOGI(TAG, "Iniciando exemplo de divisão por zero...");
    // int a = 42;
    // int b = 0;    
    // int resultado = a / b;  // Causa um erro fatal
    // ESP_LOGI(TAG, "Resultado: %d", resultado);

    

    //double free
    // ESP_LOGI(TAG, "Iniciando exemplo de double free...");

    // int *ptr = malloc(100);
    // if (ptr == NULL) {
    //     ESP_LOGE(TAG, "Falha ao alocar memória!");
    //     return;
    // }

    // ESP_LOGI(TAG, "Memória alocada no endereço: %p", ptr);

    // free(ptr);
    // ESP_LOGI(TAG, "Memória liberada pela primeira vez.");

    // // Manipulação no ponteiro antes da segunda liberação
    // ptr[0] = 42;  // Acesso a memória já liberada
    // free(ptr);    // Segunda liberação de memória já corrompida
    // ESP_LOGI(TAG, "Tentativa de double free executada.");

    

   //Watchdog Timeout
//    ESP_LOGI(TAG, "Criando tarefa que causará Watchdog Timeout...");
//    xTaskCreate(tarefa_bloqueante, "Tarefa Watchdog", 2048, NULL, 5, NULL);
   
   
}
