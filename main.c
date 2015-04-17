#include <stdint.h>
#include <stdbool.h>

#include "nrf_delay.h"
#include "nrf_pwm.h"

#include "flicker.h"

#define BLE_CANDLE_PIN_LED                    1

void bsp_indication_set(int x) {return;}

void init_pwm(void)
{
    uint32_t counter = 0;
    
    nrf_pwm_config_t pwm_config = PWM_DEFAULT_CONFIG;
    
    pwm_config.mode             = PWM_MODE_LED_255;
    pwm_config.num_channels     = 1;
    pwm_config.gpio_num[0]      = BLE_CANDLE_PIN_LED;  
    
    nrf_pwm_init(&pwm_config);

    NRF_RNG->TASKS_START = 1;

    NRF_RNG->EVENTS_VALRDY = 0;
    while (NRF_RNG->EVENTS_VALRDY == 0){}
}

void main(void)
{
    init_pwm();

    while (true)
    {
        nrf_pwm_set_value(0, cycle_flicker());
        nrf_delay_us(2000);
    }
}
