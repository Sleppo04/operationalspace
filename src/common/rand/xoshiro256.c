#include "xoshiro256.h"

uint64_t rotate_uint64_left(uint64_t bytes, int degree)
{
    return (bytes << degree) | (bytes >> (64 - degree));
}


uint64_t xoshiro256_next(xoshiro256_state_t* state) {
    uint64_t* uint64_state = state->state;
	const uint64_t result = rotate_uint64_left(uint64_state[0] + uint64_state[3], 23) + uint64_state[0];

	const uint64_t t = uint64_state[1] << 17;

	uint64_state[2] ^= uint64_state[0];
	uint64_state[3] ^= uint64_state[1];
	uint64_state[1] ^= uint64_state[2];
	uint64_state[0] ^= uint64_state[3];

	uint64_state[2] ^= t;

	uint64_state[3] = rotate_uint64_left(uint64_state[3], 45);

	return result;
}


/* This is the jump function for the generator. It is equivalent
   to 2^128 calls to next(); it can be used to generate 2^128
   non-overlapping subsequences for parallel computations. */

void jump(xoshiro256_state_t* state) {
    uint64_t* uint64_state = state->state;
	static const uint64_t JUMP[] = { 0x180ec6d33cfd0aba, 0xd5a61266f0c9392c, 0xa9582618e03fc9aa, 0x39abdc4529b1661c };

	uint64_t s0 = 0;
	uint64_t s1 = 0;
	uint64_t s2 = 0;
	uint64_t s3 = 0;
	for(uint64_t i = 0; i < sizeof JUMP / sizeof *JUMP; i++)
		for(int b = 0; b < 64; b++) {
			if (JUMP[i] & UINT64_C(1) << b) {
				s0 ^= uint64_state[0];
				s1 ^= uint64_state[1];
				s2 ^= uint64_state[2];
				s3 ^= uint64_state[3];
			}
			xoshiro256_next(state);	
		}
		
	uint64_state[0] = s0;
	uint64_state[1] = s1;
	uint64_state[2] = s2;
	uint64_state[3] = s3;
}



/* This is the long-jump function for the generator. It is equivalent to
   2^192 calls to next(); it can be used to generate 2^64 starting points,
   from each of which jump() will generate 2^64 non-overlapping
   subsequences for parallel distributed computations. */

void long_jump(xoshiro256_state_t* state) {
    uint64_t* uint64_state = state->state;
	static const uint64_t LONG_JUMP[] = { 0x76e15d3efefdcbbf, 0xc5004e441c522fb3, 0x77710069854ee241, 0x39109bb02acbe635 };

	uint64_t s0 = 0;
	uint64_t s1 = 0;
	uint64_t s2 = 0;
	uint64_t s3 = 0;
	for(uint64_t i = 0; i < sizeof LONG_JUMP / sizeof *LONG_JUMP; i++)
		for(int b = 0; b < 64; b++) {
			if (LONG_JUMP[i] & UINT64_C(1) << b) {
				s0 ^= uint64_state[0];
				s1 ^= uint64_state[1];
				s2 ^= uint64_state[2];
				s3 ^= uint64_state[3];
			}
			xoshiro256_next(state);	
		}
		
	uint64_state[0] = s0;
	uint64_state[1] = s1;
	uint64_state[2] = s2;
	uint64_state[3] = s3;
}