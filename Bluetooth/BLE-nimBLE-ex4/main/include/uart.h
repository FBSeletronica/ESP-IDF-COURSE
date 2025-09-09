/*
 * UART BLE service - NUS (Nordic UART Service)
 * SPDX-FileCopyrightText: 2025 Fabio Souza
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */
#pragma once

#include <stdint.h>
#include "nimble/ble.h"
#include "host/ble_hs.h"
#include "relay.h"   

#ifdef __cplusplus
extern "C" {
#endif

// Handle values filled after ble_gatts_add_svcs()
extern uint16_t uart_rx_val_handle;  // Cliente -> ESP (WRITE)
extern uint16_t uart_tx_val_handle;  // ESP -> Cliente (NOTIFY)

// Initit Uart service and bind a relay instance
void uart_init(Relay *relay);

// UART RC callback (WRITE)
int uart_rx_chr_access(uint16_t conn_handle, uint16_t attr_handle,
                       struct ble_gatt_access_ctxt *ctxt, void *arg);

// UART TX callback (READ)
int uart_tx_chr_access(uint16_t conn_handle, uint16_t attr_handle,
                       struct ble_gatt_access_ctxt *ctxt, void *arg);

/* Subscribe of notifications on TX characteristic */
void uart_on_subscribe(uint16_t conn_handle, uint16_t attr_handle, uint8_t cur_notify);

// Send data via TX (notify). Returns 0 on success; BLE_HS_EBUSY if no subscriber.
int uart_notify(const void *data, uint16_t len);

#ifdef __cplusplus
}
#endif
