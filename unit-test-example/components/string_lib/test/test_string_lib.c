/******************************************************************************
* test_string_lib.c: Implementation of unit test for string_lib component
*
* This example shows how to create a unit test for a component using esp-idf unity.
* This code is part of the course "Academia ESP32 Profissional" by Fábio Souza
* The course is available on https://cursos.embarcados.com.br
*
* This example code Creative Commons Attribution 4.0 International License.
* When using the code, you must keep the above copyright notice,
* this list of conditions and the following disclaimer in the source code.
* (http://creativecommons.org/licenses/by/4.0/)

* Author: Vinicius Silva <silva.viniciusr@gmail.com>
* This code is for teaching purposes only.
* No warranty of any kind is provided.
*******************************************************************************/


#include "unity.h"
#include "string_lib.h"

TEST_CASE("Test string_Upper", "[string_lib]")
{
    char test_str[] = "abcdefghijklmnopqrstuvwxyz0123456789";
    string_Upper(test_str);
    TEST_ASSERT_EQUAL_STRING("ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789", test_str);
}

TEST_CASE("Test string_Lower", "[string_lib]")
{
    char test_str[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
    string_Lower(test_str);
    TEST_ASSERT_EQUAL_STRING("abcdefghijklmnopqrstuvwxyz0123456789", test_str);
}

TEST_CASE("Test string_Mirror", "[string_lib]")
{
    char test_str1[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
    char test_str2[] = "9876543210zyxwvutsrqponmlkjihgfedcba";
    string_Mirror(test_str1);
    string_Mirror(test_str2);
    TEST_ASSERT_EQUAL_STRING("9876543210ZYXWVUTSRQPONMLKJIHGFEDCBA", test_str1);
    TEST_ASSERT_EQUAL_STRING("abcdefghijklmnopqrstuvwxyz0123456789", test_str2);
}
