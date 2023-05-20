#pragma once

#define STRUCT(name)          \
    typedef struct name name; \
    struct name

#define UNION(name)          \
    typedef union name name; \
    union name

#define ENUM(name)          \
    typedef enum name name; \
    enum name
