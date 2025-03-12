/******************************************************************************
*  Extract sensitive data from a binary without criptography
*  This example shows how to extract sensitive data from a binary file without criptography.
*  The data is stored in flash using hardcoded strings, embedded files, NVS and SPIFFS.
*  The binary is read using esptool.py and with a simple analysis, a lot of sensitive data can be readed easily.
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
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "nvs_flash.h"
#include "nvs.h"
#include "esp_spiffs.h"

// Embedded file pointers
extern const uint8_t credentials_txt_start[] asm("_binary_credentials_txt_start");
extern const uint8_t credentials_txt_end[] asm("_binary_credentials_txt_end");

// Funcion to simulate saving sensitive data to NVS
void save_sensitive_data_to_nvs()
{
    nvs_handle_t handle;
    esp_err_t err = nvs_open("storage", NVS_READWRITE, &handle);

    if (err != ESP_OK) {
        printf("Error (%d) opening NVS handle!\n", err);
        return;
    }

    // Sensitive data to be saved
    nvs_set_str(handle, "user", "admin");
    nvs_set_str(handle, "password", "senha_super_secreta_987");
    nvs_commit(handle);
    nvs_close(handle);

    printf("Sensitive data saved to NVS\n");
}


// Function to read sensitive data from SPIFFS
// The file apikey.txt and senha_banco.txt must be created in the SPIFFS partition
void read_spiffs_file(const char *path)
{
    FILE *file = fopen(path, "r");
    if (file == NULL) {
        printf("Error opening file %s\n", path);
        return;
    }

    char buf[128];
    printf("Content of %s:\n", path);
    while (fgets(buf, sizeof(buf), file)) {
        printf("%s", buf);
    }
    fclose(file);
}

// Function to initialize SPIFFS
void init_spiffs()
{
    esp_vfs_spiffs_conf_t conf = {
        .base_path = "/spiffs",
        .partition_label = NULL,
        .max_files = 5,
        .format_if_mount_failed = true
    };

    esp_err_t ret = esp_vfs_spiffs_register(&conf);
    if (ret != ESP_OK) {
        printf("Error (%d) initializing SPIFFS\n", ret);
    } else {
        printf("SPIFFS initialized\n");
    }
}

// Main function
void app_main(void)
{
    ESP_LOGI(TAG, "Starting sensitive data demo");

    // Sensitve data stored in hardcoded strings
    const char *wifi_ssid = "MeuWiFi";
    const char *wifi_password = "senha_no_codigo_123";
    const char *api_key = "API_KEY_INLINE_ABC123";
    const char *private_key = "PRIVATE_KEY_INLINE-----BEGIN PRIVATE KEY-----...";

    // Print sensitive data stored in hardcoded strings
    ESP_LOGI(TAG, "Sensitive data stored in hardcoded strings:");
    ESP_LOGI(TAG, "WiFi SSID: %s", wifi_ssid);
    ESP_LOGI(TAG, "WiFi Password: %s", wifi_password);
    ESP_LOGI(TAG, "API Key: %s", api_key);
    ESP_LOGI(TAG, "Private Key: %s", private_key);


    // Print sensitive data stored in embedded file
    size_t credentials_size = credentials_txt_end - credentials_txt_start;
    printf("\n=== Embedded credentials.txt ===\n");
    printf("%.*s\n", credentials_size, credentials_txt_start);

    // Save sensitive data to NVS
    nvs_flash_init();
    save_sensitive_data_to_nvs();

    // Read sensitive data from SPIFFS
    init_spiffs();
    read_spiffs_file("/spiffs/apikey.txt");
    read_spiffs_file("/spiffs/senha_banco.txt");

    while (1) {
        vTaskDelay(pdMS_TO_TICKS(5000));    
    }
}
