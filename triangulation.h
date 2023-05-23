#pragma once

#include <math.h>
#include <stdlib.h>

// Define REAL for the "Triangle" library
#ifdef SINGLE
#define REAL float
#else
#define REAL double
#endif
#include "triangle/triangle.h"

#include "dynamic_array_meta.h"
#include "dynamic_array_uint32_pair.h"
#include "dynamic_array_vec3.h"
#include "macros.h"
#include "vec3.h"

int vec3_lexicographical_compare(Vec3 *a, Vec3 *b)
{
    for (int i = 0; i < 3; i++)
    {
        if (a->as_array[i] < b->as_array[i])
        {
            return -1;
        }
        else if (a->as_array[i] > b->as_array[i])
        {
            return 1;
        }
    }
    return 0;
}

int vec3_lexicographical_compare_qsort_variant(const void *a, const void *b)
{
    return vec3_lexicographical_compare((Vec3 *)a, (Vec3 *)b);
}

typedef Vec3 *Vec3_ptr;
DYNAMIC_ARRAY_IMPLEMENTATION(Vec3_ptr);

void meshproc_triangulate(const Dynamic_Array_Vec3 *points, const Dynamic_Array_uint32_Pair *edges, const Triangle *triangle, int **triangles_out, int *num_triangles_out)
{
    // Vec3 **points_ptrs = malloc(points->num_used * sizeof(Vec3 *));
    // if (points_ptrs == NULL)
    // {
    //     fputs("Failed to allocate memory\n", stderr);
    //     exit(EXIT_FAILURE);
    // }
    // for (size_t i = 0; i < points->num_used; i++)
    // {
    //     points_ptrs[i] = points->data + i;
    // }

    // Dynamic_Array_Vec3_ptr *neighbors = malloc(sizeof(Dynamic_Array_Vec3_ptr) * points->num_used);
    // if (neighbors == NULL)
    // {
    //     fputs("Failed to allocated memory\n", stderr);
    //     exit(EXIT_FAILURE);
    // }
    // for (size_t i = 0; i < points->num_used; i++)
    // {
    //     neighbors[i] = create_dynamic_array_Vec3_ptr();
    // }

    // for (size_t i = 0; i < edges->num_used; i++)
    // {
    //     push_dynamic_array_Vec3_ptr(neighbors + edges->data[i].first, points_ptrs[edges->data[i].second]);
    //     push_dynamic_array_Vec3_ptr(neighbors + edges->data[i].second, points_ptrs[edges->data[i].first]);
    // }

    // for (size_t i = 0; i < points->num_used; i++)
    // {
    //     Vec3 v = *(points_ptrs[i]);
    //     printf("(%f, %f, %f) ", v.x, v.y, v.z);
    // }
    // puts("");
    // qsort(points_ptrs, points->num_used, sizeof(Vec3 *), vec3_lexicographical_compare_qsort_variant);
    // for (size_t i = 0; i < points->num_used; i++)
    // {
    //     Vec3 v = *(points_ptrs[i]);
    //     printf("(%f, %f, %f) ", v.x, v.y, v.z);
    // }
    // puts("");

    // for (size_t i = 0; i < points->num_used; i++)
    // {
    //     Vec3 v = *(points_ptrs[i]);
    //     printf("(%f, %f, %f) ", v.x, v.y, v.z);
    // }

    // {
    //     size_t i = 0;
    //     while (i < points->num_used)
    //     {
    //     }
    // }

    // for (size_t i = 0; i < points->num_used; i++)
    // {
    //     Vec3 v = *(points_ptrs[i]);
    //     printf("(%f, %f, %f) ", v.x, v.y, v.z);
    // }

    REAL *projected_points = malloc(sizeof(REAL) * 2 * points->num_used);
    if (projected_points == NULL)
    {
        fputs("Failed to allocate memory\n", stderr);
        exit(EXIT_FAILURE);
    }
    for (size_t i = 0; i < points->num_used; i++)
    {
        Vec3 v = points->data[i];
        // Project point to triangle
        float x = vec3_dot(v, triangle->normalized_edge_vectors[0]);
        float y = vec3_dot(v, triangle->normalized_edge_vectors[1]);
        projected_points[i * 2 + 0] = x;
        projected_points[i * 2 + 1] = y;
    }

    int *segment_list = malloc(sizeof(int) * 2 * edges->num_used);
    for (size_t i = 0; i < edges->num_used; i++)
    {
        segment_list[i * 2 + 0] = (int)(edges->data[i].first);
        segment_list[i * 2 + 1] = (int)(edges->data[i].second);
    }

    // Using the "Triangle" library
    {
        struct triangulateio in;
        in.pointlist = projected_points;
        in.numberofpoints = (int)(points->num_used);

        in.pointmarkerlist = NULL;
        in.numberofpointattributes = 0;

        in.segmentlist = segment_list;
        in.numberofsegments = (int)(edges->num_used);
        in.segmentmarkerlist = NULL;

        in.numberofholes = 0;
        in.numberofregions = 0;

        struct triangulateio out;
        out.pointlist = NULL;
        out.trianglelist = NULL;
        out.segmentlist = NULL;

        triangulate("pNPz", &in, &out, NULL);
        *triangles_out = out.trianglelist;
        *num_triangles_out = out.numberoftriangles;
    }

    free(segment_list);
    free(projected_points);
}
