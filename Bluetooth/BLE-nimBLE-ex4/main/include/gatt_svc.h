/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */
#ifndef GATT_SVR_H
#define GATT_SVR_H

/* NimBLE GATT/GAP APIs */
#include "host/ble_gatt.h"
#include "services/gatt/ble_svc_gatt.h"
#include "host/ble_gap.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Callbacks de registro/subscribe e inicialização do catálogo GATT */
void gatt_svr_register_cb(struct ble_gatt_register_ctxt *ctxt, void *arg);
void gatt_svr_subscribe_cb(struct ble_gap_event *event);
int  gatt_svc_init(void);

#ifdef __cplusplus
}
#endif

#endif /* GATT_SVR_H */
