#ifndef UBCTYPES_H
#define UBCTYPES_H

#include <stdint.h>

typedef struct UbcFunction {
    uint16_t constant_count;
    uint16_t variable_count;
    char* bytecode;
    uintptr_t bytecode_length;
} ubcfunction_t;

#endif //UBCTYPES