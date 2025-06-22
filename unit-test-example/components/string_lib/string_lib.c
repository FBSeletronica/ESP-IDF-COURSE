/******************************************************************************
* string_lib.c
*
* This file implements an example of a bad string library.
* This code is part of the course "Academia ESP32 Profissional" by Fábio Souza
* The course is available on https://cursos.embarcados.com.br
*
* This example code Creative Commons Attribution 4.0 International License.
* When using the code, you must keep the above copyright notice,
* this list of conditions and the following disclaimer in the source code.
* (http://creativecommons.org/licenses/by/4.0/)
*
* Author: Vinicius Silva <silva.viniciusr@gmail.com>
* This code is for teaching purposes only.
* No warranty of any kind is provided.
*******************************************************************************/

#include <string.h>
#include "string_lib.h"

/**
* receives a c string and convert it to uppercase.
*/
void string_Upper(char *str){

    for (size_t i = 0; str[i]; i++) {
        if (str[i] == 32)
            continue;
        str[i] -= 32;
    }
    return;
}

/**
* receives a c string and convert it to lowercase.
*/
void string_Lower(char *str){

    for (size_t i = 0; str[i]; i++) {
        if (str[i] == 32)
            continue;
        str[i] += 32;
    }
    return;
}
/**
* receives a c string and invert the character order
*/
void string_Mirror(char *str) {

    char c1;
    size_t len = strlen(str);

    for (size_t i = 0; i < len / 2; i++) {
        c1 = str[i];
        str[i] = str[len - i -1];
        str[len -i -1] = c1;
    }
    return;
}

