#pragma once

#include "macros.h"
#include "vec3.h"

STRUCT(AABB)
{
    Vec3 max, min;
};

AABB aabb_union(AABB *a, AABB *b)
{
    AABB result;
    result.max = vec3_max(a->max, b->max);
    result.min = vec3_min(a->min, b->min);
    return result;
}
