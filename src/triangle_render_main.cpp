#include "../include/triangle_render.hpp"
#include <chrono>
#include <iostream>

int main(int, char**)
{
    auto        begin_time = std::chrono::high_resolution_clock::now();
    const color black = { 0, 0, 0 };
    const color green = { 0, 255, 0 };

    canvas image;

    triangle_render render_tri(image, width, height);
    render_tri.clear(black);

    render_tri.draw_triangle(position{ 0, 0 },
                             position{ width - 1, height - 1 },
                             position{ 0, height - 1 },
                             green);

    image.save_image("02_triangle.ppm");

    render_tri.clear(black);

    size_t max_x = 10;
    size_t max_y = 10;

    for (size_t i = 0; i < max_x; i++)
    {
        for (size_t j = 0; j < max_y; j++)
        {
            int32_t step_x = (width - 1) / max_x;
            int32_t step_y = (height - 1) / max_y;

            position v0{ 0 + static_cast<int>(i) * step_x,
                         0 + static_cast<int>(j) * step_y };
            position v1{ v0.x + step_x, v0.y + step_y };
            position v2{ v0.x, v0.y + step_y };
            position v3{ v0.x + step_x, v0.y };

            render_tri.draw_triangle(v0, v1, v2, green);
            render_tri.draw_triangle(v0, v3, v1, green);
        }
    }
    image.save_image("03_triangle_x.ppm");

    canvas test_image;
    test_image.load_image("03_triangle_x.ppm");

    if (image != test_image)
    {
        std::cerr << "triangle: image != image_loaded\n";
        return 1;
    }
    else
    {
        std::cout << "triangle: image == image_loaded\n";
    }

    auto end_time = std::chrono::high_resolution_clock::now();
    std::cout << "triangle: time: "
              << std::chrono::duration_cast<std::chrono::microseconds>(end_time -
                                                                       begin_time)
                     .count()
              << " microseconds\n" << std::endl;

    return 0;
}
