#pragma once

#include <math.h>
#include <stdbool.h>

#include "macros.h"

UNION(Vec3)
{
    struct
    {
        float x, y, z;
    };
    float as_array[3];
};

#define VEC3(x, y, z) ((Vec3){{(x), (y), (z)}})

Vec3 vec3_add(Vec3 a, Vec3 b)
{
    return VEC3(a.x + b.x, a.y + b.y, a.z + b.z);
}

Vec3 vec3_sub(Vec3 a, Vec3 b)
{
    return VEC3(a.x - b.x, a.y - b.y, a.z - b.z);
}

Vec3 vec3_mul(Vec3 a, Vec3 b)
{
    return VEC3(a.x * b.x, a.y * b.y, a.z * b.z);
}

Vec3 vec3_div(Vec3 a, Vec3 b)
{
    return VEC3(a.x / b.x, a.y / b.y, a.z / b.z);
}

float vec3_dot(Vec3 a, Vec3 b)
{
    return a.x * b.x + a.y * b.y + a.z * b.z;
}

Vec3 vec3_cross(Vec3 a, Vec3 b)
{
    return VEC3(
        a.y * b.z - a.z * b.y,
        a.z * b.x - a.x * b.z,
        a.x * b.y - a.y * b.x);
}

Vec3 vec3_scale(Vec3 a, float b)
{
    return VEC3(a.x * b, a.y * b, a.z * b);
}

Vec3 vec3_div_scalar(Vec3 a, float b)
{
    return VEC3(a.x / b, a.y / b, a.z / b);
}

Vec3 vec3_normalize(Vec3 v)
{
    float l = sqrtf(v.x * v.x + v.y * v.y + v.z * v.z);
    return VEC3(v.x / l, v.y / l, v.z / l);
}

float vec3_length(Vec3 v)
{
    return sqrtf(v.x * v.x + v.y * v.y + v.z * v.z);
}

float vec3_distance(Vec3 a, Vec3 b)
{
    return vec3_length(vec3_sub(a, b));
}

float vec3_length_squared(Vec3 v)
{
    return v.x * v.x + v.y * v.y + v.z * v.z;
}

Vec3 vec3_max(Vec3 a, Vec3 b)
{
    return VEC3(fmaxf(a.x, b.x), fmaxf(a.y, b.y), fmaxf(a.z, b.z));
}

Vec3 vec3_min(Vec3 a, Vec3 b)
{
    return VEC3(fminf(a.x, b.x), fminf(a.y, b.y), fminf(a.z, b.z));
}

bool vec3_are_close(Vec3 a, Vec3 b)
{
    return vec3_distance(a, b) < 0.00001f;
}
