#pragma once

#include <stdbool.h>

#include "aabb.h"
#include "distance_point_to_plane.h"
#include "macros.h"
#include "vec3.h"

STRUCT(Triangle)
{
    Vec3 vertices[3];
    float edge_lengths[3];
    Vec3 normalized_edge_vectors[3];
    Vec3 normal;
    Vec3 auxilary_plane_normals[3];
    AABB aabb;
    Vec3 centroid;
};

Vec3 calc_triangle_centroid(Vec3 a, Vec3 b, Vec3 c)
{
    return vec3_div_scalar(vec3_add(a, vec3_add(b, c)), 3.0f);
}

AABB calc_triangle_aabb(Vec3 a, Vec3 b, Vec3 c)
{
    AABB result;
    result.max = vec3_max(a, vec3_max(b, c));
    result.min = vec3_min(a, vec3_min(b, c));
    return result;
}

Triangle create_triangle(Vec3 a, Vec3 b, Vec3 c)
{
    Triangle triangle = {0};
    triangle.vertices[0] = a;
    triangle.vertices[1] = b;
    triangle.vertices[2] = c;

    Vec3 e1 = vec3_sub(b, a);
    Vec3 e2 = vec3_sub(c, b);
    Vec3 e3 = vec3_sub(a, c);

    triangle.edge_lengths[0] = vec3_length(e1);
    triangle.edge_lengths[1] = vec3_length(e2);
    triangle.edge_lengths[2] = vec3_length(e3);

    triangle.normalized_edge_vectors[0] = vec3_div_scalar(e1, triangle.edge_lengths[0]);
    triangle.normalized_edge_vectors[1] = vec3_div_scalar(e2, triangle.edge_lengths[1]);
    triangle.normalized_edge_vectors[2] = vec3_div_scalar(e3, triangle.edge_lengths[2]);

    triangle.normal = vec3_normalize(vec3_cross(e1, e2));

    triangle.auxilary_plane_normals[0] = vec3_cross(triangle.normal, e1);
    triangle.auxilary_plane_normals[1] = vec3_cross(triangle.normal, e2);
    triangle.auxilary_plane_normals[2] = vec3_cross(triangle.normal, e3);

    triangle.aabb = calc_triangle_aabb(a, b, c);
    triangle.centroid = calc_triangle_centroid(a, b, c);

    return triangle;
}

bool is_above_plane(Vec3 point, Vec3 plane_point, Vec3 plane_normal)
{
    return distance_point_to_plane(point, plane_point, plane_normal) >= -0.00001f;
}

bool is_inside_triangle_prism(Vec3 point, Triangle *triangle)
{
    return is_above_plane(point, triangle->vertices[0], triangle->auxilary_plane_normals[0]) &&
           is_above_plane(point, triangle->vertices[1], triangle->auxilary_plane_normals[1]) &&
           is_above_plane(point, triangle->vertices[2], triangle->auxilary_plane_normals[2]);
}

bool is_almost_zero(float value)
{
    return fabsf(value) < 0.00001f;
}

bool is_almost_equal(float a, float b)
{
    return fabsf(a - b) < 0.00001f;
}

bool is_almost_unit(Vec3 v)
{
    return is_almost_equal(vec3_length(v), 1.0f);
}

bool ray_plane_intersection(Vec3 ray_origin, Vec3 ray_direction, Vec3 plane_point, Vec3 plane_normal, float *t_out)
{
    float l_dot_n = vec3_dot(ray_direction, plane_normal);
    if (is_almost_zero(l_dot_n))
    {
        return false;
    }
    float v_dot_n = vec3_dot(vec3_sub(plane_point, ray_origin), plane_normal);
    *t_out = v_dot_n / l_dot_n;
    return (*t_out) > -0.00001f;
}

bool ray_triangle_intersection(Vec3 ray_origin, Vec3 ray_direction, Triangle *triangle, float *t_out)
{
    if (ray_plane_intersection(ray_origin, ray_direction, triangle->vertices[0], triangle->normal, t_out))
    {
        Vec3 p = vec3_add(vec3_scale(ray_direction, *t_out), ray_origin);
        return is_inside_triangle_prism(p, triangle);
    }
    return false;
}
