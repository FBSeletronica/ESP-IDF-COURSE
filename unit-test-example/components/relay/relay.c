/*
Esse é um componente simples para demonstrar o uso de componentes no ESP32.

Esse componente é usado para controlar relés de uma forma mais abstraída.

Autor: Fábio Souza
Data: 28/06/2023

* This example code Creative Commons Attribution 4.0 International License.
* When using the code, you must keep the above copyright notice,
* this list of conditions and the following disclaimer in the source code.
* (http://creativecommons.org/licenses/by/4.0/)

* This code is for teaching purposes only.
* No warranty of any kind is provided.
*/

#include <stdio.h>
#include "relay.h"
#include "driver/gpio.h"

// Função de inicialização do componente relé
void relay_init(Relay *relay, int pin) {
    relay->pin = pin;
    relay->state = 0;
    gpio_set_direction(relay->pin, GPIO_MODE_OUTPUT);
    gpio_set_level(relay->pin, relay->state);
}

// Função para ligar o relé
void relay_turn_on(Relay *relay) {
    relay->state = 1;
    gpio_set_level(relay->pin, relay->state);
}

// Função para desligar o relé
void relay_turn_off(Relay *relay) {
    relay->state = 0;
    gpio_set_level(relay->pin, relay->state);
}

//
int relay_get_status(Relay *relay) {
    return relay->state;
}