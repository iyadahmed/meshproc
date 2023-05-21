#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "bvh.h"
#include "edge_bvh_intersection.h"
#include "mapping.h"
#include "mesh_io.h"
#include "ray_bvh_intersection.h"
#include "timer.h"
#include "triangle.h"

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
    Triangle *triangles = NULL;
    uint32_t triangle_count = 0;
    BVH *bvh_ptr = NULL;
    Millis_Timer timer;

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

    for (uint32_t i = 0; i < triangle_count; i++)
    {
        Triangle *triangle = triangles + i;

        for (int ei = 0; ei < 3; ei++)
        {
            Vec3 edge_origin = triangle->vertices[ei];
            Vec3 edge_direction = triangle->normalized_edge_vectors[ei];
            float t;
            if (edge_bvh_intersection(bvh_ptr, edge_origin, edge_direction, triangle->edge_lengths[ei], &t))
            {
                Vec3 point = vec3_add(edge_origin, vec3_scale(edge_direction, t));
                fwrite(&point, sizeof(Vec3), 1, points_file);
            }
        }
    }

cleanup:
    if (ppm_file)
        fclose(ppm_file);

    if (triangles)
        free(triangles);

    if (bvh_ptr)
        free_bvh(bvh_ptr);

    if (points_file)
        fclose(points_file);

    return 0;
}
