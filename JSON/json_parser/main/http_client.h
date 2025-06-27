#pragma once

#include "esp_err.h"

esp_err_t http_client_request(const char *url, char *response, const int len);

