/*
 * SPDX-FileCopyrightText: 2025 Fabio Souza
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */

#pragma once

#include <stdint.h>
#include "nimble/ble.h"
#include "host/ble_hs.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Battery Level characteristic handle (0x2A19), filled by NimBLE
 * when services are registered (used to send notifications). */
extern uint16_t battery_chr_val_handle;

/* Initializes the module (creates simulation timer, resets states). */
void battery_init(void);

/* Access callback para a característica Battery Level (READ). */
int battery_chr_access(uint16_t conn_handle,
                       uint16_t attr_handle,
                       struct ble_gatt_access_ctxt *ctxt,
                       void *arg);

/* Called by the GAP subscribe event when there is (un)subscription to NOTIFY. */
void battery_on_subscribe(uint16_t conn_handle,
                          uint16_t attr_handle,
                          uint8_t  cur_notify);

/* Called on disconnect to clear internal state. */
void battery_on_disconnect(uint16_t conn_handle);

/* Utilities (optional, useful for demos/classes) */
uint8_t battery_level_get(void);      /* 0..100%  */
void    battery_level_set(uint8_t v); /* forces level and (optionally) notifies */
void    notify_battery_level(void);   /* notifies immediately, if subscribed */

#ifdef __cplusplus
}
#endif
