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
#include "triangulation.h"

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
    FILE *triangles_file = NULL;
    Triangle *triangles = NULL;
    uint32_t triangle_count = 0;
    BVH *bvh_ptr = NULL;
    Millis_Timer timer;
    Dynamic_Array_Vec3 *intersection_points_per_triangle = NULL;
    Dynamic_Array_uint32_Pair *intersection_edges_per_triangle = NULL;

    timer_start(&timer);
    load_binary_stl(mesh_filepath, &triangles, &triangle_count);
    printf("Loading mesh took %lu ms\n", timer_stop(&timer));

    timer_start(&timer);
    BVH bvh = build_bvh(triangles, triangle_count);
    printf("Building BVH took %lu ms\n", timer_stop(&timer));
    bvh_ptr = &bvh;

    triangles_file = fopen("triangles.bin", "wb");
    if (triangles_file == NULL)
    {
        goto cleanup;
    }

    intersection_points_per_triangle = malloc(triangle_count * sizeof(Dynamic_Array_Vec3));
    if (intersection_points_per_triangle == NULL)
    {
        goto cleanup;
    }
    intersection_edges_per_triangle = malloc(triangle_count * sizeof(Dynamic_Array_uint32_Pair));
    if (intersection_edges_per_triangle == NULL)
    {
        goto cleanup;
    }

    // Setup points and edges
    for (uint32_t i = 0; i < triangle_count; i++)
    {
        intersection_points_per_triangle[i] = create_dynamic_array_Vec3();
        push_dynamic_array_Vec3(intersection_points_per_triangle + i, triangles[i].vertices[0]);
        push_dynamic_array_Vec3(intersection_points_per_triangle + i, triangles[i].vertices[1]);
        push_dynamic_array_Vec3(intersection_points_per_triangle + i, triangles[i].vertices[2]);

        intersection_edges_per_triangle[i] = create_dynamic_array_uint32_Pair();
        push_dynamic_array_uint32_Pair(intersection_edges_per_triangle + i, (uint32_Pair){0, 1});
        push_dynamic_array_uint32_Pair(intersection_edges_per_triangle + i, (uint32_Pair){1, 2});
        push_dynamic_array_uint32_Pair(intersection_edges_per_triangle + i, (uint32_Pair){2, 0});
    }

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
                            triangle_triangle_intersection_non_coplanar(triangle, bvh.triangles + bvh.indices[i], intersection_points_per_triangle + i, intersection_edges_per_triangle + i);
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

    for (uint32_t i = 0; i < triangle_count; i++)
    {
        int *triangles_out;
        int num_triangles_out;
        meshproc_triangulate(intersection_points_per_triangle + i, intersection_edges_per_triangle + i, triangles + i, &triangles_out, &num_triangles_out);
        for (int ti = 0; ti < num_triangles_out; ti++)
        {
            for (int vi = 0; vi < 3; vi++)
            {
                assert((ti * 3 + vi) < (num_triangles_out * 3));
                int rvi = triangles_out[ti * 3 + vi];
                assert(rvi >= 0);
                assert(rvi < intersection_points_per_triangle[i].num_used);
                Vec3 v = intersection_points_per_triangle[i].data[rvi];
                fwrite(&v, sizeof(Vec3), 1, triangles_file);
            }
        }
        trifree(triangles_out);
    }

cleanup:
    if (ppm_file)
        fclose(ppm_file);

    if (triangles)
        free(triangles);

    if (bvh_ptr)
        free_bvh(bvh_ptr);

    if (triangles_file)
        fclose(triangles_file);

    if (intersection_points_per_triangle)
    {
        for (uint32_t i = 0; i < triangle_count; i++)
        {
            free_dynamic_array_Vec3(intersection_points_per_triangle + i);
        }
        free(intersection_points_per_triangle);
    }

    if (intersection_edges_per_triangle)
    {
        for (uint32_t i = 0; i < triangle_count; i++)
        {
            free_dynamic_array_uint32_Pair(intersection_edges_per_triangle + i);
        }
        free(intersection_edges_per_triangle);
    }

    return 0;
}
