#include "../include/line_render.hpp"
#include <chrono>
#include <iostream>

int main(int, char**)
{
    auto        begin_time = std::chrono::high_resolution_clock::now();
    const color black = { 0, 0, 0 };
    const color white = { 255, 255, 255 };
    const color green = { 0, 255, 0 };

    canvas image;

    line_render render(image, width, height);

    render.clear(black);

    render.draw_line(
        position{ 0, 0 }, position{ height - 1, width - 1 }, white);

    render.draw_line(
        position{ 0, height - 1 }, position{ width - 1, 0 }, green);

    render.draw_circle(position{ 50, 50 }, 100, green);

    for (size_t i = 0; i < 100; ++i)
    {
        position start{ rand() % static_cast<int>(width),
                        rand() % static_cast<int>(height) };
        position end{ rand() % static_cast<int>(width),
                      rand() % static_cast<int>(height) };
        color    color{ static_cast<uint8_t>(rand() % 256),
                     static_cast<uint8_t>(rand() % 256),
                     static_cast<uint8_t>(rand() % 256) };
        render.draw_line(start, end, color);
    }

    image.save_image("01_lines.ppm");

    canvas test_image;
    test_image.load_image("01_lines.ppm");

    if (image != test_image)
    {
        std::cerr << "line: image != image_loaded\n";
        return 1;
    }
    else
    {
        std::cout << "line: image == image_loaded\n";
    }

    auto end_time = std::chrono::high_resolution_clock::now();
    std::cout << "line time: "
              << std::chrono::duration_cast<std::chrono::microseconds>(end_time -
                                                                       begin_time)
                     .count()
              << " microseconds\n" << std::endl;

    return 0;
}
