#include "feature.h"

int Feature_CalculateProbability(feature_t* feature, size_t self_distance, size_t foreign_distance, double* destination)
{
    if (feature == NULL) {
        return EINVAL;
    }
    if (destination == NULL) {
        return EDESTADDRREQ;
    }

    if (self_distance < feature->minimum_distance) {
        destination[0] = 0.0;
        return EXIT_SUCCESS;
    }
    if (foreign_distance < feature->foreign_distance) {
        destination[0] = 0.0;
        return EXIT_SUCCESS;
    }

    double probability = feature->constant_factor;
    probability += self_distance * feature->linear_factor;
    probability += pow(self_distance, 2) * feature->quadratic_factor;
    probability += pow(self_distance, 3) * feature->cubic_factor;

    probability = fmax(probability, feature->min_probability);
    probability = fmin(probability, feature->max_probability);

    destination[0] = probability;

    return 0;
}
