/******************************************************************************
* Relay component
* This example shows how to create a new component and use it in a project.
* This code is part of the course "Programe o ESP32 com ESP-IDF 5" by Fábio Souza
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
// Inclui as bibliotecas do FreeRTOS
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
// Inclui a biblioteca do relé
#include "relay.h"
// Inclui a biblioteca string_lib
#include "string_lib.h"


// Define os pinos dos relés
#define RELAY1_PIN 21   //GPIO21
#define RELAY2_PIN 33   //GPIO33

void app_main(void)
{
    // Cria as variáveis dos relés
    Relay relay1, relay2;

    // Cria as mensagens para output
    char msg1[] = "status do rele 1:";
    char msg2[] = "status do rele 2:";

    // Inicializa os relés
    relay_init(&relay1, RELAY1_PIN);    // Inicializa o relé 1
    relay_init(&relay2, RELAY2_PIN);    // Inicializa o relé 2

    while(1) {
        string_Upper(msg1); string_Upper(msg2);                //converte msg para uppercase
        relay_turn_on(&relay1);                                         // Liga o relé 1
        printf("%s %d\n", msg1, relay_get_status(&relay1));    // Imprime o status do relé 1
        relay_turn_off(&relay2);                                        // Desliga o relé 2
        printf("%s %d\n", msg2, relay_get_status(&relay2));    // Imprime o status do relé 2
        vTaskDelay(5000 / portTICK_PERIOD_MS);                          // Aguarda 5 segundos
        string_Lower(msg1); string_Lower(msg2);                //converte msg para lowercase
        relay_turn_off(&relay1);                                        // Desliga o relé 1
        printf("%s %d\n", msg1, relay_get_status(&relay1));    // Imprime o status do relé 1
        relay_turn_on(&relay2);                                         // Liga o relé 2
        printf("%s %d\n", msg2, relay_get_status(&relay2));    // Imprime o status do relé 2
        vTaskDelay(5000 / portTICK_PERIOD_MS);                          // Aguarda 5 segundos
        string_Mirror(msg1); string_Mirror(msg2);              //espelha conteudo das msgs
    }
}
