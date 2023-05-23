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

if(MSVC)
# TODO
else()
    set_target_properties(meshproc PROPERTIES
        CMAKE_C_FLAGS_DEBUG
        "${CMAKE_C_FLAGS_DEBUG} -Werror -fsanitize=undefined -fsanitize=address"
    )
    target_link_options(meshproc
        BEFORE INTERFACE -fsanitize=undefined INTERFACE -fsanitize=address
    )
endif()

if(MSVC)
    target_compile_options(meshproc INTERFACE "/openmp:llvm")
else()
    find_package(OpenMP REQUIRED)
    target_link_libraries(meshproc INTERFACE OpenMP::OpenMP_C)
endif()

add_subdirectory(triangle)
target_link_libraries(meshproc INTERFACE Triangle)

add_executable(render_mesh render_mesh.c)
target_link_libraries(render_mesh PRIVATE meshproc)

add_executable(self_intersection self_intersection.c)
target_link_libraries(self_intersection PRIVATE meshproc)
