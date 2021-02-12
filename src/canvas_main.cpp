#include "../include/canvas.hpp"
#include <iostream>
#include <chrono>

int main(int, char**)
{
    auto        begin_time = std::chrono::high_resolution_clock::now();
    const color green = { 0, 255, 0 };

    canvas image;

    std::fill(begin(image), end(image), green);

    const char* file_name = "00_green_image.ppm";

    image.save_image(file_name);

    canvas image_loaded;
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
              << std::chrono::duration_cast<std::chrono::microseconds>(end_time -
                                                                       begin_time)
                     .count()
              << " microseconds\n" << std::endl;

    return 0;
}