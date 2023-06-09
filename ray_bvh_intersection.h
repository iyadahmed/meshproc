#pragma once

#include <stdbool.h>

#include "bvh.h"
#include "ray_aabb_intersection.h"
#include "triangle.h"

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
        float aabb_t_min;
        if (ray_aabb_intersection(ray_origin, ray_direction, &(node->aabb), &aabb_t_min))
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
