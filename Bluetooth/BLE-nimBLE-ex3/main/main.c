/******************************************************************************
*  Bluetooth LE GATT Server Example - revised
*  This example creates a GATT server with: 
*   - GAP (name, flags, advertising params/fields)
*   - GATT (services, characteristics, descriptors)
*   - Heart Rate Service (0x180D)
*   - Battery Service (0x180F)
*   - LED Control Service (custom UUID)
*   - Uses NimBLE stack on ESP32
*   - Simulates heart rate and battery level
*   - Controls an LED via a writable characteristic
*   
*   I revised from the original example to better structure and comments. 
*   Now the code is more modular and easier to understand and add new services.
* 
* This code is part of the course "Academia ESP32 Profissional" by Fábio Souza
* The course is available on https://cursos.embarcados.com.br
*
* This example code Creative Commons Attribution 4.0 International License.
* When using the code, you must keep the above copyright notice,
* this list of conditions and the following disclaimer in the source code.
* (http://creativecommons.org/licenses/by/4.0/)

* Author: Fábio Souza
* This code is for teaching purposes only.
* No warranty of any kind is provided.
*******************************************************************************/

/* Includes */
#include "common.h"
#include "gap.h"
#include "gatt_svc.h"
#include "heart_rate.h"
#include "led.h"
#include "battery.h"

/* NimBLE store */
void ble_store_config_init(void);

/* Private function declarations */
static void on_stack_reset(int reason);
static void on_stack_sync(void);
static void nimble_host_config_init(void);
static void nimble_host_task(void *param);

/* ---------------- Stack event callbacks ---------------- */
static void on_stack_reset(int reason) {
    ESP_LOGI(TAG, "nimble stack reset, reason: %d", reason);
}

static void on_stack_sync(void) {
    /* Init advertising when stack is ready */
    adv_init();
}

static void nimble_host_config_init(void) {
    /* Callbacks do host */
    ble_hs_cfg.reset_cb          = on_stack_reset;           // Reset callback
    ble_hs_cfg.sync_cb           = on_stack_sync;            // Sync callback
    ble_hs_cfg.gatts_register_cb = gatt_svr_register_cb;     // GATT registration
    ble_hs_cfg.store_status_cb   = ble_store_util_status_rr; // Storage status

    /* Store bonds/keys in NVS */
    ble_store_config_init();
}

static void nimble_host_task(void *param) {
    ESP_LOGI(TAG, "nimble host task started");
    nimble_port_run();  //init the host stack 
    vTaskDelete(NULL);  //should never return
}

/* ---------------------- app_main ------------------------ */
void app_main(void) {
    int rc;
    esp_err_t ret;
    

    /* NVS (needed for BLE stack) */
    ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "failed to initialize NVS, err=%d", ret);
        return;
    }

    /* NimBLE stack */
    ret = nimble_port_init();
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "failed to init NimBLE, err=%d", ret);
        return;
    }

    /* GAP (name, flags, advertising params/fields) */
    rc = gap_init();
    if (rc != 0) {
        ESP_LOGE(TAG, "failed to init GAP, rc=%d", rc);
        return;
    }

    /* GATT (services, characteristics, descriptors) */
    rc = gatt_svc_init();
    if (rc != 0) {
        ESP_LOGE(TAG, "failed to init GATT server, rc=%d", rc);
        return;
    }

    /* Init services */
    heart_rate_init();  // heart rate service
    battery_init();     // battery service
    led_init();         // led control service

    /* Configure callbacks and initialize host */
    nimble_host_config_init();

    /* Start the host task */
    xTaskCreate(nimble_host_task, "NimBLE Host", 4 * 1024, NULL, 5, NULL);
}
