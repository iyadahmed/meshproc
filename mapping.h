#pragma once

#include <float.h>
#include <math.h>

float map_float_to_zero_one(float input)
{
    // Handle the special case when the input is 0
    if (input == 0.0f)
    {
        return 1.0f;
    }

    // Use the reciprocal function to map the input to the range (0, 1]
    float mapped_value = 1.0f / (1.0f + input);

    // The result should be within the range [0, 1], so no clamping is needed
    return mapped_value;
}
