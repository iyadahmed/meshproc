#pragma once

#include <math.h>
#include <stdbool.h>

#include "aabb.h"
#include "vec3.h"

bool ray_aabb_intersection(Vec3 ray_origin, Vec3 ray_direction, AABB *aabb, float *t_min_out)
{
    float t_min = -INFINITY;
    float t_max = INFINITY;

    for (int i = 0; i < 3; i++)
    {
        if (fabsf(ray_direction.as_array[i]) < 1e-6)
        {
            if (ray_origin.as_array[i] < aabb->min.as_array[i] || ray_origin.as_array[i] > aabb->max.as_array[i])
                return false;
        }
        else
        {
            float t1 = (aabb->min.as_array[i] - ray_origin.as_array[i]) / ray_direction.as_array[i];
            float t2 = (aabb->max.as_array[i] - ray_origin.as_array[i]) / ray_direction.as_array[i];

            if (t1 > t2)
            {
                float temp = t1;
                t1 = t2;
                t2 = temp;
            }

            t_min = fmaxf(t_min, t1);
            t_max = fminf(t_max, t2);

            if (t_min > t_max)
                return false;
        }
    }
    *t_min_out = t_min;
    return true;
}
