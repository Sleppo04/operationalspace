#ifndef FEATURE_H
#define FEATURE_H

#include <stdlib.h>

#include "gameobject.h"

typedef int (*FeatureProviderFunction) (gameobject_t** destination, void* user_data);

typedef struct Feature {
    int (*provider) (gameobject_t** destination, void* user_data);
    void* user_data;
    double max_probability;    // The modified probability shall never exceed this, if the distances are kept
    double min_probability;    // The modified probability shall never subceed this, if the distances are kept
    double base_probability;   // Probability to place this feature on one tile
    double probability_mod;    // this will be added to the base probability for every tile distance higher then minimum_distance
    double probability_growth;  // the probability will be multiplied by this for each tile of distance
    size_t foreign_distance;   // minimum distance of this feature to other features
    size_t minimum_distance;   // minimum distance between two of these features
} feature_t;

#endif //FEATURE_H