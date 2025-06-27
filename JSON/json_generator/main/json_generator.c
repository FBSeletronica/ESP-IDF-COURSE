/******************************************************************************
* JSON Generator
* This example shows how to create a json string using espressif/json_generator
* component.
* https://components.espressif.com/components/espressif/json_generator/versions/1.1.2
*
* This code is part of the course "Aprenda programar o ESP32 com ESP-IDF" by Fábio Souza
* The course is available on https://cursos.embarcados.com.br
*
* This example code Creative Commons Attribution 4.0 International License.
* When using the code, you must keep the above copyright notice,
* this list of conditions and the following disclaimer in the source code.
* (http://creativecommons.org/licenses/by/4.0/)
*
* Author: Vinícius Silva <silva.viniciusr@gmail.com>
* This code is for teaching purposes only.
* No warranty of any kind is provided.
*******************************************************************************/

#include <stdio.h>
#include "esp_err.h"
#include "json_generator.h"

#define BUFFER_SIZE (1024)

void app_main(void)
{
    /* some data to encode */
    int solution = 42;
    char mojo[] = "I program with esp-idf!!";
    float f1 = 3.14, f2 = 6.5821, f3 = -16;
    bool led_1 = true;

    /* buffer to hold json string */
    char jstr[BUFFER_SIZE] = {0};

    /* json context to use with the lib */
    json_gen_str_t json_ctx;

    /* Lib initialization */
    json_gen_str_start(&json_ctx, jstr, BUFFER_SIZE, NULL, NULL);

    ESP_ERROR_CHECK(json_gen_start_object(&json_ctx));                    //add {
    ESP_ERROR_CHECK(json_gen_obj_set_int(&json_ctx, "solution", solution));   //add "solution" : "42"
    ESP_ERROR_CHECK(json_gen_obj_set_string(&json_ctx, "mojo", mojo));        //add "mojo" : "I program with esp-idf!!"
    ESP_ERROR_CHECK(json_gen_push_array(&json_ctx, "constants"));         //add "constants" : [
    ESP_ERROR_CHECK(json_gen_arr_set_float(&json_ctx, f1));               //add "3.14"
    ESP_ERROR_CHECK(json_gen_arr_set_float(&json_ctx, f2));               //add "6.5821"
    ESP_ERROR_CHECK(json_gen_arr_set_float(&json_ctx, f3));               //add "-16.0"
    ESP_ERROR_CHECK(json_gen_pop_array(&json_ctx));                       //add ]
    ESP_ERROR_CHECK(json_gen_obj_set_bool(&json_ctx, "led_1", led_1));    //add "led_1" : "true"
    ESP_ERROR_CHECK(json_gen_end_object(&json_ctx));                      //add }

    /* Clear json_ctx */
    ESP_ERROR_CHECK(json_gen_end_object(&json_ctx));
    printf("My JSON string:\n%s\n", jstr);
}
