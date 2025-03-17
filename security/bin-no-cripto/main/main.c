/******************************************************************************
*  Extract sensitive data from a binary without cryptography
*  This example shows how to extract sensitive data from a binary file without cryptography.
*  The data is stored in flash using hardcoded strings, embedded files, NVS and FATFS.
*  The binary is read using esptool.py and with a simple analysis, a lot of sensitive data can be read easily.
*
* This code is part of the course "Academia ESP32 Profissional" by Fábio Souza
* The course is available on https://cursos.embarcados.com.br
*
* This example code is licensed under the Creative Commons Attribution 4.0 International License.
* (http://creativecommons.org/licenses/by/4.0/)
*
* Author: Fábio Souza
*******************************************************************************/

#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "nvs_flash.h"
#include "nvs.h"
#include "esp_vfs_fat.h"
#include "esp_log.h"

// Embedded file pointers
extern const uint8_t credentials_txt_start[] asm("_binary_credentials_txt_start");
extern const uint8_t credentials_txt_end[] asm("_binary_credentials_txt_end");

static const char *TAG = "SENSITIVE_DEMO";

// Function to simulate saving sensitive data to NVS
void save_sensitive_data_to_nvs()
{
    nvs_handle_t handle;
    esp_err_t err = nvs_open("storage", NVS_READWRITE, &handle);

    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Error (%d) opening NVS handle!", err);
        return;
    }

    // Sensitive data to be saved
    nvs_set_str(handle, "user", "admin");
    nvs_set_str(handle, "password", "senha_super_secreta_987");
    const uint8_t secret_key[16] = { 0xde, 0xad, 0xbe, 0xef, 0xca, 0xfe, 0xba, 0xbe, 0xfa, 0xce, 0xb0, 0x0c, 0xde, 0xad, 0xbe, 0xef };
    nvs_set_blob(handle, "secret_key", secret_key, sizeof(secret_key));
    const char *jwt_token = "eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9...";
    nvs_set_str(handle, "jwt_token", jwt_token);
    nvs_commit(handle);
    nvs_close(handle);

    ESP_LOGI(TAG, "Sensitive data saved to NVS");
}

// Function to read sensitive data from FATFS
void read_fatfs_file(const char *path)
{
    FILE *file = fopen(path, "r");
    if (file == NULL) {
        ESP_LOGE(TAG, "Error opening file %s", path);
        return;
    }

    char buf[128];
    ESP_LOGI(TAG, "Content of %s:", path);
    while (fgets(buf, sizeof(buf), file)) {
        printf("%s", buf);
    }
    fclose(file);
}

// Function to initialize FATFS (updated version)
void init_fatfs()
{
    const char *base_path = "/fatfs";
    const char *partition_label = "storage"; // Must match your partition label in partitions.csv

    esp_vfs_fat_mount_config_t mount_config = {
        .max_files = 4,
        .format_if_mount_failed = true,
        .allocation_unit_size = CONFIG_WL_SECTOR_SIZE
    };

    wl_handle_t s_wl_handle;

    // ✅ Updated: use esp_vfs_fat_spiflash_mount_rw_wl()
    esp_err_t ret = esp_vfs_fat_spiflash_mount_rw_wl(base_path, partition_label, &mount_config, &s_wl_handle);

    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to mount FATFS (%s)", esp_err_to_name(ret));
    } else {
        ESP_LOGI(TAG, "FATFS mounted successfully at '%s'", base_path);
    }
}

// Main function
void app_main(void)
{
    ESP_LOGI(TAG, "Starting sensitive data demo (FATFS version)");

    // Sensitive data stored in hardcoded strings
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

    // Initialize NVS and save sensitive data
    nvs_flash_init();
    save_sensitive_data_to_nvs();

    // Initialize FATFS and read sensitive files
    init_fatfs();
    read_fatfs_file("/fatfs/apikey.txt");
    read_fatfs_file("/fatfs/senha_banco.txt");
    read_fatfs_file("/fatfs/aws_cert.pem");
    read_fatfs_file("/fatfs/aws_private_key.pem");
    read_fatfs_file("/fatfs/config.json");
    read_fatfs_file("/fatfs/tokens_seguranca.txt");
    read_fatfs_file("/fatfs/bd_clientes.db");

    while (1) {
        vTaskDelay(pdMS_TO_TICKS(5000));
    }
}
