#pragma once

#include <assert.h>
#include <stdbool.h>

#include "dynamic_array_uint32_pair.h"
#include "dynamic_array_vec3.h"
#include "edge_triangle_intersection.h"
#include "triangle.h"
#include "vec3.h"

Vec3 point_along_ray(Vec3 ray_origin, Vec3 ray_direction, float t)
{
    return vec3_add(ray_origin, vec3_scale(ray_direction, t));
}

bool is_triangle_vertex(Vec3 point, Triangle *triangle)
{
    return vec3_are_close(point, triangle->vertices[0]) ||
           vec3_are_close(point, triangle->vertices[1]) ||
           vec3_are_close(point, triangle->vertices[2]);
}

bool are_all_triangle_vertices(Vec3 *points, int size, Triangle *triangle)
{
    for (int i = 0; i < size; i++)
    {
        if (!is_triangle_vertex(points[i], triangle))
        {
            return false;
        }
    }
    return true;
}

void deduplicate_vec3_array_inplace(Vec3 *array, int *size)
{
    for (int i = 0; i < *size; i++)
    {
        for (int j = i + 1; j < *size;)
        {
            if (vec3_are_close(array[i], array[j]))
            {
                // Shift the remaining elements to the left to overwrite the duplicate
                for (int k = j; k < *size - 1; k++)
                {
                    array[k] = array[k + 1];
                }
                // Decrease the size of the array
                (*size)--;
            }
            else
            {
                // Move to the next element
                j++;
            }
        }
    }
}

void triangle_triangle_intersection_non_coplanar(Triangle *a, Triangle *b, Dynamic_Array_Vec3 *points_out, Dynamic_Array_uint32_Pair *edges_out)
{
    Vec3 points[6];
    int num_points = 0;

    for (int i = 0; i < 3; i++)
    {
        float t;
        if (edge_triangle_intersection(a->vertices[i], a->normalized_edge_vectors[i], a->edge_lengths[i], b, &t))
        {
            Vec3 p = point_along_ray(a->vertices[i], a->normalized_edge_vectors[i], t);
            points[num_points++] = p;
        }
        if (edge_triangle_intersection(b->vertices[i], b->normalized_edge_vectors[i], b->edge_lengths[i], a, &t))
        {
            Vec3 p = point_along_ray(b->vertices[i], b->normalized_edge_vectors[i], t);
            points[num_points++] = p;
        }
    }
    assert(num_points < 6);

    deduplicate_vec3_array_inplace(points, &num_points);

    assert(num_points >= 0);
    assert(num_points <= 2);

    if (are_all_triangle_vertices(points, num_points, a) && are_all_triangle_vertices(points, num_points, b))
    {
        return;
    }

    if (num_points == 1)
    {
        push_dynamic_array_Vec3(points_out, points[0]);
    }
    else if (num_points == 2)
    {
        size_t a = push_dynamic_array_Vec3(points_out, points[0]);
        size_t b = push_dynamic_array_Vec3(points_out, points[1]);
        push_dynamic_array_uint32_Pair(edges_out, (uint32_Pair){a, b});
    }
}
