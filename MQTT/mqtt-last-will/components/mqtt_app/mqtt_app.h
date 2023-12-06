#ifndef MQTT_APP_H
#define MQTT_APP_H

void mqtt_app_start(void);
void mqtt_app_subscribe(char *topic,int qos);
void mqtt_app_unsubscribe(char *topic);
void mqtt_app_publish(char *topic, char *payload, int qos, int retain);
int mqtt_is_connected(void);
void mqtt_app_stop(void);

#endif