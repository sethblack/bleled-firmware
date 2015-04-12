#include "flicker.h"
#include "nordic_common.h"
#include "nrf_rng.h"

uint32_t flicker_cycle_no = 0;
uint32_t flicker_step_no = 0;
uint32_t last_flicker_end = 0;
uint32_t next_flicker_start = 0;
uint32_t flicker_steps = 0;
bool is_flickering = false;

static uint8_t flicker_table[] = {
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff
};

uint8_t flicker_rand(void)
{
    return (uint8_t)NRF_RNG->VALUE;
}

uint8_t cycle_flicker(void)
{
    uint8_t new_flicker_val;
    ++flicker_cycle_no;

    if (flicker_step_no >= flicker_steps)
    {
        is_flickering = false;
        flicker_step_no = 0;
        last_flicker_end = flicker_cycle_no;
        next_flicker_start = 0;
    }

    if (is_flickering == true)
    {
        new_flicker_val = flicker_table[flicker_step_no++];
    }
    else
    {
        is_flickering = should_start_flickering();
        new_flicker_val = 237;
    }

    return new_flicker_val;
}

bool should_start_flickering()
{
    if (next_flicker_start == 0)
    {
        uint8_t random_no = flicker_rand();
        uint8_t random_multiplier = flicker_rand() % 8;
        generate_flicker();

        next_flicker_start = flicker_cycle_no + (random_no * random_multiplier);
    }

    if (flicker_cycle_no >= next_flicker_start)
    {
        return true;
    }

    return false;
}

void generate_flicker(void)
{
    flicker_steps = flicker_rand() + 100;
    uint8_t begin_val = 238; //(flicker_rand() % 156) + 100;
    uint8_t middle_val = flicker_rand() % 156 + 30;
    uint8_t end_val = 242; //(flicker_rand() % 156) + 100;
    uint32_t center = flicker_steps / 2;

    uint8_t up_steps = (MAX(begin_val, middle_val) - MIN(begin_val, middle_val)) / center;
    uint8_t down_steps =  (MAX(middle_val, end_val) - MIN(middle_val, end_val)) / center;

    for (uint32_t i = 0; i < flicker_steps; ++i)
    {
        if (i < center)
        {
           flicker_table[i] = begin_val + (i * up_steps);
        }
        else if (i == center)
        {
            flicker_table[i] = middle_val;
        }
        else
        {
            flicker_table[i] = middle_val - (i * down_steps);
        }
    }
}