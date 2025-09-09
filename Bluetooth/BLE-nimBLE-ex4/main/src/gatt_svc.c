/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */

/* Includes (catálogo GATT + módulos) */
#include "gatt_svc.h"
#include "common.h"

#include "heart_rate.h"
#include "battery.h"
#include "led.h"
#include "uart.h"

#include "esp_log.h"
#include "host/ble_hs.h"
#include "host/ble_uuid.h"        /* ble_uuid_to_str */

/* TAG padrão (caso não venha de common.h) */
#ifndef TAG
#define TAG "gatt_svc"
#endif

/* ---------------------- Catálogo GATT ---------------------- */
static const struct ble_gatt_svc_def gatt_svr_svcs[] = {
    /* Heart Rate (0x180D) */
    {
    .type = BLE_GATT_SVC_TYPE_PRIMARY,
    .uuid = BLE_UUID16_DECLARE(0x180D),
    .characteristics = (struct ble_gatt_chr_def[]) {
        {
        .uuid       = BLE_UUID16_DECLARE(0x2A37),   // Heart Rate Measurement
        .access_cb  = heart_rate_chr_access,        // ok manter (ou NULL se não quiser READ)
        .flags      = BLE_GATT_CHR_F_READ | BLE_GATT_CHR_F_NOTIFY,        // (ou BLE_GATT_CHR_F_READ | BLE_GATT_CHR_F_NOTIFY)
        .val_handle = &heart_rate_chr_val_handle,
        },
        { 0 }
    },
    },

    /* Automation IO (0x1815) -> LED (UUID128 custom) */
    {
        .type = BLE_GATT_SVC_TYPE_PRIMARY,
        .uuid = BLE_UUID16_DECLARE(0x1815),
        .characteristics = (struct ble_gatt_chr_def[]) {
            {
                .uuid = BLE_UUID128_DECLARE(
                            0x23,0xD1,0xBC,0xEA,0x5F,0x78,0x23,0x15,
                            0xDE,0xEF,0x12,0x12,0x25,0x15,0x00,0x00), // → 00001525-1212-EFDE-1523-785FEABCD123
                .access_cb  = led_chr_access,
                .flags      = BLE_GATT_CHR_F_WRITE | BLE_GATT_CHR_F_READ, // (READ opcional p/ consultar estado)
                .val_handle = &led_chr_val_handle,
            },
            { 0 }
        },
    },

    /* Battery Service (0x180F) */
    {
        .type = BLE_GATT_SVC_TYPE_PRIMARY,
        .uuid = BLE_UUID16_DECLARE(0x180F),
        .characteristics = (struct ble_gatt_chr_def[]) {
            {
                .uuid       = BLE_UUID16_DECLARE(0x2A19),   // Battery Level
                .access_cb  = battery_chr_access,           // em battery.c
                .flags      = BLE_GATT_CHR_F_READ | BLE_GATT_CHR_F_NOTIFY,
                .val_handle = &battery_chr_val_handle,      // exportado por battery.c
            },
            { 0 }
        },
    },

    /* Nordic UART Service (NUS)
    * Service: 6E400001-B5A3-F393-E0A9-E50E24DCCA9E
    * RX (Write/WriteNR): 6E400002-B5A3-F393-E0A9-E50E24DCCA9E
    * TX (Notify):        6E400003-B5A3-F393-E0A9-E50E24DCCA9E
    */
{
  .type = BLE_GATT_SVC_TYPE_PRIMARY,
  /* 6E400001-B5A3-F393-E0A9-E50E24DCCA9E (bytes LSB->MSB) */
  .uuid = BLE_UUID128_DECLARE(0x9E,0xCA,0xDC,0x24,0x0E,0xE5,0xA9,0xE0,
                              0x93,0xF3,0xA3,0xB5,0x01,0x00,0x40,0x6E),
  .characteristics = (struct ble_gatt_chr_def[]) {
    { /* TX: ESP -> Cliente (Notify) = 6E400003-B5A3-F393-E0A9-E50E24DCCA9E */
      .uuid = BLE_UUID128_DECLARE(0x9E,0xCA,0xDC,0x24,0x0E,0xE5,0xA9,0xE0,
                                  0x93,0xF3,0xA3,0xB5,0x03,0x00,0x40,0x6E),
      .access_cb  = uart_tx_chr_access,        /* READ opcional */
      .flags      = BLE_GATT_CHR_F_NOTIFY | BLE_GATT_CHR_F_READ,
      .val_handle = &uart_tx_val_handle,
    },
    { /* RX: Cliente -> ESP (Write / Write No Rsp) = 6E400002-B5A3-F393-E0A9-E50E24DCCA9E */
      .uuid = BLE_UUID128_DECLARE(0x9E,0xCA,0xDC,0x24,0x0E,0xE5,0xA9,0xE0,
                                  0x93,0xF3,0xA3,0xB5,0x02,0x00,0x40,0x6E),
      .access_cb  = uart_rx_chr_access,
      .flags      = BLE_GATT_CHR_F_WRITE | BLE_GATT_CHR_F_WRITE_NO_RSP,
      .val_handle = &uart_rx_val_handle,
    },
    { 0 }
  },
},

    { 0 } // fim dos serviços
};

