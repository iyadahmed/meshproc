#pragma once

#include <assert.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "aabb.h"
#include "macros.h"
#include "triangle.h"
#include "vec3.h"

STRUCT(BVH_Node)
{
    AABB aabb;
    BVH_Node *left, *right;
    uint32_t start, count;
};

STRUCT(BVH)
{
    BVH_Node *root;
    Triangle *triangles;
    uint32_t *indices;
};

uint32_t count_leaf_triangles(BVH_Node *node)
{
    if (node == NULL)
    {
        return 0;
    }
    else if (node->left == NULL && node->right == NULL)
    {
        return node->count;
    }
    else
    {
        uint32_t l = count_leaf_triangles(node->left);
        uint32_t r = count_leaf_triangles(node->right);
        return l + r;
    }
}

BVH_Node *create_node(Triangle *triangles, uint32_t *indices, uint32_t start, uint32_t count)
{
    BVH_Node *node = malloc(sizeof(BVH_Node));
    if (node == NULL)
    {
        fputs("Failed to allocate memory for BVH_Node\n", stderr);
        exit(EXIT_FAILURE);
    }
    node->aabb = triangles[indices[start]].aabb;
    for (uint32_t i = start; i < (start + count); i++)
    {
        node->aabb = aabb_union(&(node->aabb), &(triangles[indices[i]].aabb));
    }
    node->start = start;
    node->count = count;
    node->left = NULL;
    node->right = NULL;
    return node;
}

BVH build_bvh(Triangle *triangles, uint32_t triangle_count)
{
    uint32_t *indices = malloc(sizeof(uint32_t) * triangle_count);
    if (indices == NULL)
    {
        fputs("Failed to allocate memory for BVH indices\n", stderr);
        exit(EXIT_FAILURE);
    }
    for (uint32_t i = 0; i < triangle_count; i++)
    {
        indices[i] = i;
    }

    BVH_Node *root = create_node(triangles, indices, 0, triangle_count);

    BVH_Node *stack[64];
    int stack_size = 0;
    stack[stack_size++] = root;

    while (stack_size > 0)
    {
        BVH_Node *node = stack[--stack_size];
        if (node->count <= 2)
        {
            continue;
        }

        int split_axis = 0;
        float split_pos = 0.0f;

#if 0
        AABB aabb = node->aabb;
        Vec3 extents = vec3_sub(aabb.max, aabb.min);
        split_axis = 0;
        if (extents.y > extents.x)
        {
            split_axis = 1;
        }
        if (extents.z > extents.as_array[split_axis])
        {
            split_axis = 2;
        }
        split_pos = (aabb.min.as_array[split_axis] + aabb.max.as_array[split_axis]) / 2.0f;
#else
        Vec3 mean = VEC3(0, 0, 0);
        Vec3 mean_of_squares = VEC3(0, 0, 0);
        for (uint32_t i = node->start; i < (node->start + node->count); i++)
        {
            Vec3 centroid = triangles[indices[i]].centroid;
            mean = vec3_add(mean, vec3_div_scalar(centroid, node->count));
            mean_of_squares = vec3_add(mean_of_squares, vec3_div_scalar(vec3_mul(centroid, centroid), node->count));
        }
        Vec3 variance = vec3_sub(mean_of_squares, vec3_mul(mean, mean));
        split_axis = 0;
        if (variance.y > variance.x)
        {
            split_axis = 1;
        }
        if (variance.z > variance.as_array[split_axis])
        {
            split_axis = 2;
        }
        split_pos = mean.as_array[split_axis];
#endif

        uint32_t i = node->start;
        uint32_t j = node->start + node->count - 1;
        do
        {
            while (i <= j && triangles[indices[i]].centroid.as_array[split_axis] < split_pos)
            {
                i++;
            }
            while (i <= j && triangles[indices[j]].centroid.as_array[split_axis] >= split_pos)
            {
                j--;
            }

            if (i < j)
            {
                uint32_t tmp = indices[i];
                indices[i] = indices[j];
                indices[j] = tmp;
                i++;
                j--;
            }
        } while (i < j);

        if (i == node->start || i == node->start + node->count)
        {
            continue;
        }

        node->left = create_node(triangles, indices, node->start, i - node->start);
        node->right = create_node(triangles, indices, i, node->count - node->left->count);

        stack[stack_size++] = node->left;
        stack[stack_size++] = node->right;
    }

    assert(count_leaf_triangles(root) == triangle_count);

    BVH bvh;
    bvh.indices = indices;
    bvh.root = root;
    bvh.triangles = triangles;
    return bvh;
}

void free_node_tree(BVH_Node *node)
{
    if (node->left)
        free_node_tree(node->left);

    if (node->right)
        free_node_tree(node->right);

    free(node);
}

void free_bvh(BVH *bvh)
{
    free_node_tree(bvh->root);
    free(bvh->indices);
}

uint32_t largest_leaf(BVH_Node *node)
{
    if (node == NULL)
    {
        return 0;
    }
    else if (node->left == NULL && node->right == NULL)
    {
        return node->count;
    }
    else
    {
        uint32_t l = largest_leaf(node->left);
        uint32_t r = largest_leaf(node->right);
        return (l > r) ? l : r;
    }
}
