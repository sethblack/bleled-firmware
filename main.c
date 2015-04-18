#include <stdbool.h>
#include <stdint.h>
#include "nrf.h"
#include "nrf_delay.h"
#include "nrf_pwm.h"
#include "flicker.h"

#define FLAME_LED       22

int main(void)
{
    nrf_pwm_init(FLAME_LED);

    while (true)
    {
        nrf_pwm_set_value(cycle_flicker());
        nrf_delay_us(2000);
    }
}
