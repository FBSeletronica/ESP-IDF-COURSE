/******************************************************************************
*  Mutex Semaphore Example
* 
*  This example shows how to use mutex semaphores to protect a shared resource.
*  This code is part of the course "Programe o ESP32 com ESP-IDF 5" by Fábio Souza
*  The course is available on https://cursos.embarcados.com.br
* 
*  This example code Creative Commons Attribution 4.0 International License.
*  When using the code, you must keep the above copyright notice,
*  this list of conditions and the following disclaimer in the source code.
*  (http://creativecommons.org/licenses/by/4.0/)
*
* Author: Fábio Souza
* This code is for teaching purposes only.
* No warranty of any kind is provided.
*******************************************************************************/
#include <stdio.h>              // Standard C functions, printf()
#include "freertos/FreeRTOS.h"  // FreeRTOS
#include "freertos/task.h"      // Task functions
#include "freertos/semphr.h"    // Semaphore functions

#include "driver/gpio.h"        // GPIO functions
#include "esp_log.h"            // ESP32 logging functions

static const char *TAG = "Semaphore Mutex: "; // Logging tag
SemaphoreHandle_t xMutexSemaphore = NULL;    // Semaphore handle


// Function to send data to serial interface
void sendSerialData(char *data)
{
    printf(data);                   // Print data to serial interface
}

// Task to be created.
void task1(void *pvParameter)
{
    while(1)
    {
        xSemaphoreTake(xMutexSemaphore, portMAX_DELAY);                             // Take the semaphore
        ESP_LOGI(TAG, "Task 1: PEGOU O SEMAFORO MUTEX");                            // Log the event
        sendSerialData("Task 1: [1] Enviando informações pela interface Serial\n"); // Send data to serial interface
        sendSerialData("Task 1: [2] Enviando informações pela interface Serial\n"); // Send data to serial interface
        sendSerialData("Esperando 2 segundos...\n");                                // Send data to serial interface
        for(int i = 0;i<20000;i++){
            esp_rom_delay_us(100);
        }
        sendSerialData("Task 1: [3] Enviando informações pela interface Serial\n"); // Send data to serial interface
        sendSerialData("Task 1: [4] Enviando informações pela interface Serial\n"); // Send data to serial interface
        ESP_LOGI(TAG, "Task 1: LIBEROU O SEMAFORO MUTEX"); 
        xSemaphoreGive(xMutexSemaphore); 
        vTaskDelay(1000 / portTICK_PERIOD_MS);                                       // Delay 100ms  
    }
}

// Task to be created.
void task2(void *pvParameter)
{
    while(1)
    {
        xSemaphoreTake(xMutexSemaphore, portMAX_DELAY);                             // Take the semaphore             
        ESP_LOGI(TAG, "Task 2: PEGOU O SEMAFORO MUTEX");                            // Log the event
        sendSerialData("Task 2: [1] Enviando informações pela interface Serial\n"); // Send data to serial interface
        sendSerialData("Task 2: [2] Enviando informações pela interface Serial\n"); // Send data to serial interface
        sendSerialData("Task 2: [3] Enviando informações pela interface Serial\n"); // Send data to serial interface
        sendSerialData("Task 2: [4] Enviando informações pela interface Serial\n"); // Send data to serial interface
        sendSerialData("Task 2: [5] Enviando informações pela interface Serial\n"); // Send data to serial interface
        ESP_LOGI(TAG, "Task 2: LIBEROU O SEMAFORO MUTEX");                          // Log the event
        xSemaphoreGive(xMutexSemaphore);                                            // Give the semaphore
        vTaskDelay(100 / portTICK_PERIOD_MS);                                       // Delay 100ms
    }
}


void app_main(void)
{
    xMutexSemaphore = xSemaphoreCreateMutex();          // Create a mutex semaphore

    // Create tasks
    xTaskCreate(&task1, "task1", 2048, NULL, 1, NULL);  // Task1 
    xTaskCreate(&task2, "task2", 2048, NULL, 4, NULL);  // Task2

    vTaskDelete(NULL);                                  // Delete this task

}
