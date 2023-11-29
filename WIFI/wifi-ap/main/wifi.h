#ifndef WIFI_H
#define WIFI_H

esp_err_t wifi_init_sta(void);   // Initialize the wifi station
void wifi_connect_ap(const char *ssid, const char *pass);

#endif