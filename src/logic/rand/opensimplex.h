#ifndef OPENSIMPLEX2S_H
#define OPENSIMPLEX2S_H

#include <math.h>


#define PRIME_X         (0x5205402B9270C86FL)
#define PRIME_Y         (0x598CD327003817B5L)
#define HASH_MULTIPLIER (0x53A3F72DEEC546F5L)

#define ROOT2OVER2 (0.7071067811865476)
#define SKEW_2D    (0.366025403784439)
#define UNSKEW_2D  (-0.21132486540518713)

#define N_GRADS_2D_EXPONENT (7)
#define N_GRADS_2D          (1 << N_GRADS_2D_EXPONENT)

#define NORMALIZER_2D (0.05481866495625118)

#define RSQUARED_2D   (2.0f / 3.0f)

/// @brief Get the standard noise using the at position x and y with seed seed
/// @param seed 
/// @param x 
/// @param y 
/// @return noise at x, y with seed seed
float OpenSimplex_2DNoise(long seed, double x, double y);

float OpenSimplex_2DNoise_ImprovedX(long seed, double x, double y);

#endif