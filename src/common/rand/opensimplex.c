#include "opensimplex.h"

float GRADIENTS_2D[] = {6.9808965, 16.853374, 16.853374, 6.9808965, 16.853374, -6.9808965, 6.9808965, -16.853374, -6.9808965, -16.853374, -16.853374, -6.9808965, -16.853374, 6.9808965, -6.9808965, 16.853374, 2.3810537, 18.0859, 11.105002, 14.4723215, 14.4723215, 11.105002, 18.0859, 2.3810537, 18.0859, -2.3810537, 14.4723215, -11.105002, 11.105002, -14.4723215, 2.3810537, -18.0859, -2.3810537, -18.0859, -11.105002, -14.4723215, -14.4723215, -11.105002, -18.0859, -2.3810537, -18.0859, 2.3810537, -14.4723215, 11.105002, -11.105002, 14.4723215, -2.3810537, 18.0859, 6.9808965, 16.853374, 16.853374, 6.9808965, 16.853374, -6.9808965, 6.9808965, -16.853374, -6.9808965, -16.853374, -16.853374, -6.9808965, -16.853374, 6.9808965, -6.9808965, 16.853374, 2.3810537, 18.0859, 11.105002, 14.4723215, 14.4723215, 11.105002, 18.0859, 2.3810537, 18.0859, -2.3810537, 14.4723215, -11.105002, 11.105002, -14.4723215, 2.3810537, -18.0859, -2.3810537, -18.0859, -11.105002, -14.4723215, -14.4723215, -11.105002, -18.0859, -2.3810537, -18.0859, 2.3810537, -14.4723215, 11.105002, -11.105002, 14.4723215, -2.3810537, 18.0859, 6.9808965, 16.853374, 16.853374, 6.9808965, 16.853374, -6.9808965, 6.9808965, -16.853374, -6.9808965, -16.853374, -16.853374, -6.9808965, -16.853374, 6.9808965, -6.9808965, 16.853374, 2.3810537, 18.0859, 11.105002, 14.4723215, 14.4723215, 11.105002, 18.0859, 2.3810537, 18.0859, -2.3810537, 14.4723215, -11.105002, 11.105002, -14.4723215, 2.3810537, -18.0859, -2.3810537, -18.0859, -11.105002, -14.4723215, -14.4723215, -11.105002, -18.0859, -2.3810537, -18.0859, 2.3810537, -14.4723215, 11.105002, -11.105002, 14.4723215, -2.3810537, 18.0859, 6.9808965, 16.853374, 16.853374, 6.9808965, 16.853374, -6.9808965, 6.9808965, -16.853374, -6.9808965, -16.853374, -16.853374, -6.9808965, -16.853374, 6.9808965, -6.9808965, 16.853374, 2.3810537, 18.0859, 11.105002, 14.4723215, 14.4723215, 11.105002, 18.0859, 2.3810537, 18.0859, -2.3810537, 14.4723215, -11.105002, 11.105002, -14.4723215, 2.3810537, -18.0859, -2.3810537, -18.0859, -11.105002, -14.4723215, -14.4723215, -11.105002, -18.0859, -2.3810537, -18.0859, 2.3810537, -14.4723215, 11.105002, -11.105002, 14.4723215, -2.3810537, 18.0859, 6.9808965, 16.853374, 16.853374, 6.9808965, 16.853374, -6.9808965, 6.9808965, -16.853374, -6.9808965, -16.853374, -16.853374, -6.9808965, -16.853374, 6.9808965, -6.9808965, 16.853374, 2.3810537, 18.0859, 11.105002, 14.4723215, 14.4723215, 11.105002, 18.0859, 2.3810537, 18.0859, -2.3810537, 14.4723215, -11.105002, 11.105002, -14.4723215, 2.3810537, -18.0859, -2.3810537, -18.0859, -11.105002, -14.4723215, -14.4723215, -11.105002, -18.0859, -2.3810537, -18.0859, 2.3810537, -14.4723215, 11.105002, -11.105002, 14.4723215, -2.3810537, 18.0859, 6.9808965, 16.853374, 16.853374, 6.9808965, 16.853374, -6.9808965, 6.9808965, -16.853374, -6.9808965, -16.853374, -16.853374, -6.9808965, -16.853374, 6.9808965, -6.9808965, 16.853374};

