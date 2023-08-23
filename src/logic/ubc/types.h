#ifndef UBCTYPES_H
#define UBCTYPES_H

#include <stdint.h>

typedef struct UbcString
{
    unsigned int memOffset;
    unsigned int length;
} ubcString_t;

enum UbcType {
    UBCTYPE_INTEGER,
    UBCTYPE_BOOLEAN,
    UBCTYPE_CUSTOM,
    UBCTYPE_STRING,
    //UBCTYPE_VECTOR
    UBCTYPE_FLOAT,
};

typedef struct UbcCustomType
{
    enum UbcType field_types;
    char*   field_names;
    void*   fields;
    int     field_count;
} ubcCustomType_t;

#endif //UBCTYPES
