#pragma once

#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "triangle.h"

void load_binary_stl(const char *filepath, Triangle **triangles_out, uint32_t *triangle_count_out)
{
    FILE *file = NULL;
    Triangle *triangles = NULL;

    file = fopen(filepath, "rb");
    if (file == NULL)
    {
        fputs("Failed to open mesh file\n", stderr);
        exit(EXIT_FAILURE);
    }

    if (fseek(file, 80, SEEK_CUR) != 0)
    {
        fputs("Failed to skip mesh file header\n", stderr);
        exit(EXIT_FAILURE);
    }

    uint32_t triangle_count = 0;
    if (fread(&triangle_count, sizeof(uint32_t), 1, file) != 1)
    {
        fputs("Failed to read number of triangles from mesh file\n", stderr);
        exit(EXIT_FAILURE);
    }

    triangles = malloc(sizeof(Triangle) * triangle_count);
    if (triangles == NULL)
    {
        fputs("Failed to allocate memory for mesh triangles\n", stderr);
        exit(EXIT_FAILURE);
    }

    assert(sizeof(Vec3) == sizeof(float[3]));
    for (uint32_t i = 0; i < triangle_count; i++)
    {
        if (fseek(file, sizeof(Vec3), SEEK_CUR) != 0)
        {
            fputs("Failed to skip normals in mesh file\n", stderr);
            exit(EXIT_FAILURE);
        }

        Vec3 vertices[3];
        if (fread(vertices, sizeof(Vec3), 3, file) != 3)
        {
            fputs("Failed to read vertices from mesh file\n", stderr);
            exit(EXIT_FAILURE);
        }

        triangles[i] = create_triangle(vertices[0], vertices[1], vertices[2]);

        if (fseek(file, sizeof(uint16_t), SEEK_CUR) != 0)
        {
            fputs("Failed to skip \"attribute byte count\" in mesh file\n", stderr);
            exit(EXIT_FAILURE);
        }
    }

    fclose(file);
    *triangles_out = triangles;
    *triangle_count_out = triangle_count;
}
