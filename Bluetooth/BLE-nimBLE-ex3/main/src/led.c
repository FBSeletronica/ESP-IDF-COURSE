/*
 * SPDX-FileCopyrightText: 2025 Fabio Souza
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */

/* Includes */
#include "led.h"
#include "common.h"        /* your header (logs/global defines) */
#include "host/ble_hs.h"   /* NimBLE */
#include "os/os_mbuf.h"    /* os_mbuf_* APIs for READ/WRITE payload */

/* ------------------------------------------------------------- */
/* LED state (shared between HW and BLE)                         */
/* ------------------------------------------------------------- */
static uint8_t led_state = 0;

/* Exported in the header: filled after ble_gatts_add_svcs() */
uint16_t led_chr_val_handle = 0;

#ifdef CONFIG_BLINK_LED_STRIP
static led_strip_handle_t led_strip;
#endif

/* ------------------- BLE Implementation ---------------------- */
int led_chr_access(uint16_t conn_handle,
                   uint16_t attr_handle,
                   struct ble_gatt_access_ctxt *ctxt,
                   void *arg)
{
    (void)conn_handle;
    (void)attr_handle;
    (void)arg;

    switch (ctxt->op) {
    case BLE_GATT_ACCESS_OP_WRITE_CHR: {
        if (OS_MBUF_PKTLEN(ctxt->om) < 1) {
            return BLE_ATT_ERR_INVALID_ATTR_VALUE_LEN;
        }
        uint8_t v = 0;
        int rc = os_mbuf_copydata(ctxt->om, 0, 1, &v);
        if (rc != 0) {
            return BLE_ATT_ERR_UNLIKELY;
        }

        if (v) {
            led_on();
            ESP_LOGI(TAG, "LED ON (write=0x%02X)", v);
        } else {
            led_off();
            ESP_LOGI(TAG, "LED OFF (write=0x%02X)", v);
        }
        return 0;
    }

    case BLE_GATT_ACCESS_OP_READ_CHR: {
        /* Only works if the characteristic's .flags include READ */
        uint8_t v = get_led_state() ? 1 : 0;
        return os_mbuf_append(ctxt->om, &v, sizeof(v)) == 0
               ? 0
               : BLE_ATT_ERR_INSUFFICIENT_RES;
    }

    default:
        return BLE_ATT_ERR_UNLIKELY;
    }
}

/* ------------------- HW Implementation -------------------- */
uint8_t get_led_state(void) { return led_state; }

#ifdef CONFIG_BLINK_LED_STRIP

void led_on(void) {
    /* Set the LED pixel using RGB from 0 (0%) to 255 (100%) for each color */
    led_strip_set_pixel(led_strip, 0, 16, 16, 16);
    /* Refresh the strip to send data */
    led_strip_refresh(led_strip);
    /* Update LED state */
    led_state = 1;
}

void led_off(void) {
    /* Set all LED off to clear all pixels */
    led_strip_clear(led_strip);
    /* Update LED state */
    led_state = 0;
}

void led_init(void) {
    ESP_LOGI(TAG, "example configured to blink addressable led!");

    /* LED strip initialization with the GPIO and pixels number */
    led_strip_config_t strip_config = {
        .strip_gpio_num = BLINK_GPIO,
        .max_leds = 1, // at least one LED on board
    };
#if CONFIG_BLINK_LED_STRIP_BACKEND_RMT
    led_strip_rmt_config_t rmt_config = {
        .resolution_hz = 10 * 1000 * 1000, // 10MHz
        .flags.with_dma = false,
    };
    ESP_ERROR_CHECK(led_strip_new_rmt_device(&strip_config, &rmt_config, &led_strip));
#elif CONFIG_BLINK_LED_STRIP_BACKEND_SPI
    led_strip_spi_config_t spi_config = {
        .spi_bus = SPI2_HOST,
        .flags.with_dma = true,
    };
    ESP_ERROR_CHECK(led_strip_new_spi_device(&strip_config, &spi_config, &led_strip));
#else
#error "unsupported LED strip backend"
#endif

    /* Initial state = OFF */
    led_off();
}

#elif CONFIG_BLINK_LED_GPIO

void led_on(void)  { gpio_set_level(BLINK_GPIO, 1); led_state = 1; }
void led_off(void) { gpio_set_level(BLINK_GPIO, 0); led_state = 0; }

void led_init(void) {
    ESP_LOGI(TAG, "example configured to blink gpio led!");
    gpio_reset_pin(BLINK_GPIO);
    /* Set the GPIO as a push/pull output */
    gpio_set_direction(BLINK_GPIO, GPIO_MODE_OUTPUT);
    /* Initial state = OFF */
    led_off();
}

#else
#error "unsupported LED type"
#endif
