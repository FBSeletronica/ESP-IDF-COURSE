
#include <stdio.h>
#include "esp_log.h"

static const char *TAG = "Programa Logging";

void app_main(void)
{
   ESP_LOGI(TAG, "Inicio do Programa\n");

   printf("\n LOG Level para Default = INFO\n");
   ESP_LOGE(TAG, "Erro...");   // Será exibido
   ESP_LOGW(TAG, "Warning");   // Será exibido
   ESP_LOGI(TAG, "Informação");// Será exibido
   ESP_LOGD(TAG, "Debug");     // Não será exibido
   ESP_LOGV(TAG, "Verbose");   // Não será exibido

   printf("\n LOG Level para Erro\n");
   esp_log_level_set(TAG, ESP_LOG_ERROR);

   ESP_LOGE(TAG, "Erro...");   // Será exibido
   ESP_LOGW(TAG, "Warning");   // Não será exibido
   ESP_LOGI(TAG, "Informação");// Não será exibido
   ESP_LOGD(TAG, "Debug");     // Não será exibido
   ESP_LOGV(TAG, "Verbose");   // Não será exibido
  
   printf("\n LOG Level para Warning\n");
   esp_log_level_set(TAG, ESP_LOG_WARN);

   ESP_LOGE(TAG, "Erro...");   // Será exibido
   ESP_LOGW(TAG, "Warning");   // Será exibido
   ESP_LOGI(TAG, "Informação");// Não será exibido
   ESP_LOGD(TAG, "Debug");     // Não será exibido
   ESP_LOGV(TAG, "Verbose");   // Não será exibido

   printf("\n LOG Level para Info\n");
   esp_log_level_set(TAG, ESP_LOG_INFO);

   ESP_LOGE(TAG, "Erro...");   // Será exibido
   ESP_LOGW(TAG, "Warning");   // Será exibido
   ESP_LOGI(TAG, "Informação");// Será exibido
   ESP_LOGD(TAG, "Debug");     // Não será exibido
   ESP_LOGV(TAG, "Verbose");   // Não será exibido

   printf("\n LOG Level para Debug\n");
   esp_log_level_set(TAG, ESP_LOG_DEBUG);

   ESP_LOGE(TAG, "Erro...");   // Será exibido
   ESP_LOGW(TAG, "Warning");   // Será exibido
   ESP_LOGI(TAG, "Informação");// Será exibido
   ESP_LOGD(TAG, "Debug");     // Será exibido
   ESP_LOGV(TAG, "Verbose");   // Não será exibido

   printf("\n LOG Level para Verbose\n");
   esp_log_level_set(TAG, ESP_LOG_VERBOSE);

   ESP_LOGE(TAG, "Erro...");   // Será exibido
   ESP_LOGW(TAG, "Warning");   // Será exibido
   ESP_LOGI(TAG, "Informação");// Será exibido
   ESP_LOGD(TAG, "Debug");     // Será exibido
   ESP_LOGV(TAG, "Verbose");   // Será exibido

   printf("\n LOG Level para None\n");
   esp_log_level_set(TAG, ESP_LOG_NONE);
  
   ESP_LOGE(TAG, "Erro...");   // Não será exibido
   ESP_LOGW(TAG, "Warning");   // Não será exibido
   ESP_LOGI(TAG, "Informação");// Não será exibido
   ESP_LOGD(TAG, "Debug");     // Não será exibido
   ESP_LOGV(TAG, "Verbose");   // Não será exibido              
}

