#ifndef FEATURE_H
#define FEATURE_H

#include <math.h>
#include <stdlib.h>

#include "gameobject.h"

typedef int (*FeatureProviderFunction) (gameobject_t** destination, void* user_data);

typedef struct Feature {
    int (*provider) (gameobject_t** destination, void* user_data);
    void* user_data;
    double max_probability;    // The modified probability shall never exceed this, if the distances are kept
    double min_probability;    // The modified probability shall never subceed this, if the distances are kept
    double constant_factor;    // factor for the ... part of the probability function f(x) where x is the distance to the next feature
    double linear_factor;      
    double quadratic_factor;
    double cubic_factor;
    size_t foreign_distance;   // minimum distance of this feature to other features
    size_t minimum_distance;   // minimum distance between two of these features
} feature_t;

int Feature_CalculateProbability(feature_t* feature, size_t self_distance, size_t foreign_distance, double* destination);

#endif //FEATURE_H