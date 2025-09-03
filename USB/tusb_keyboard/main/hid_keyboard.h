/*
 * SPDX-FileCopyrightText: 2022-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */

#include "esp_err.h"
#include "tinyusb.h"

esp_err_t hid_keyboard_init(void);
void hid_keyboard_send(uint8_t keycode[6]);
