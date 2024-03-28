/******************************************************************************
* LittleFS Example
* This example shows how to use LittleFS with ESP32
* This code is part of the course "Programe o ESP32 com ESP-IDF 5" by Fábio Souza
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
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>
#include "esp_flash.h"
#include "esp_log.h"
#include "esp_littlefs.h"

static const char *TAG = "Test LittleFS";

void app_main(void)
{
        
    ESP_LOGI(TAG, "This is a LittleFS example");

    uint32_t size_flash_chip = 0;
    esp_flash_get_size(NULL, &size_flash_chip);
    ESP_LOGI(TAG, "Flash chip size: %uMB", (unsigned int)size_flash_chip >> 20);

    ESP_LOGI(TAG, "Initializing LittleFS");

    esp_vfs_littlefs_conf_t conf = 
    {
        .base_path = "/littlefs",
        .partition_label = "littlefs",
        .format_if_mount_failed = true,
        .dont_mount = false,
    };

    // Initialize LittleFS
    esp_err_t ret = esp_vfs_littlefs_register(&conf);   	    // Register LittleFS with VFS

    // Test if LittleFS is mounted
    if (ret != ESP_OK)                                
    {
        if (ret == ESP_FAIL)                                            // If mounting failed
        {
            ESP_LOGE(TAG, "Failed to mount or format filesystem");      // Log error
        }
        else if (ret == ESP_ERR_NOT_FOUND)                              // If partition not found
        {
            ESP_LOGE(TAG, "Failed to find LittleFS partition");         // Log error
        }
        else                                                            // If initialization failed
        {
            ESP_LOGE(TAG, "Failed to initialize LittleFS (%s)", esp_err_to_name(ret));  // Log error
        }
    }

    // Get LittleFS partition information
    size_t total = 0, used = 0;                                                                         // Initialize variables
    ret = esp_littlefs_info(conf.partition_label, &total, &used);                                       // Get partition information
    if (ret != ESP_OK)                                                                                 // If failed
    {   
        ESP_LOGE(TAG, "Failed to get LittleFS partition information (%s)", esp_err_to_name(ret));       // Log error
    }
    else                                                                                               // If successful
    {
        ESP_LOGI(TAG, "Partition size: total: %d, used: %d", total, used);                            // Log partition information
    }

    // Working with files
    ESP_LOGI(TAG, "Opening file");                                      // Log message
    FILE *f = fopen("/littlefs/hello.txt", "w");                        // Open file for writing
    if (f == NULL)                                                      // If file not opened
    {
        ESP_LOGE(TAG, "Failed to open file for writing");               // Log error
        return;                                                         // Exit function
    }
    fprintf(f, "Hello World From a TXT file!\nThis is an example for LittleFS");    // Write to file
    fclose(f);                                                                      // Close file                         
    ESP_LOGI(TAG, "File written");                                                  // Log message

    //Rename file
    // Check if destination file exists before renaming
    struct stat st;
    if (stat("/littlefs/foo.txt", &st) == 0)                            // Check if file exists
    {
            // Delete it if it exists
            unlink("/littlefs/foo.txt");                                // Delete file  
    }

    // Rename original file
    ESP_LOGI(TAG, "Renaming file");
    if (rename("/littlefs/hello.txt", "/littlefs/foo.txt") != 0)
    {
            ESP_LOGE(TAG, "Rename failed");
            return;
    }

    // Open renamed file for reading
    ESP_LOGI(TAG, "Reading file");                              // Log message
    f = fopen("/littlefs/foo.txt", "r");                        // Open file for reading
    if (f == NULL)                                              // if file not opened   
    {
            ESP_LOGE(TAG, "Failed to open file for reading");   // Log error
            return;
    }

    // Read file
    char line[128];                                    // Initialize buffer
    while(fgets(line, sizeof(line), f)!=NULL)         // Read file
    {
        printf("%s", line);                           // Print file
    }
    printf("\n");                                    // Print new line
    fclose(f);                                      // Close file
      
    // Remove file
    ESP_LOGI(TAG, "Removing file");                                      // Log message    
    // All done, unmount partition and disable LittleFS
    esp_vfs_littlefs_unregister(conf.partition_label);
    ESP_LOGI(TAG, "LittleFS unmounted");
}