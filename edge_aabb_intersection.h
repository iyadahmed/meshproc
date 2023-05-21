#pragma once

#include <stdbool.h>

#include "aabb.h"
#include "ray_aabb_intersection.h"
#include "vec3.h"

bool edge_aabb_intersection(Vec3 edge_origin, Vec3 edge_direction, float edge_length, AABB *aabb, float *t_out)
{
    if (ray_aabb_intersection(edge_origin, edge_direction, aabb, t_out))
    {
        return (*t_out) < edge_length;
    }
    return false;
}
