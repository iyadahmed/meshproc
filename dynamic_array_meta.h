#pragma once

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

#include "macros.h"

#define DYNAMIC_ARRAY_IMPLEMENTATION(type)                                      \
    STRUCT(Dynamic_Array_##type)                                                \
    {                                                                           \
        type *data;                                                             \
        size_t cap;                                                             \
        size_t num_used;                                                        \
        size_t type_size;                                                       \
    };                                                                          \
    Dynamic_Array_##type create_dynamic_array_##type()                          \
    {                                                                           \
        Dynamic_Array_##type arr;                                               \
        arr.data = malloc(sizeof(type));                                        \
        if (arr.data == NULL)                                                   \
        {                                                                       \
            fputs("Failed to allocate memory for dynamic array", stderr);       \
            exit(EXIT_FAILURE);                                                 \
        }                                                                       \
        arr.cap = 1;                                                            \
        arr.num_used = 0;                                                       \
        arr.type_size = sizeof(type);                                           \
        return arr;                                                             \
    }                                                                           \
    void resize_dynamic_array_##type(Dynamic_Array_##type *arr, size_t new_cap) \
    {                                                                           \
        type *new_data = realloc(arr->data, new_cap * sizeof(type));            \
        if (new_data == NULL)                                                   \
        {                                                                       \
            fputs("Failed to resize dynamic array", stderr);                    \
            exit(EXIT_FAILURE);                                                 \
        }                                                                       \
        arr->data = new_data;                                                   \
        arr->cap = new_cap;                                                     \
    }                                                                           \
    size_t push_dynamic_array_##type(Dynamic_Array_##type *arr, type value)     \
    {                                                                           \
        if (arr->num_used == arr->cap)                                          \
        {                                                                       \
            resize_dynamic_array_##type(arr, arr->cap * 2);                     \
        }                                                                       \
        arr->data[arr->num_used] = value;                                       \
        return arr->num_used++;                                                 \
    }                                                                           \
    type pop_dynamic_array_##type(Dynamic_Array_##type *arr)                    \
    {                                                                           \
        if (arr->num_used == 0)                                                 \
        {                                                                       \
            fputs("Attempted to pop from an empty dynamic array", stderr);      \
            exit(EXIT_FAILURE);                                                 \
        }                                                                       \
        type value = arr->data[--arr->num_used];                                \
        if (arr->num_used <= arr->cap / 4)                                      \
        {                                                                       \
            resize_dynamic_array_##type(arr, arr->cap / 2);                     \
        }                                                                       \
        return value;                                                           \
    }                                                                           \
    void free_dynamic_array_##type(Dynamic_Array_##type *arr)                   \
    {                                                                           \
        free(arr->data);                                                        \
        arr->data = NULL;                                                       \
        arr->cap = 0;                                                           \
        arr->num_used = 0;                                                      \
    }
