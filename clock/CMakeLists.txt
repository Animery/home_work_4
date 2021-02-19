cmake_minimum_required(VERSION 3.17)
project(clock CXX)


add_executable(clock_render     clock_render.cpp
                                arrow.cpp
                                arrow.hpp
                                gfx.hpp
                                ../src/triangle_interpolated_render.cpp
                                ../include/triangle_interpolated_render.hpp
                                ../src/canvas.cpp
                                ../src/line_render.cpp
                                ../src/triangle_render.cpp
                                ../src/triangle_indexed_render.cpp
                                )


find_package(SDL2)
target_link_libraries(clock_render PRIVATE SDL2::SDL2-static SDL2::SDL2main)
target_compile_features(clock_render PUBLIC cxx_std_17)