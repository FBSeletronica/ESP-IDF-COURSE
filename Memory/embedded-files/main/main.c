/******************************************************************************
* Embedding Binary Data in ESP32 Flash Memory
* This example shows how to embed binary data in the flash memory of the ESP32.
* First we will configure the CMakeLists.txt file to include the binary data in the build process.
* Then we will use the asm() function to get the address of the start and end of the binary data.
* We will also use the address to calculate the size of the binary data.
* Finally, we will use the address to access the binary data and print it to the console.

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
#include "esp_log.h"
static const char *TAG = "main test binary data";

void app_main(void)
{

    ESP_LOGI(TAG, "Page.html");
    extern const uint8_t page_html_start[] asm("_binary_page_html_start");
    extern const uint8_t page_html_end[]   asm("_binary_page_html_end");

    //log the address of the start and end of the binary
    ESP_LOGI(TAG, "START Address: %p, END Adress: %p", page_html_start, page_html_end);
    //log the size of the binary
    ESP_LOGI(TAG, "SIZE: %d", page_html_end - page_html_start);
    //log all content of the binary
    ESP_LOGI(TAG, "CONTENT:\n\n%s", page_html_start);
    
    ESP_LOGI(TAG, "Sample.txt");
    extern const uint8_t sample_txt_start[] asm("_binary_sample_txt_start");
    extern const uint8_t sample_txt_end[]   asm("_binary_sample_txt_end");

    //log the address of the start and end of the binary
    ESP_LOGI(TAG, "START Address: %p, END Adress: %p", sample_txt_start, sample_txt_end);
    //log the size of the binary
    ESP_LOGI(TAG, "SIZE: %d", sample_txt_end - sample_txt_start);
    //log all content of the binary
    ESP_LOGI(TAG, "CONTENT:\n\n%s", sample_txt_start);


    ESP_LOGI(TAG, "Favicon");
    extern const uint8_t favicon_ico_start[] asm("_binary_favicon_ico_start");
    extern const uint8_t favicon_ico_end[]   asm("_binary_favicon_ico_end");

    //log the address of the start and end of the binary
    ESP_LOGI(TAG, "START Address: %p, END Adress: %p", favicon_ico_start, favicon_ico_end);
    //log the size of the binary
    ESP_LOGI(TAG, "SIZE: %d", favicon_ico_end - favicon_ico_start);
    

}
