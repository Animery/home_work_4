cmake_minimum_required(VERSION 3.17)
project(new_project CXX)

add_executable(1_canvas src/canvas_main.cpp
                        src/canvas.cpp
                        include/canvas.hpp)
target_compile_features(1_canvas PUBLIC cxx_std_17)

add_executable(2_line_render    src/line_render.cpp
                                include/line_render.hpp
                                src/line_render_main.cpp
                                src/canvas.cpp)
target_compile_features(2_line_render PUBLIC cxx_std_17)

add_executable(3_triangle_render        src/triangle_render.cpp
                                        include/triangle_render.hpp
                                        src/triangle_render_main.cpp
                                        src/canvas.cpp
                                        src/line_render.cpp
                                        )
target_compile_features(3_triangle_render PUBLIC cxx_std_17)

add_executable(4_triangle_indexed       src/triangle_indexed_render.cpp
                                        include/triangle_indexed_render.hpp
                                        src/triangle_indexed_render_main.cpp
                                        src/canvas.cpp
                                        src/line_render.cpp
                                        src/triangle_render.cpp
                                        )
target_compile_features(4_triangle_indexed PUBLIC cxx_std_17)

add_executable(5_triangle-interpolated  src/triangle_interpolated_render.cpp
                                        include/triangle_interpolated_render.hpp
                                        src/triangle_interpolated_render_main.cpp
                                        src/canvas.cpp
                                        src/line_render.cpp
                                        src/triangle_render.cpp
                                        src/triangle_indexed_render.cpp
                                        )
target_compile_features(5_triangle-interpolated PUBLIC cxx_std_17)

find_package(SDL2)

add_executable(basic-windowed   src/render_basic.cpp
                                src/triangle_interpolated_render.cpp
                                include/triangle_interpolated_render.hpp
                                src/canvas.cpp
                                src/line_render.cpp
                                src/triangle_render.cpp
                                src/triangle_indexed_render.cpp
                                )

target_link_libraries(basic-windowed PRIVATE SDL2::SDL2-static SDL2::SDL2main)
target_compile_features(basic-windowed PUBLIC cxx_std_17)

install(TARGETS 1_canvas #2_line_render 3_triangle_render 4_triangle_indexed 5_triangle-interpolated basic-windowed
        RUNTIME DESTINATION ${CMAKE_CURRENT_LIST_DIR}/bin
        LIBRARY DESTINATION ${CMAKE_CURRENT_LIST_DIR}/lib
        ARCHIVE DESTINATION ${CMAKE_CURRENT_LIST_DIR}/lib)