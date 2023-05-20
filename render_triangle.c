#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "bvh.h"
#include "mapping.h"
#include "ray_bvh_intersection.h"
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

    FILE *mesh_file = NULL, *ppm_file = NULL;
    Triangle *triangles = NULL;
    BVH *bvh_ptr = NULL;

    mesh_file = fopen(mesh_filepath, "rb");
    if (mesh_file == NULL)
    {
        fputs("Failed to open mesh file\n", stderr);
        goto cleanup;
    }

    if (fseek(mesh_file, 80, SEEK_CUR) != 0)
    {
        fputs("Failed to skip mesh file header\n", stderr);
        goto cleanup;
    }

    uint32_t triangle_count = 0;
    if (fread(&triangle_count, sizeof(uint32_t), 1, mesh_file) != 1)
    {
        fputs("Failed to read number of triangles from mesh file\n", stderr);
        goto cleanup;
    }

    triangles = malloc(sizeof(Triangle) * triangle_count);
    if (triangles == NULL)
    {
        fputs("Failed to allocate memory for mesh triangles\n", stderr);
        goto cleanup;
    }

    assert(sizeof(Vec3) == sizeof(float[3]));
    for (uint32_t i = 0; i < triangle_count; i++)
    {
        if (fseek(mesh_file, sizeof(Vec3), SEEK_CUR) != 0)
        {
            fputs("Failed to skip normals in mesh file\n", stderr);
            goto cleanup;
        }

        Vec3 vertices[3];
        if (fread(vertices, sizeof(Vec3), 3, mesh_file) != 3)
        {
            fputs("Failed to read vertices from mesh file\n", stderr);
            goto cleanup;
        }

        triangles[i] = create_triangle(vertices[0], vertices[1], vertices[2]);

        if (fseek(mesh_file, sizeof(uint16_t), SEEK_CUR) != 0)
        {
            fputs("Failed to skip \"attribute byte count\" in mesh file\n", stderr);
            goto cleanup;
        }
    }

    BVH bvh = build_bvh(triangles, triangle_count);
    bvh_ptr = &bvh;

    ppm_file = fopen("image.ppm", "wb");
    if (ppm_file == NULL)
    {
        fputs("Failed to open image.ppm for writing\n", stderr);
        goto cleanup;
    }

    int width = 1200;
    int height = 800;
    uint8_t color[3] = {255, 0, 255};

    fprintf(ppm_file, "P6\n%d %d\n255\n", width, height);

    Vec3 camera_location = VEC3(50, -200, 50);
    Vec3 camera_up = VEC3(0, 0, 1);
    Vec3 camera_right = VEC3(1, 0, 0);
    Vec3 camera_forward = VEC3(0, 2, 0);

    for (int j = 0; j < height; j++)
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
                color[0] = map_float_to_zero_one(min_t) * 255;
                color[1] = 0;
                color[2] = 0;
            }
            else
            {
                color[0] = 255;
                color[1] = 255;
                color[2] = 255;
            }

            fwrite(color, sizeof(uint8_t), 3, ppm_file);
        }
    }

cleanup:
    if (ppm_file)
        fclose(ppm_file);

    if (mesh_file)
        fclose(mesh_file);

    if (triangles)
        free(triangles);

    if (bvh_ptr)
        free_bvh(bvh_ptr);

    return 0;
}
