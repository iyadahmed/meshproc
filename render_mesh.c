#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "bvh.h"
#include "mapping.h"
#include "mesh_io.h"
#include "ray_bvh_intersection.h"
#include "timer.h"
#include "triangle.h"

int main(int argc, char **argv)
{
    puts("render_mesh: a program for rendering a binary STL mesh file to a PPM image");
    if (argc != 2)
    {
        fputs("Expected arguments: /path/to/mesh.stl\n", stderr);
        return 1;
    }

    const char *mesh_filepath = argv[1];

    FILE *ppm_file = NULL;
    Triangle *triangles = NULL;
    uint32_t triangle_count = 0;
    BVH *bvh_ptr = NULL;
    uint8_t *image = NULL;
    Millis_Timer timer;

    int width = 1200;
    int height = 800;

    image = malloc(width * height * 3 * sizeof(uint8_t));
    if (image == NULL)
    {
        goto cleanup;
    }

    timer_start(&timer);
    load_binary_stl(mesh_filepath, &triangles, &triangle_count);
    printf("Loading mesh took %lu ms\n", timer_stop(&timer));

    timer_start(&timer);
    BVH bvh = build_bvh(triangles, triangle_count);
    printf("Building BVH took %lu ms\n", timer_stop(&timer));
    bvh_ptr = &bvh;

    ppm_file = fopen("image.ppm", "wb");
    if (ppm_file == NULL)
    {
        fputs("Failed to open image.ppm for writing\n", stderr);
        goto cleanup;
    }

    fprintf(ppm_file, "P6\n%d %d\n255\n", width, height);

    Vec3 camera_location = VEC3(50, -200, 50);
    Vec3 camera_up = VEC3(0, 0, 1);
    Vec3 camera_right = VEC3(1, 0, 0);
    Vec3 camera_forward = VEC3(0, 2, 0);

    timer_start(&timer);

    // NOTE: we declare j before the OpenMP pragma to make it work with MSVC
    int j;
#pragma omp parallel for schedule(dynamic)
    for (j = 0; j < height; j++)
    {
        for (int i = 0; i < width; i++)
        {
            float x = (float)i / (float)width;
            float y = (float)j / (float)height;

            x = 2 * x - 1;
            x *= (float)width / (float)height;
            y = 2 * y - 1;
            y = 0 - y;

            Vec3 pixel_pos = vec3_add(vec3_add(
                                          vec3_add(camera_location, camera_forward),
                                          vec3_scale(camera_right, x)),
                                      vec3_scale(camera_up, y));
            Vec3 ray_direction = vec3_normalize(vec3_sub(pixel_pos, camera_location));

            // bool is_hit = false;
            // float t, min_t;
            // for (uint32_t ti = 0; ti < triangle_count; ti++)
            // {
            //     if (ray_triangle_intersection(camera_location, ray_direction, triangles + ti, &t))
            //     {
            //         if (is_hit)
            //         {
            //             if (t < min_t)
            //             {
            //                 min_t = t;
            //             }
            //         }
            //         else
            //         {
            //             min_t = t;
            //             is_hit = true;
            //         }
            //     }
            // }
            float min_t;
            bool is_hit = ray_bvh_intersection(bvh_ptr, camera_location, ray_direction, &min_t);

            if (is_hit)
            {
                image[(i + j * width) * 3 + 0] = map_float_to_zero_one(min_t) * 255;
                image[(i + j * width) * 3 + 1] = 0;
                image[(i + j * width) * 3 + 2] = 0;
            }
            else
            {
                image[(i + j * width) * 3 + 0] = 255;
                image[(i + j * width) * 3 + 1] = 255;
                image[(i + j * width) * 3 + 2] = 255;
            }
        }
    }

    printf("Rendering took %lu ms\n", timer_stop(&timer));

    fwrite(image, sizeof(uint8_t), width * height * 3, ppm_file);

cleanup:
    if (ppm_file)
        fclose(ppm_file);

    if (triangles)
        free(triangles);

    if (bvh_ptr)
        free_bvh(bvh_ptr);

    if (image)
        free(image);

    return 0;
}
