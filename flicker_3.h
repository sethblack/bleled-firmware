#ifndef __FLICKER_3_H__
#define __FLICKER_3_H__

#include <stdint.h>

#define TOTAL_CYCLES_PER_FLICKER 316

extern uint8_t FLICKER_BRIGHTNESS;
extern uint8_t FLICKER_SPEED;

uint8_t cycle_flicker_3(uint8_t max_brightness);

#endif