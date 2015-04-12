#ifndef __FLICKER_H__
#define __FLICKER_H__

#include <stdint.h>
#include <stdbool.h>

uint8_t cycle_flicker(void);
uint8_t determine_current_flicker_val();
bool should_start_flickering();
uint8_t flicker_rand(void);
void generate_flicker(void);

#endif