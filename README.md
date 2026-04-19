# ESP-IDF Course Examples

This repository contains the example projects used in the course "Academia ESP32 Profissional" by Fábio Souza.

The course is available at:
https://cursos.embarcados.com.br/cursos/academia-esp32-profissional/

## What You Will Find Here

The repository is organized as a collection of practical ESP-IDF examples for ESP32 development. The projects cover topics such as:

- GPIO, ADC, DAC, PWM, UART, I2C, I2S, USB, and timers
- Wi-Fi, MQTT, REST APIs, and ESP-NOW
- FreeRTOS tasks, queues, semaphores, notifications, and software timers
- Memory management, NVS, SPIFFS, LittleFS, and embedded files
- Sleep modes, watchdogs, logging, debugging, and security
- Sensors, external components, unit tests, and reusable project templates

Each project folder contains its own `README.md` with a short explanation of the example and its purpose.

## Repository Structure

Most examples are grouped by topic. Some of the main directories are:

- `ADC`, `DAC`, `GPIO`, `PWM`, `UART`, `I2C`, `I2S`, `USB`
- `WIFI`, `MQTT`, `RESTful_API`, `ESP-NOW`, `Bluetooth`
- `FreeRTOS`, `Memory`, `SLEEP`, `security`, `debugging`
- `Sensores`, `component-registry`, `extern-components`

There are also standalone examples and templates for common development workflows.

## How To Use

1. Install and configure ESP-IDF on your machine.
2. Open the example project you want to study.
3. Read the local `README.md` inside that project folder.
4. Build and flash the project with the usual ESP-IDF workflow, for example:

```bash
idf.py set-target esp32
idf.py build
idf.py -p PORT flash monitor
```

Replace `PORT` with the serial port used by your board.

## Who This Repository Is For

This repository is intended for students following the course and for developers who want practical ESP-IDF reference projects for ESP32-based applications.

## Hardware Reference

Most example projects in this repository were developed and tested primarily using the Franzininho WiFi and Franzininho WiFi LAB01 boards.

- Franzininho WiFi:
https://docs.franzininho.com.br/docs/franzininho-wifi/franzininho-wifi
- Franzininho WiFi LAB01:
https://docs.franzininho.com.br/docs/franzininho-wifi-lab01

## Course Link

If you want the complete training, lessons, and guided explanations, visit:

https://cursos.embarcados.com.br/cursos/academia-esp32-profissional/