int OpenSimplex_Floor(double n)
{
    return (int) floor(n);
}

float OpenSimplex_2DGrad(int64_t seed, int64_t xsvp, int64_t ysvp, float dx, float dy) {
    int64_t hash = seed ^ xsvp ^ ysvp;
    hash *= HASH_MULTIPLIER;
    hash ^= hash >> (64 - N_GRADS_2D_EXPONENT + 1);
    int gi = (int)hash & ((N_GRADS_2D - 1) << 1);
    return GRADIENTS_2D[gi | 0] * dx + GRADIENTS_2D[gi | 1] * dy;
}

/**
 * 2D  OpenSimplex2S/SuperSimplex noise base.
 */
float OpenSimplex_2DNoise_UnskewedBase(int64_t seed, double xs, double ys) {

    // Get base points and offsets.
    int xsb = OpenSimplex_Floor(xs), ysb = OpenSimplex_Floor(ys);
    float xi = (float)(xs - xsb), yi = (float)(ys - ysb);

    // Prime pre-multiplication for hash.
    int64_t xsbp = xsb * PRIME_X, ysbp = ysb * PRIME_Y;

    // Unskew.
    float t = (xi + yi) * (float)UNSKEW_2D;
    float dx0 = xi + t, dy0 = yi + t;

    // First vertex.
    float a0 = RSQUARED_2D - dx0 * dx0 - dy0 * dy0;
    float value = (a0 * a0) * (a0 * a0) * OpenSimplex_2DGrad(seed, xsbp, ysbp, dx0, dy0);

    // Second vertex.
    float a1 = (float)(2 * (1 + 2 * UNSKEW_2D) * (1 / UNSKEW_2D + 2)) * t + ((float)(-2 * (1 + 2 * UNSKEW_2D) * (1 + 2 * UNSKEW_2D)) + a0);
    float dx1 = dx0 - (float)(1 + 2 * UNSKEW_2D);
    float dy1 = dy0 - (float)(1 + 2 * UNSKEW_2D);
    value += (a1 * a1) * (a1 * a1) * OpenSimplex_2DGrad(seed, xsbp + PRIME_X, ysbp + PRIME_Y, dx1, dy1);

    // Third and fourth vertices.
    // Nested conditionals were faster than compact bit logic/arithmetic.
    float xmyi = xi - yi;
    if (t < UNSKEW_2D) {
        if (xi + xmyi > 1) {
            float dx2 = dx0 - (float)(3 * UNSKEW_2D + 2);
            float dy2 = dy0 - (float)(3 * UNSKEW_2D + 1);
            float a2 = RSQUARED_2D - dx2 * dx2 - dy2 * dy2;
            if (a2 > 0) {
                value += (a2 * a2) * (a2 * a2) * OpenSimplex_2DGrad(seed, xsbp + (PRIME_X << 1), ysbp + PRIME_Y, dx2, dy2);
            }
        }
        else
        {
            float dx2 = dx0 - (float)UNSKEW_2D;
            float dy2 = dy0 - (float)(UNSKEW_2D + 1);
            float a2 = RSQUARED_2D - dx2 * dx2 - dy2 * dy2;
            if (a2 > 0) {
                value += (a2 * a2) * (a2 * a2) * OpenSimplex_2DGrad(seed, xsbp, ysbp + PRIME_Y, dx2, dy2);
            }
        }

        if (yi - xmyi > 1) {
            float dx3 = dx0 - (float)(3 * UNSKEW_2D + 1);
            float dy3 = dy0 - (float)(3 * UNSKEW_2D + 2);
            float a3 = RSQUARED_2D - dx3 * dx3 - dy3 * dy3;
            if (a3 > 0) {
                value += (a3 * a3) * (a3 * a3) * OpenSimplex_2DGrad(seed, xsbp + PRIME_X, ysbp + (PRIME_Y << 1), dx3, dy3);
            }
        }
        else
        {
            float dx3 = dx0 - (float)(UNSKEW_2D + 1);
            float dy3 = dy0 - (float)UNSKEW_2D;
            float a3 = RSQUARED_2D - dx3 * dx3 - dy3 * dy3;
            if (a3 > 0) {
                value += (a3 * a3) * (a3 * a3) * OpenSimplex_2DGrad(seed, xsbp + PRIME_X, ysbp, dx3, dy3);
            }
        }
    }
    else
    {
        if (xi + xmyi < 0) {
            float dx2 = dx0 + (float)(1 + UNSKEW_2D);
            float dy2 = dy0 + (float)UNSKEW_2D;
            float a2 = RSQUARED_2D - dx2 * dx2 - dy2 * dy2;
            if (a2 > 0) {
                value += (a2 * a2) * (a2 * a2) * OpenSimplex_2DGrad(seed, xsbp - PRIME_X, ysbp, dx2, dy2);
            }
        }
        else
        {
            float dx2 = dx0 - (float)(UNSKEW_2D + 1);
            float dy2 = dy0 - (float)UNSKEW_2D;
            float a2 = RSQUARED_2D - dx2 * dx2 - dy2 * dy2;
            if (a2 > 0) {
                value += (a2 * a2) * (a2 * a2) * OpenSimplex_2DGrad(seed, xsbp + PRIME_X, ysbp, dx2, dy2);
            }
        }

        if (yi < xmyi) {
            float dx2 = dx0 + (float)UNSKEW_2D;
            float dy2 = dy0 + (float)(UNSKEW_2D + 1);
            float a2 = RSQUARED_2D - dx2 * dx2 - dy2 * dy2;
            if (a2 > 0) {
                value += (a2 * a2) * (a2 * a2) * OpenSimplex_2DGrad(seed, xsbp, ysbp - PRIME_Y, dx2, dy2);
            }
        }
        else
        {
            float dx2 = dx0 - (float)UNSKEW_2D;
            float dy2 = dy0 - (float)(UNSKEW_2D + 1);
            float a2 = RSQUARED_2D - dx2 * dx2 - dy2 * dy2;
            if (a2 > 0) {
                value += (a2 * a2) * (a2 * a2) * OpenSimplex_2DGrad(seed, xsbp, ysbp + PRIME_Y, dx2, dy2);
            }
        }
    }

    return value;
}

/*
    * Noise Evaluators
    */

/**
 * 2D OpenSimplex2S/SuperSimplex noise, standard lattice orientation.
 */
float OpenSimplex_2DNoise(int64_t seed, double x, double y) {

    // Get points for A2* lattice
    double s = SKEW_2D * (x + y);
    double xs = x + s, ys = y + s;

    return OpenSimplex_2DNoise_UnskewedBase(seed, xs, ys);
}

/**
 * 2D OpenSimplex2S/SuperSimplex noise, with Y pointing down the main diagonal.
 * Might be better for a 2D sandbox style game, where Y is vertical.
 * Probably slightly less optimal for heightmaps or continent maps,
 * unless your map is centered around an equator. It's a slight
 * difference, but the option is here to make it easy.
 */
float OpenSimplex_2DNoise_ImprovedX(int64_t seed, double x, double y) {

    // Skew transform and rotation baked into one.
    double xx = x * ROOT2OVER2;
    double yy = y * (ROOT2OVER2 * (1 + 2 * SKEW_2D));

    return OpenSimplex_2DNoise_UnskewedBase(seed, yy + xx, yy - xx);
}