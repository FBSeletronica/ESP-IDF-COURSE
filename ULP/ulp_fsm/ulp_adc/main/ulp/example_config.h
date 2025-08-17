/*
 * SPDX-FileCopyrightText: 2022-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */
#pragma once

/* Ints are used here to be able to include the file in assembly as well */
#define EXAMPLE_ADC_CHANNEL     0 // ADC_CHANNEL_0, GPIO36 on ESP32, GPIO1 on ESP32-S3
#define EXAMPLE_ADC_UNIT        0 // ADC_UNIT_1
#define EXAMPLE_ADC_ATTEN       3 // ADC_ATTEN_DB_12
#define EXAMPLE_ADC_WIDTH       0 // ADC_BITWIDTH_DEFAULT

/* Set low and high thresholds */
#define EXAMPLE_ADC_LOW_TRESHOLD    2000
#define EXAMPLE_ADC_HIGH_TRESHOLD   7600
