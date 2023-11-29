#ifndef HTTP_CLIENT_H
#define HTTP_CLIENT_H

void http_client_request();  // Initialize the wifi station
void http_client_task(void *pvParameters);   // Task to be run on the core 0

#endif // HTTP_CLIENT_H
