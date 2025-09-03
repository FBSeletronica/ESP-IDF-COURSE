/*
 * SPDX-FileCopyrightText: 2025 Fabio Souza
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */

#include "battery.h"

#include <string.h>
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/timers.h"
#include "host/ble_hs.h"

#ifndef TAG
#define TAG "BAS"
#endif

/* Exported in the header: filled by NimBLE when registering the GATT */
uint16_t battery_chr_val_handle = 0;

/* --- Internal module state --- */
static TimerHandle_t s_batt_timer = NULL;
static uint16_t s_conn_handle = BLE_HS_CONN_HANDLE_NONE;
static uint8_t  s_level = 95;          /* initial simulated level */
static bool     s_notify_enabled = false;

/* --- Internal utilities --- */
static inline uint8_t clamp_u8(int v, int lo, int hi) {
    if (v < lo) v = lo;
    if (v > hi) v = hi;
    return (uint8_t)v;
}

/* Simple simulation: decrease until 30%, then reset to 100% */
static void battery_sim_step(void) {
    if (s_level > 30) s_level--;
    else s_level = 100;
}

/* Sends a notification with the current value (if there is a subscriber) */
static void battery_notify_if_needed(void) {
    if (!s_notify_enabled || s_conn_handle == BLE_HS_CONN_HANDLE_NONE) {
        return;
    }

    struct os_mbuf *om = ble_hs_mbuf_from_flat(&s_level, sizeof(s_level));
    if (!om) {
        ESP_LOGW(TAG, "alloc mbuf failed");
        return;
    }

    int rc = ble_gatts_notify_custom(s_conn_handle, battery_chr_val_handle, om);
    if (rc != 0) {
        /* om is released by the stack on success; on error, it has already been released */
        ESP_LOGW(TAG, "notify failed (rc=%d)", rc);
    }
}

/* Timer callback (1s): simulates and notifies */
static void battery_timer_cb(TimerHandle_t xTimer) {
    (void)xTimer;
    battery_sim_step();
    battery_notify_if_needed();
}

/* --- Public API --- */
void battery_init(void) {
    /* Creates the periodic 1s timer for simulation/notification */
    s_batt_timer = xTimerCreate("bas_timer",
                                pdMS_TO_TICKS(1000),
                                pdTRUE,   /* auto-reload */
                                NULL,
                                battery_timer_cb);

    if (!s_batt_timer) {
        ESP_LOGE(TAG, "xTimerCreate failed");
    }

    s_conn_handle = BLE_HS_CONN_HANDLE_NONE;
    s_notify_enabled = false;
    s_level = clamp_u8(s_level, 0, 100);
}

uint8_t battery_level_get(void) {
    return s_level;
}

void battery_level_set(uint8_t v) {
    s_level = clamp_u8(v, 0, 100);
    /* Optional: notify immediately every time the value is set */
    battery_notify_if_needed();
}

void notify_battery_level(void) {
    battery_notify_if_needed();
}

/* Access CB for the Battery Level characteristic (0x2A19): READ (uint8) */
int battery_chr_access(uint16_t conn_handle,
                       uint16_t attr_handle,
                       struct ble_gatt_access_ctxt *ctxt,
                       void *arg)
{
    (void)conn_handle;
    (void)attr_handle;
    (void)arg;

    uint8_t v = battery_level_get();
    int rc = os_mbuf_append(ctxt->om, &v, sizeof(v));
    return (rc == 0) ? 0 : BLE_ATT_ERR_INSUFFICIENT_RES;
}

/* Subscribe/unsubscribe received in GAP (NOTIFY for Battery Level) */
void battery_on_subscribe(uint16_t conn_handle,
                          uint16_t attr_handle,
                          uint8_t  cur_notify)
{
    if (attr_handle != battery_chr_val_handle) {
        /* Not the Battery Level characteristic */
        return;
    }

    s_conn_handle = conn_handle;
    s_notify_enabled = (cur_notify != 0);

    if (s_notify_enabled) {
        /* Notify the current value immediately and ensure the timer is running */
        battery_notify_if_needed();
        if (s_batt_timer) xTimerStart(s_batt_timer, 0);
    } else {
        if (s_batt_timer) xTimerStop(s_batt_timer, 0);
    }
}

/* Clears state on disconnect */
void battery_on_disconnect(uint16_t conn_handle) {
    if (conn_handle != s_conn_handle) return;

    s_conn_handle = BLE_HS_CONN_HANDLE_NONE;
    s_notify_enabled = false;

    if (s_batt_timer) {
        xTimerStop(s_batt_timer, 0);
    }
}
