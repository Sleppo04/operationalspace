#ifndef XOSHIRO256_H
#define XOSHIRO256_H

#include <stdint.h>

typedef struct XoshiroState {
    uint64_t state[4];
} xoshiro256_state_t;

uint64_t rotate_uint64_left(uint64_t bytes, int degree);

uint64_t xoshiro256_next(xoshiro256_state_t *state);

#endif