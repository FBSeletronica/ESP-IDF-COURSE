/*
 * SPDX-FileCopyrightText: 2025 Fabio Souza
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */

#include "heart_rate.h"
#include "common.h"

#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/timers.h"
#include "host/ble_hs.h"
#include "os/os_mbuf.h"

#ifndef TAG
#define TAG "HR_SVC"
#endif

/* Handle exported in the header; filled by ble_gatts_add_svcs() */
uint16_t heart_rate_chr_val_handle = 0;

/* ---- Internal state ---- */
static TimerHandle_t s_hr_timer = NULL;
static uint16_t s_conn_handle   = BLE_HS_CONN_HANDLE_NONE;
static bool s_notify_enabled    = false;

/* Mock value (starts at 72) */
static uint8_t s_heart_rate = 72;

/* ---- Data mock (public API) ---- */
uint8_t get_heart_rate(void) { return s_heart_rate; }

/* 60..80 bpm */
void update_heart_rate(void) {
    s_heart_rate = 60 + (uint8_t)(esp_random() % 21);
}

/* ---- Helpers ---- */
static int hr_build(uint8_t *buf, size_t buflen) {
    if (buflen < 2) return -1;
    buf[0] = 0x00;              // Flags = 0 (HR 8-bit)
    buf[1] = get_heart_rate();  // HR
    return 2;
}

static void hr_notify_now(void) {
    if (!s_notify_enabled || s_conn_handle == BLE_HS_CONN_HANDLE_NONE) {
        return;
    }

    uint8_t payload[2];
    int n = hr_build(payload, sizeof(payload));
    if (n < 0) return;

    struct os_mbuf *om = ble_hs_mbuf_from_flat(payload, n);
    if (!om) return;

    /* No logs here to avoid stack overflow in Timer Service */
    (void) ble_gatts_notify_custom(s_conn_handle, heart_rate_chr_val_handle, om);
}

/* Timer: updates and notifies every 1s */
static void hr_timer_cb(TimerHandle_t xTimer) {
    (void)xTimer;
    update_heart_rate();
    hr_notify_now();
    // do not log here!
}

/* ---- BLE Integration (public API) ---- */
#ifndef HEART_RATE_TASK_PERIOD
#define HEART_RATE_TASK_PERIOD (1000 / portTICK_PERIOD_MS)
#endif

void heart_rate_init(void) {
    s_hr_timer = xTimerCreate("hr_timer",
                              HEART_RATE_TASK_PERIOD,
                              pdTRUE, NULL, hr_timer_cb);
    if (!s_hr_timer) ESP_LOGE(TAG, "xTimerCreate failed");
}

/* Optional READ (if the characteristic has READ flag) */
int heart_rate_chr_access(uint16_t conn_handle,
                          uint16_t attr_handle,
                          struct ble_gatt_access_ctxt *ctxt,
                          void *arg) {
    (void)conn_handle; (void)attr_handle; (void)arg;
    uint8_t payload[2];
    int n = hr_build(payload, sizeof(payload));
    if (n < 0) return BLE_ATT_ERR_UNLIKELY;
    return os_mbuf_append(ctxt->om, payload, n) == 0
           ? 0 : BLE_ATT_ERR_INSUFFICIENT_RES;
}

/* IMPORTANT: the 3rd argument here is cur_notify! */
void heart_rate_on_subscribe(uint16_t conn_handle,
                             uint16_t attr_handle,
                             uint8_t  cur_notify) {
    /* Ensure it is 0x2A37 (value handle) */
    if (attr_handle != heart_rate_chr_val_handle) {
        ESP_LOGW(TAG, "subscribe for unknown attr=%u (HR handle=%u)",
                 attr_handle, heart_rate_chr_val_handle);
        return;
    }

    s_conn_handle    = conn_handle;
    s_notify_enabled = (cur_notify != 0);

    ESP_LOGI(TAG, "HR subscribe: conn=%d attr=%u notify=%d",
             conn_handle, attr_handle, (int)cur_notify);

    if (s_notify_enabled) {
        hr_notify_now();                  // triggers one immediately
        if (s_hr_timer) xTimerStart(s_hr_timer, 0);
    } else {
        if (s_hr_timer) xTimerStop(s_hr_timer, 0);
    }
}

void heart_rate_on_disconnect(uint16_t conn_handle) {
    if (conn_handle != s_conn_handle) return;
    s_conn_handle    = BLE_HS_CONN_HANDLE_NONE;
    s_notify_enabled = false;
    if (s_hr_timer) xTimerStop(s_hr_timer, 0);
}

/* Compatibility: keeps the old name (now sends notify) */
void send_heart_rate_indication(void) {
    hr_notify_now();
}
