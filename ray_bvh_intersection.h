#pragma once

#include <stdbool.h>

#include "bvh.h"
#include "triangle.h"

bool ray_aabb_intersection(Vec3 ray_origin, Vec3 ray_direction, AABB *aabb)
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
    return true;
}

bool ray_bvh_intersection(BVH *bvh, Vec3 ray_origin, Vec3 ray_direction, float *t_out)
{
    BVH_Node *stack[64];
    int stack_size = 0;
    stack[stack_size++] = bvh->root;

    float t;
    bool is_hit = false;

    while (stack_size > 0)
    {
        BVH_Node *node = stack[--stack_size];
        if (ray_aabb_intersection(ray_origin, ray_direction, &(node->aabb)))
        {
            if (node->left == NULL && node->right == NULL)
            {
                for (uint32_t i = node->start; i < (node->start + node->count); i++)
                {
                    if (ray_triangle_intersection(ray_origin, ray_direction, bvh->triangles + bvh->indices[i], &t))
                    {
                        if (is_hit)
                        {
                            if (t < *t_out)
                            {
                                *t_out = t;
                            }
                        }
                        else
                        {
                            *t_out = t;
                            is_hit = true;
                        }
                    }
                }
            }
            else
            {
                stack[stack_size++] = node->left;
                stack[stack_size++] = node->right;
            }
        }
    }

    return is_hit;
}
