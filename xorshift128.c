#include "xorshift128.h"

/* These state variables must be initialized so that they are not all zero. */
uint32_t XORSHIFT_X = 0x32fa56cd;
uint32_t XORSHIFT_Y = 0x1f5cade1;
uint32_t XORSHIFT_Z = 0x1059f2c5;
uint32_t XORSHIFT_W = 0xf39d0fa9;

uint32_t xorshift128(void) {
    uint32_t t = XORSHIFT_X;
    t ^= t << 11;
    t ^= t >> 8;
    XORSHIFT_X = XORSHIFT_Y;
    XORSHIFT_Y = XORSHIFT_Z;
    XORSHIFT_Z = XORSHIFT_W;
    XORSHIFT_W ^= XORSHIFT_W >> 19;
    XORSHIFT_W ^= t;
    return XORSHIFT_W;
}