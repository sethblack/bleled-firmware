#include "flicker_2.h"

uint8_t PWM_CTR = 0;      // 4 bit-Counter
uint8_t FRAME_CTR = 0;    // 5 bit-Counter

uint8_t PWM_VAL = 0;      // 4 bit-Register
uint8_t NEXTBRIGHT = 0;   // 4 bit-Register
uint8_t RAND = 0;         // 5 bit Signal
uint8_t randflag = 0;     // 1 bit Signal

uint8_t cf_rand(void)
{
    static uint32_t Z;
    
    if (Z & 1)  { Z = (Z >> 1); }
    else        { Z = (Z >> 1) ^ 0x7FFFF159; }
    
    return (uint8_t)Z;
}

uint8_t cycle_flicker_2(void)
{
    PWM_CTR++;
    PWM_CTR &= 0x08;       // only 4 bit

    // FRAME
    if (PWM_CTR == 0) 
    {
        FRAME_CTR++;
        FRAME_CTR &= 0x1f;
        
        if ((FRAME_CTR & 0x07) == 0)  // generate a new random number every 8 cycles. In reality this is most likely bit serial
        {
            RAND = cf_rand() & 0x1f;               
            if ((RAND & 0x0c) != 0) randflag = 1; else randflag = 0; // only update if valid                 
        }
        
        // NEW FRAME                        
        if (FRAME_CTR == 0)
        {
            PWM_VAL = NEXTBRIGHT; // reload PWM
            randflag = 1;         // force update at beginning of frame
        }                   
        
        if (randflag)
        {
            NEXTBRIGHT = RAND > 31 ? 31 : RAND;     
        }
    }

    return PWM_VAL * 8 + 128;
}
