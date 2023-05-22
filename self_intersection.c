#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "bvh.h"
#include "dynamic_array_uint32_pair.h"
#include "dynamic_array_vec3.h"
#include "edge_aabb_intersection.h"
#include "edge_triangle_intersection.h"
#include "mapping.h"
#include "mesh_io.h"
#include "timer.h"
#include "triangle.h"
#include "triangle_aabb_intersection.h"
#include "triangle_triangle_intersection.h"

int main(int argc, char **argv)
{
    puts("boolean: a program for resolving mesh self intersections");
    if (argc != 2)
    {
        fputs("Expected arguments: /path/to/mesh.stl\n", stderr);
        return 1;
    }

    const char *mesh_filepath = argv[1];

    FILE *ppm_file = NULL;
    FILE *points_file = NULL;
    FILE *edges_file = NULL;
    Triangle *triangles = NULL;
    uint32_t triangle_count = 0;
    BVH *bvh_ptr = NULL;
    Millis_Timer timer;
    Dynamic_Array_Vec3 *intersection_points_ptr = NULL;
    Dynamic_Array_uint32_Pair *intersection_edges_ptr = NULL;

    timer_start(&timer);
    load_binary_stl(mesh_filepath, &triangles, &triangle_count);
    printf("Loading mesh took %lu ms\n", timer_stop(&timer));

    timer_start(&timer);
    BVH bvh = build_bvh(triangles, triangle_count);
    printf("Building BVH took %lu ms\n", timer_stop(&timer));
    bvh_ptr = &bvh;

    points_file = fopen("points.bin", "wb");
    if (points_file == NULL)
    {
        goto cleanup;
    }

    edges_file = fopen("edges.bin", "wb");
    if (edges_file == NULL)
    {
        goto cleanup;
    }

    Dynamic_Array_Vec3 intersection_points = create_dynamic_array_Vec3();
    intersection_points_ptr = &intersection_points;
    Dynamic_Array_uint32_Pair intersection_edges = create_dynamic_array_uint32_Pair();
    intersection_edges_ptr = &intersection_edges;

    for (uint32_t i = 0; i < triangle_count; i++)
    {
        Triangle *triangle = triangles + i;
        {
            BVH_Node *stack[64];
            int stack_size = 0;
            stack[stack_size++] = bvh.root;

            while (stack_size > 0)
            {
                BVH_Node *node = stack[--stack_size];
                float aabb_t_min;
                if (triangle_aabb_intersection(triangle, &(node->aabb)))
                {
                    if (node->left == NULL && node->right == NULL)
                    {
                        for (uint32_t i = node->start; i < (node->start + node->count); i++)
                        {
                            triangle_triangle_intersection_non_coplanar(triangle, bvh.triangles + bvh.indices[i], &intersection_points, &intersection_edges);
                        }
                    }
                    else
                    {
                        stack[stack_size++] = node->left;
                        stack[stack_size++] = node->right;
                    }
                }
            }
        }
    }

    fwrite(intersection_points.data, intersection_points.type_size, intersection_points.num_used, points_file);
    fwrite(intersection_edges.data, intersection_edges.type_size, intersection_edges.num_used, edges_file);

cleanup:
    if (ppm_file)
        fclose(ppm_file);

    if (triangles)
        free(triangles);

    if (bvh_ptr)
        free_bvh(bvh_ptr);

    if (points_file)
        fclose(points_file);

    if (edges_file)
        fclose(edges_file);

    if (intersection_points_ptr)
        free_dynamic_array_Vec3(intersection_points_ptr);

    if (intersection_edges_ptr)
        free_dynamic_array_uint32_Pair(intersection_edges_ptr);

    return 0;
}
