#include "battery.h"

static uint8_t battery_level = 100;

void battery_init(void){
    // initialize battery level to 100%
    battery_level = 100;
}


void battery_set_level(uint8_t level){
    if (level > 100) {
        level = 100;
    }
    battery_level = level;
}


uint8_t battery_get_level(void){
    return battery_level;
}

void battery_update(void){
    // Decrease battery level by 1% every update cycle
    if (battery_level > 0) {
        battery_level--;
    }else{
        battery_level = 100; // Reset to 100% when it reaches 0%
    }
}