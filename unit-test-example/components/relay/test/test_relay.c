/******************************************************************************
* test_relay.c: Implementation of unit test for relay.c component
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
#include "relay.h"

/* Local variables for component testing */
static Relay test_relay;
static int pin = 21;

TEST_CASE("Test relay_init", "[relay]")
{
    relay_init(&test_relay, pin);
    TEST_ASSERT_EQUAL(pin, test_relay.pin);
    TEST_ASSERT_EQUAL(0, test_relay.state);
}

TEST_CASE("Test relay_turn_on", "[relay]")
{
    relay_turn_on(&test_relay);
    TEST_ASSERT_EQUAL(1, test_relay.state);
}

TEST_CASE("Test relay_turn_off", "[relay]")
{
    relay_turn_off(&test_relay);
    TEST_ASSERT_EQUAL(0, test_relay.state);
}

TEST_CASE("Test relay_get_status", "[relay]")
{
    relay_turn_on(&test_relay);
    TEST_ASSERT_EQUAL(1, relay_get_status(&test_relay));

    relay_turn_off(&test_relay);
    TEST_ASSERT_EQUAL(0, relay_get_status(&test_relay));
}