/* ----------------- Callback de registro (log) -------------- */
void gatt_svr_register_cb(struct ble_gatt_register_ctxt *ctxt, void *arg)
{
    (void)arg;
    char buf[BLE_UUID_STR_LEN];

    switch (ctxt->op) {
    case BLE_GATT_REGISTER_OP_SVC:
        ESP_LOGD(TAG, "registered service %s handle=%d",
                 ble_uuid_to_str(ctxt->svc.svc_def->uuid, buf),
                 ctxt->svc.handle);
        break;

    case BLE_GATT_REGISTER_OP_CHR:
        ESP_LOGD(TAG, "registered characteristic %s def_handle=%d val_handle=%d",
                 ble_uuid_to_str(ctxt->chr.chr_def->uuid, buf),
                 ctxt->chr.def_handle, ctxt->chr.val_handle);
        break;

    case BLE_GATT_REGISTER_OP_DSC:
        ESP_LOGD(TAG, "registered descriptor %s handle=%d",
                 ble_uuid_to_str(ctxt->dsc.dsc_def->uuid, buf),
                 ctxt->dsc.handle);
        break;

    default:
        assert(0);
        break;
    }
}

/* -------------- Roteador de subscribe (notify/indicate) ----- */
void gatt_svr_subscribe_cb(struct ble_gap_event *event)
{
    if (event->subscribe.conn_handle != BLE_HS_CONN_HANDLE_NONE) {
        ESP_LOGI(TAG, "subscribe: conn=%d attr=%d notify=%d indicate=%d",
                 event->subscribe.conn_handle,
                 event->subscribe.attr_handle,
                 event->subscribe.cur_notify,
                 event->subscribe.cur_indicate);
    } else {
        ESP_LOGI(TAG, "subscribe by stack: attr=%d", event->subscribe.attr_handle);
    }

    if (event->subscribe.attr_handle == heart_rate_chr_val_handle) {
        /* Heart Rate usa INDICATE */
        heart_rate_on_subscribe(event->subscribe.conn_handle,
                                event->subscribe.attr_handle,
                                event->subscribe.cur_notify);
    } else if (event->subscribe.attr_handle == battery_chr_val_handle) {
        /* Battery usa NOTIFY */
        battery_on_subscribe(event->subscribe.conn_handle,
                             event->subscribe.attr_handle,
                             event->subscribe.cur_notify);
    
    } else if (event->subscribe.attr_handle == uart_tx_val_handle) {
        /* UART usa NOTIFY */
        uart_on_subscribe(event->subscribe.conn_handle,
                      event->subscribe.attr_handle,
                      event->subscribe.cur_notify);
}
    else {
        ESP_LOGW(TAG, "subscribe for unknown attr=%d", event->subscribe.attr_handle);
    }
}

/* ----------------------- Inicialização ---------------------- */
int gatt_svc_init(void)
{
    int rc;

    /* Serviços padrão do GATT (Device Info, etc., se usados) */
    ble_svc_gatt_init();

    rc = ble_gatts_count_cfg(gatt_svr_svcs);
    if (rc != 0) {
        return rc;
    }

    rc = ble_gatts_add_svcs(gatt_svr_svcs);
    if (rc != 0) {
        return rc;
    }

    return 0;
}
