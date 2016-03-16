#include "flicker.h"

#ifdef NRF51
#include "nrf_rng.h"
#endif

#ifndef MIN
#define MIN(a, b) ((a) < (b) ? (a) : (b))
#endif

#ifndef MAX
#define MAX(a, b) ((a) > (b) ? (a) : (b))
#endif

uint32_t flicker_cycle_no = 0;
uint32_t flicker_step_no = 0;
uint32_t last_flicker_end = 0;
uint32_t next_flicker_start = 0;
uint32_t flicker_steps = 0;
bool is_flickering = false;
bool random_seed = false;

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
};

uint8_t flicker_rand(void)
{
#ifdef NRF51
    return (uint8_t)NRF_RNG->VALUE;
#else
    if (random_seed == false)
    {
        srand(time(NULL));
        random_seed = true;
    }
    return rand() % 255;
#endif
}

uint32_t get_flicker_steps(void)
{
    return flicker_steps;
}

uint8_t* get_flicker_table(void)
{
    return flicker_table;
}

void generate_flicker(void)
{
    flicker_steps = (flicker_rand() % 100) + 100;
    uint8_t begin_val = 156;
    uint8_t middle_val = (flicker_rand() % 80) + 30;
    uint8_t end_val = 156;
    uint32_t center = flicker_steps / 2;

    uint8_t up_steps = 0;
    uint8_t down_steps =  0;

    uint32_t new_val = 0xff;

    up_steps = (up_steps == 0) ? 1 : up_steps;
    down_steps = (down_steps == 0) ? 1 : down_steps;

    for (uint32_t i = 0; i < flicker_steps; ++i)
    {
        if (i <= center)
        {
            new_val = begin_val - (i * (MAX(begin_val, middle_val) - MIN(begin_val, middle_val)) / center );
        }
        else
        {
            new_val = middle_val + ((i - center) * (MAX(middle_val, end_val) - MIN(middle_val, end_val)) / center);
        }

        if (new_val < 100)
        {
            new_val = 100;
        }

        if (new_val > 255)
        {
            new_val = 156;
        }

        flicker_table[i] = (uint8_t)new_val;
    }
}

bool should_start_flickering()
{
    if (next_flicker_start == 0)
    {
        uint8_t random_no = flicker_rand() % 200;
        uint8_t random_multiplier = flicker_rand() % 7;
        generate_flicker();

        next_flicker_start = flicker_cycle_no + (random_no * random_multiplier);
    }

    if (flicker_cycle_no >= next_flicker_start)
    {
        return true;
    }

    return false;
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
