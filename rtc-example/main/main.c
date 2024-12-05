/******************************************************************************
*  RTC 
* 
*  This example shows how to implement a Real-Time Clock (RTC) using the ESP32.
*  The ESP Timer (High Resolution Timer) is used to ensure accurate time tracking.
*  This code is part of the course "Programe o ESP32 com ESP-IDF 5" by Fábio Souza
*  The course is available on https://cursos.embarcados.com.br
* 
*  This example code is licensed under the Creative Commons Attribution 4.0 International License.
*  When using the code, you must keep the above copyright notice,
*  this list of conditions and the following disclaimer in the source code.
*  (http://creativecommons.org/licenses/by/4.0/)
*
* Author: Fábio Souza
* This code is for teaching purposes only.
* No warranty of any kind is provided.
*******************************************************************************/

#include <stdio.h>
#include <time.h>
#include <sys/time.h>
#include "esp_log.h"
#include "esp_timer.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

static const char *TAG = "RTC_UPDATE";

// Global variables for timer
static esp_timer_handle_t periodic_timer;

// Function to configure the system date and time
void set_system_time(int year, int month, int day, int hour, int minute, int second) {
    struct tm timeinfo = {
        .tm_year = year - 1900, // Year since 1900
        .tm_mon = month - 1,    // Month (0-11)
        .tm_mday = day,         // Day of the month
        .tm_hour = hour,        // Hour (0-23)
        .tm_min = minute,       // Minute (0-59)
        .tm_sec = second,       // Second (0-59)
    };

    time_t t = mktime(&timeinfo); // Convert to time_t
    if (t == -1) {
        ESP_LOGE(TAG, "Failed to convert time.");
        return;
    }

    struct timeval now = {
        .tv_sec = t,
        .tv_usec = 0,
    };

    settimeofday(&now, NULL); // Update system time
    ESP_LOGI(TAG, "Time set to: %04d-%02d-%02d %02d:%02d:%02d",
             year, month, day, hour, minute, second);
}

// Function to print the current date and time
void print_current_time() {
    time_t now;
    struct tm timeinfo;

    time(&now);                   // Get current time
    localtime_r(&now, &timeinfo); // Convert to struct tm

    ESP_LOGI(TAG, "Current time: %04d-%02d-%02d %02d:%02d:%02d",
             timeinfo.tm_year + 1900, timeinfo.tm_mon + 1, timeinfo.tm_mday,
             timeinfo.tm_hour, timeinfo.tm_min, timeinfo.tm_sec);
}

// Timer callback function
void timer_callback(void *arg) {
    print_current_time();
}

void app_main(void) {
    // Configure the initial date and time
    set_system_time(2024, 12, 05, 18, 53, 0);

    // Configure the ESP Timer
    const esp_timer_create_args_t periodic_timer_args = {
        .callback = &timer_callback,
        .name = "periodic_timer"
    };

    ESP_ERROR_CHECK(esp_timer_create(&periodic_timer_args, &periodic_timer));
    ESP_ERROR_CHECK(esp_timer_start_periodic(periodic_timer, 1000000)); // 1-second interval in microseconds

    
}
