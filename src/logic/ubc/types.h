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
    char*   name;
    char**  field_typenames;
    char**  field_names;
    int     field_count;
    size_t  type_size;
} ubccustomtype_t;

#endif //UBCTYPES
