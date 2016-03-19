#include "flicker_3.h"
#include "xorshift128.h"
#include <cross_studio_io.h>

uint8_t FLICKER_BRIGHTNESS = 0xff;

uint8_t cycle_flicker_3(uint8_t max_brightness) {
    static uint32_t flicker_start = 0, flicker_bottom = 255, flicker_duration = 0,
        cycle_no = TOTAL_CYCLES_PER_FLICKER, half_flicker_duration = 0, cycles_remaining = 0;
    static float flicker_slope = 0.0;

    if (FLICKER_BRIGHTNESS == 0) {
        return 0;
    }

    cycles_remaining = TOTAL_CYCLES_PER_FLICKER - cycle_no;

    if (cycles_remaining == 0) {
        flicker_start = (xorshift128() % 128);
        //flicker_bottom = ((xorshift128() % 100) + 128);
        //flicker_duration = (xorshift128() % 168) + 32;
        flicker_bottom = 128;
        flicker_duration = 128;
        flicker_slope = (float)(255 - flicker_bottom) / (float)(flicker_duration / 2);
        half_flicker_duration = flicker_duration / 2;
        cycle_no = 0;
    }

    uint8_t flicker_val;

    if (cycle_no < flicker_start || cycle_no >= (flicker_duration + flicker_start)) {
        flicker_val = 255;
    } else {
        if (cycle_no < flicker_start + half_flicker_duration) {
            flicker_val = 255 - (int)(flicker_slope * (cycle_no - flicker_start));
        } else {
            flicker_val = flicker_bottom + (int)(flicker_slope * (cycle_no - flicker_start - half_flicker_duration));
        }
    }

    flicker_val = flicker_val * ((float)(max_brightness) / 255);

    if (flicker_val < 60) {
        flicker_val = 60;
    }

    /*debug_printf("start: %d, bottom: %d, duration: %d, slope: %f, cycle: %d, remaining: %d\n",
        flicker_start, flicker_bottom, flicker_duration, flicker_slope,
        cycle_no, cycles_remaining);*/

    ++cycle_no;

    return flicker_val;
}