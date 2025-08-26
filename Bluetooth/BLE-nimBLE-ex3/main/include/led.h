/*
 * SPDX-FileCopyrightText: 2025 Fabio Souza
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */
#ifndef LED_H
#define LED_H

/* ---------------- ESP-IDF (hardware) ---------------- */
#include "driver/gpio.h"
#include "led_strip.h"
#include "sdkconfig.h"

/* GPIO defined in menuconfig */
#ifndef BLINK_GPIO
#define BLINK_GPIO CONFIG_BLINK_GPIO
#endif

/* ---------------- NimBLE (BLE) ---------------- */
#include "nimble/ble.h"
#include "host/ble_hs.h"

#ifdef __cplusplus
extern "C" {
#endif

/* ---------- Hardware API (as in your project) ---------- */
uint8_t get_led_state(void);
void    led_on(void);
void    led_off(void);
void    led_init(void);

/* ---------- BLE Integration (LED characteristic) ------------ */
/* Characteristic handle (filled when registering services). */
extern uint16_t led_chr_val_handle;

/* Access callback for the LED characteristic.
 * - WRITE: 1 byte (0x00 = OFF, !=0 = ON)
 * - READ:  optional (returns 0/1); only effective if .flags include READ
 */
int led_chr_access(uint16_t conn_handle,
                   uint16_t attr_handle,
                   struct ble_gatt_access_ctxt *ctxt,
                   void *arg);

#ifdef __cplusplus
}
#endif

#endif // LED_H
