#pragma once

#include <stdint.h>

#include "dynamic_array_meta.h"
#include "macros.h"

STRUCT(uint32_Pair)
{
    uint32_t first, second;
};

DYNAMIC_ARRAY_IMPLEMENTATION(uint32_Pair);
