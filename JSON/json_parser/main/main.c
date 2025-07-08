/******************************************************************************
* WIFI Connect with HTTP Client request and JSON Parsing
* This example shows how to connect to a WiFi network, make a http request and
* parse the JSON received data using espressif/json_parser.
* https://components.espressif.com/components/espressif/json_parser/versions/1.0.3
*
* This code is part of the course "Academia ESP32 Profissional" by Fábio Souza
* The course is available on https://cursos.embarcados.com.br
*
* This example code Creative Commons Attribution 4.0 International License.
* When using the code, you must keep the above copyright notice,
* this list of conditions and the following disclaimer in the source code.
* (http://creativecommons.org/licenses/by/4.0/)
*
* Author: Fábio Souza and Vinícius Silva <silva.viniciusr@gmail.com>
* This code is for teaching purposes only.
* No warranty of any kind is provided.
*******************************************************************************/

#include <string.h>
#include <stdlib.h>
#include <stdio.h>

//freertos includes
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"

//esp-idf includes
#include "nvs_flash.h"
#include "esp_log.h"
#include "esp_err.h"

#include "wifi.h"
#include "http_client.h"
#include "json_parser.h"

#define HTTP_URL "http://api.openweathermap.org/data/2.5/weather?q=Campinas,BR&APPID=2631b64b1f9374c56bd15a3c87c19574"
#define BUFFER_SIZE (1024)

//tag for logging
static const char *TAG = "JSON Example";

//main func
void app_main(void)
{
    //Initialize NVS
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {   //if nvs flash is not initialized
      ESP_ERROR_CHECK(nvs_flash_erase());                                             //erase nvs flash
      ret = nvs_flash_init();                                                         //initialize nvs flash
    }
    ESP_ERROR_CHECK(ret);                                                             //check error

    //log wifi mode and initialize wifi
    ESP_LOGI(TAG, "ESP_WIFI_MODE_STA");
    wifi_init_sta();

    //Make an http request and save the reply in jstr
    char *jstr = (char*) calloc(1, BUFFER_SIZE);
    ESP_ERROR_CHECK(!jstr);
    ESP_ERROR_CHECK(http_client_request(HTTP_URL, jstr, BUFFER_SIZE));

    //start parsing the response
    jparse_ctx_t json_ctx;
    ESP_ERROR_CHECK(json_parse_start(&json_ctx, jstr, strlen(jstr)));

    //parse json data
    float temp; int  humidity;                              //local variables to hold data
    json_obj_get_object(&json_ctx, "main");                 //access "main" object
    json_obj_get_float(&json_ctx, "temp", &temp);           //parse "temp" primitive
    json_obj_get_int(&json_ctx, "humidity", &humidity);     //parse "humidity" primitive
    json_obj_leave_object(&json_ctx);                       //return to root object
    temp -= 273.15;                                         //convert to Celsius

    //find "name" key
    json_tok_t *token = json_ctx.tokens;
    int i;
    for (i = 0; i < json_ctx.num_tokens; i++) {
        if (token->type != JSMN_STRING) {
            token ++;
            continue;
        }
        if (!strncmp((jstr + token->start), "name", 4))
            break;
        token++;
    }

    if (i < json_ctx.num_tokens) {
        //get its value
        token++;
        printf("| %-8s | %-11s | %-8s |\n", "Cidade", "Temperatura", "Umidade");
        printf("| %-8.*s | %-11.2f | %-8d |\n", (token->end - token->start),
               (jstr + token->start), temp, humidity);
    }

    json_parse_end(&json_ctx);
    free(jstr);
}

