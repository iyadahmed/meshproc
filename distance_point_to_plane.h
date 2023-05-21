#pragma once

#include "vec3.h"

float distance_point_to_plane(Vec3 point, Vec3 plane_point, Vec3 plane_normal)
{
    return vec3_dot(vec3_sub(point, plane_point), plane_normal);
}
