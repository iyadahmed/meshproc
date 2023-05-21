#pragma once

#include <stdbool.h>

#include "triangle.h"
#include "vec3.h"

bool edge_triangle_intersection(Vec3 edge_origin, Vec3 edge_direction, float edge_length, Triangle *triangle, float *t_out)
{
    if (ray_triangle_intersection(edge_origin, edge_direction, triangle, t_out))
    {
        return (*t_out) < edge_length;
    }
    return false;
}
