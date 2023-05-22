#pragma once

#include <stdint.h>

#include "dynamic_array_meta.h"
#include "macros.h"

STRUCT(uint32_Pair)
{
    uint32_t a, b;
};

DYNAMIC_ARRAY_IMPLEMENTATION(uint32_Pair);
