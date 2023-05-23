add_library(Triangle triangle.c)
target_compile_definitions(Triangle PUBLIC TRILIBRARY CDT_ONLY REDUCED SINGLE NO_TIMER)
target_compile_features(Triangle PRIVATE c_std_99)
target_include_directories(Triangle PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
