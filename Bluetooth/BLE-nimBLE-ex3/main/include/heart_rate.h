/*
 * SPDX-FileCopyrightText: 2025 Fabio Souza
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */
#ifndef HEART_RATE_H
#define HEART_RATE_H

/* ---------------- ESP-IDF (for mock/timers) ---------------- */
#include "esp_random.h"
#include "freertos/FreeRTOS.h"

/* ---------------- NimBLE (BLE) ---------------- */
#include "nimble/ble.h"
#include "host/ble_hs.h"

/* Defines */
#ifndef HEART_RATE_TASK_PERIOD
#define HEART_RATE_TASK_PERIOD (1000 / portTICK_PERIOD_MS)
#endif

#ifdef __cplusplus
extern "C" {
#endif

/* -------- Data mock (as in your project) -------- */
uint8_t get_heart_rate(void);
void    update_heart_rate(void);

/* -------- BLE Integration (Heart Rate Measurement 0x2A37) ----
 * - characteristic value = [Flags (0x00)] [HR 8-bit]
 *   (we use Flags=0 for HR in 8 bits, simple for the lesson)
 */

/* Characteristic handle (filled when registering services). */
extern uint16_t heart_rate_chr_val_handle;

/* Access callback (for READ, if enabled) */
int heart_rate_chr_access(uint16_t conn_handle,
                          uint16_t attr_handle,
                          struct ble_gatt_access_ctxt *ctxt,
                          void *arg);

/* Called on subscribe/unsubscribe of INDICATE (via GAP event). */
void heart_rate_on_subscribe(uint16_t conn_handle,
                             uint16_t attr_handle,
                             uint8_t  cur_indicate);

/* Called on disconnect to clear internal state. */
void heart_rate_on_disconnect(uint16_t conn_handle);

/* Initializes internal service state (timer/optional). */
void heart_rate_init(void);

/* Sends an immediate indication (Flags=0, HR=get_heart_rate()).
 * Can be called by a task/timer every HEART_RATE_TASK_PERIOD. */
void send_heart_rate_indication(void);

#ifdef __cplusplus
}
#endif

#endif // HEART_RATE_H
