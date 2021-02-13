#include "../include/canvas.hpp"
#include <chrono>
#include <iostream>

int main(int, char**)
{

    auto        begin_time = std::chrono::high_resolution_clock::now();

    const color green      = { 0, 255, 0 };

    constexpr uint16_t width      = 320 * 2;
    constexpr uint16_t height     = 240 * 2;

    canvas image(width, height);
    std::fill(image.begin(), image.end(), green);
    const char* file_name = "00_green_image.ppm";
    image.save_image(file_name);

    canvas image_loaded(0,0);
    image_loaded.load_image(file_name);

    if (image != image_loaded)
    {
        std::cerr << "canvas: image != image_loaded\n";
        return 1;
    }
    else
    {
        std::cout << "canvas: image == image_loaded\n";
    }

    auto end_time = std::chrono::high_resolution_clock::now();
    std::cout << "canvas time: "
              << std::chrono::duration_cast<std::chrono::microseconds>(
                     end_time - begin_time)
                     .count()
              << " microseconds\n"
              << std::endl;

    return 0;
}