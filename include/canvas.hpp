#pragma once

// #include <array>
#include <cassert>
#include <cmath>
#include <cstddef>
#include <fstream>
#include <vector>
// #include <iostream>
// #include <iomanip>


#pragma pack(push, 1)
/*
uint8_t     r = 0;
uint8_t     g = 0;
uint8_t     b = 0;*/
struct color
{
    uint8_t     r = 0;
    uint8_t     g = 0;
    uint8_t     b = 0;
    friend bool operator==(const color& l, const color& r);
};
#pragma pack(pop)

// const size_t buffer_size = width * height;

class canvas : public std::vector<color>
{
public:
    // canvas(){};
    explicit canvas(uint16_t width_, uint16_t height_)
        : width{ width_ }
        , height{ height_ }
    {
        resize(width_ * height_);
    }
    /// PPM - image format:
    /// https://ru.wikipedia.org/wiki/Portable_anymap#%D0%9F%D1%80%D0%B8%D0%BC%D0%B5%D1%80_PPM
    void save_image(const std::string& file_name)
    {
        std::ofstream out_file(file_name, std::ios_base::binary);
        out_file.exceptions(std::ios_base::failbit);
        out_file << "P6\n" << width << ' ' << height << ' ' << 255 << '\n';
        std::streamsize buf_size =
            static_cast<std::streamsize>(sizeof(color) * size());
        out_file.write(reinterpret_cast<const char*>(data()), buf_size);
    }

    void load_image(const std::string& file_name)
    {
        std::ifstream in_file(file_name, std::ios_base::binary);
        in_file.exceptions(std::ios_base::failbit);
        std::string header;
        uint16_t      image_width  = 0;
        uint16_t      image_height = 0;
        uint16_t      color_format = 0;

        // char last_next_line = 0;
        // in_file >> header >> image_width >> image_height >> color_format >>
        //     last_next_line;

        in_file >> header >> image_width >> image_height >> color_format >>
            std::ws;
        resize(image_width * image_height);
        std::streamsize buf_size =
            static_cast<std::streamsize>(sizeof(color) * size());
        in_file.read(reinterpret_cast<char*>(data()), buf_size);
    }
    uint16_t getWidth() { return width; }
    uint16_t getHeight() { return height; }

private:
    uint16_t width  = 0;
    uint16_t height = 0;
};

/*int32_t         x = 0;
  int32_t         y = 0;*/
struct position
{
    double          length() { return std::sqrt(x * x + y * y); }
    friend position operator-(const position& left, const position& right);
    friend bool     operator==(const position& left, const position& right);
    int32_t         x = 0;
    int32_t         y = 0;
};

using pixels = std::vector<position>;

struct irender
{
    virtual void clear(color) = 0;
    virtual ~irender();

protected:
    virtual void   draw_pix(pixels&, color)                       = 0;
    virtual void   set_pixel(position, color)                     = 0;
    virtual pixels pixels_positions(position start, position end) = 0;

private:
};