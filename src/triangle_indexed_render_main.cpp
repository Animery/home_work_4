#include "../include/triangle_indexed_render.hpp"
#include <chrono>
#include <iostream>

int main(int, char**)
{
    auto        begin_time = std::chrono::high_resolution_clock::now();
    const color black = { 0, 0, 0 };
    const color green = { 0, 255, 0 };

    constexpr uint16_t width      = 320 * 2;
    constexpr uint16_t height     = 240 * 2;

    canvas image(width, height);

    std::vector<position> vertex_buffer;

    size_t max_x = 10;
    size_t max_y = 10;

    int32_t step_x = (width - 1) / max_x;
    int32_t step_y = (height - 1) / max_y;

    for (size_t i = 0; i <= max_x; ++i)
    {
        for (size_t j = 0; j <= max_y; ++j)
        {
            position v{ static_cast<int>(j) * step_x,
                        static_cast<int>(i) * step_y };

            vertex_buffer.push_back(v);
        }
    }

    assert(vertex_buffer.size() == (max_x + 1) * (max_y + 1));

    std::vector<uint16_t> index_buffer;

    for (size_t x = 0; x < max_x; ++x)
    {
        for (size_t y = 0; y < max_y; ++y)
        {
            uint8_t index0 = static_cast<uint8_t>(y * (max_y + 1) + x);
            uint8_t index1 = static_cast<uint8_t>(index0 + (max_y + 1) + 1);
            uint8_t index2 = index1 - 1;
            uint8_t index3 = index0 + 1;

            index_buffer.push_back(index0);
            index_buffer.push_back(index1);
            index_buffer.push_back(index2);

            index_buffer.push_back(index0);
            index_buffer.push_back(index3);
            index_buffer.push_back(index1);
        }
    }

    triangle_indexed_render indexed_render(image);
    indexed_render.clear(black);

    indexed_render.draw_triangles(vertex_buffer, index_buffer, green);

    image.save_image("04_triangles_indexes.ppm");

    canvas test_image(0,0);
    test_image.load_image("04_triangles_indexes.ppm");

    if (image != test_image)
    {
        std::cerr << "triangle_indexed: image != image_loaded\n";
        return 1;
    }
    else
    {
        std::cout << "triangle_indexed: image == image_loaded\n";
    }

    assert(image == test_image);

    auto end_time = std::chrono::high_resolution_clock::now();
    std::cout << "triangle_indexed time: "
              << std::chrono::duration_cast<std::chrono::microseconds>(end_time -
                                                                       begin_time)
                     .count()
              << " microseconds\n" << std::endl;

    return 0;
}