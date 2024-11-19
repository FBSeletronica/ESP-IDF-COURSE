#include "lib.h"
#include <esp_log.h>

static const char *TAG = "LIB";

void foo(void)
{
    ESP_LOGI(TAG,"LOG INFO");
}
void bar(void)
{
    ESP_LOGI(TAG,"LOG INFO");
}