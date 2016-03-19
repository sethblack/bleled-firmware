#include "xorshift128.h"

uint32_t xorshift32() {
    static uint32_t y = 2463534242UL;
    y ^= (y << 13);
    y ^= (y >> 17);
    return (y ^= (y << 15));
}

uint32_t xorshift64() {
    static uint32_t x = 123456789, y = 362436069;
    uint32_t t = (x^(x<<10));
    x = y;
    return y = (y ^ (y >> 10)) ^ (t ^ (t >> 13));
}

uint32_t xorshift128(void) {
    static uint32_t x = 123456789, y = 362436069, z = 521288629,
                    w = 88675123;

    uint32_t t = (x^(x<<11)); x = y; y = z; z = w; return w = (w^(w>>19))^(t^(t>>8));
}