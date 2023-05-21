#pragma once

#include <float.h>
#include <math.h>
#include <stdbool.h>

#include "aabb.h"
#include "triangle.h"
#include "vec3.h"

// Helper function to check if an axis is separating
bool is_axis_separating(Vec3 *aabb_vertices, Vec3 *triangle_vertices, Vec3 axis)
{
    float aabb_min = FLT_MAX, aabb_max = -FLT_MAX;
    float triangle_min = FLT_MAX, triangle_max = -FLT_MAX;

    for (int i = 0; i < 8; ++i)
    {
        float projection = vec3_dot(aabb_vertices[i], axis);
        aabb_min = fminf(aabb_min, projection);
        aabb_max = fmaxf(aabb_max, projection);
    }

    for (int i = 0; i < 3; ++i)
    {
        float projection = vec3_dot(triangle_vertices[i], axis);
        triangle_min = fminf(triangle_min, projection);
        triangle_max = fmaxf(triangle_max, projection);
    }

    return (aabb_min > triangle_max) || (triangle_min > aabb_max);
}

bool triangle_aabb_intersection(Triangle *triangle, AABB *aabb)
{
    // AABB vertices
    Vec3 aabb_vertices[8] = {
        {aabb->min.x, aabb->min.y, aabb->min.z},
        {aabb->min.x, aabb->min.y, aabb->max.z},
        {aabb->min.x, aabb->max.y, aabb->min.z},
        {aabb->min.x, aabb->max.y, aabb->max.z},
        {aabb->max.x, aabb->min.y, aabb->min.z},
        {aabb->max.x, aabb->min.y, aabb->max.z},
        {aabb->max.x, aabb->max.y, aabb->min.z},
        {aabb->max.x, aabb->max.y, aabb->max.z}};

    // Triangle vertices
    Vec3 triangle_vertices[3] = {
        triangle->vertices[0],
        triangle->vertices[1],
        triangle->vertices[2]};

    // 3 axes corresponding to the AABB faces
    Vec3 aabb_axes[3] = {
        {1, 0, 0},
        {0, 1, 0},
        {0, 0, 1}};

    // Check AABB axes
    for (int i = 0; i < 3; ++i)
    {
        if (is_axis_separating(aabb_vertices, triangle_vertices, aabb_axes[i]))
        {
            return false;
        }
    }

    // Check triangle normal axis
    if (is_axis_separating(aabb_vertices, triangle_vertices, triangle->normal))
    {
        return false;
    }

    // Check cross product axes
    for (int i = 0; i < 3; ++i)
    {
        for (int j = 0; j < 3; ++j)
        {
            Vec3 cross_axis = vec3_cross(aabb_axes[i], triangle->normalized_edge_vectors[j]);
            if (is_axis_separating(aabb_vertices, triangle_vertices, cross_axis))
            {
                return false;
            }
        }
    }

    // No separating axis found, intersection occurs
    return true;
}
