#ifndef BATTERY_H
#define BATTERY_H

#include <stdint.h>

#define BATTERY_TASK_PERIOD (5000 / portTICK_PERIOD_MS)

void battery_init(void);
void battery_set_level(uint8_t level);
uint8_t battery_get_level(void);
void battery_update(void);


#endif // BATTERY_H