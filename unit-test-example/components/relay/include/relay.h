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


#ifndef RELAY_H
#define RELAY_H

// Estrutura para armazenar as características do relé
typedef struct {
    int pin;        // Pino do relé
    int state;      // Estado do relé (ligado ou desligado)
} Relay;

// Função de inicialização do componente relé
void relay_init(Relay *relay, int pin);

// Função para ligar o relé
void relay_turn_on(Relay *relay);

// Função para desligar o relé
void relay_turn_off(Relay *relay);

// Função para obter o status do relé
int relay_get_status(Relay *relay);

#endif
