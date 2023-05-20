cmake_minimum_required(VERSION 3.16)
project(meshproc)

add_library(meshproc INTERFACE)
target_include_directories(meshproc INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})
target_compile_features(meshproc INTERFACE c_std_11)
set_target_properties(meshproc PROPERTIES C_EXTENSIONS OFF)

find_library(LIB_MATH m)

if(LIB_MATH)
    message(STATUS "Found libm: ${LIB_MATH}")
    target_link_libraries(meshproc INTERFACE ${LIB_MATH})
endif()

add_executable(render_triangle render_triangle.c)
target_link_libraries(render_triangle PRIVATE meshproc)
