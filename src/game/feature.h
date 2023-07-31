#ifndef FEATURE_H
#define FEATURE_H

#define NOISE_COUNT 1

#include <math.h>
#include <stdlib.h>

#include "gameobject.h"

typedef int (*FeatureProviderFunction) (gameobject_t** destination, void* user_data);

typedef struct Feature {
    int (*provider) (gameobject_t** destination, void* user_data);
    void* user_data;
    uint16_t minimum_noise_levels[NOISE_COUNT]; // at least 0
    uint16_t maximum_noise_levels[NOISE_COUNT]; // at most 1024
} feature_t;

#endif //FEATURE_H