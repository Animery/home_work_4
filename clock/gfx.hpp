#pragma once

#ifndef __GFX_H__
#define __GFX_H__

#include <array>

#include "../include/triangle_interpolated_render.hpp"

namespace gfx
{

/*double  f0       = 0;       // number of divisions
  double  f1       = 0;       // time_from_start
  double  f2       = 0;       // scale
  double  f3       = 0;       // width  Image
  double  f4       = 0;       // height Image
  double  f5       = 0;
  double  f6       = 0;
  double  f7       = 0;*/    
struct my_uniforms : uniforms
{
    double  f0       = 0;       // number of divisions
    double  f1       = 0;       // time_from_start
    double  f2       = 0;       // scale
    double  f3       = 0;       // width  Image
    double  f4       = 0;       // height Image
    double  f5       = 0;
    double  f6       = 0;
    double  f7       = 0;
};


struct program : gfx_program
{
    uniforms uniforms_;

    void set_uniforms(const uniforms& a_uniforms) override
    {
        uniforms_ = a_uniforms;
    }
    vertex vertex_shader(const vertex& v_in) override
    {
        vertex out = v_in;
        return out;
    }

    color fragment_shader(const vertex& v_in) override
    {
        color out;
        out.r = static_cast<uint8_t>(v_in.r * 255);
        out.g = static_cast<uint8_t>(v_in.g * 255);
        out.b = static_cast<uint8_t>(v_in.b * 255);
        return out;
    }
};

struct rotate_arrow : program
{  

    vertex vertex_shader(const vertex& v_in) override
    {
        vertex out = v_in;

        const double scale = uniforms_.f2;

        out.x -= (uniforms_.f3 * 0.5);
        out.y -= (uniforms_.f4 * 0.5);

        out.x *= scale;
        out.y *= scale;

        // rotate
        double alpha = (3.14159) * (uniforms_.f1 / uniforms_.f0) * 2;
        double x     = out.x;
        double y     = out.y;
        out.x        = x * std::cos(alpha) - y * std::sin(alpha);
        out.y        = x * std::sin(alpha) + y * std::cos(alpha);

        out.x += (uniforms_.f3 * 0.5);
        out.y += (uniforms_.f4 * 0.5);

        return out;
    }
};

struct refresh_clock : program
{
    std::array<canvas*, 4> canvas_arr{ nullptr, nullptr, nullptr, nullptr };

    void set_texture(canvas* board_  = nullptr,
                     canvas* second_ = nullptr,
                     canvas* minute_ = nullptr,
                     canvas* hour_   = nullptr)
    {
        canvas_arr[0] = board_;
        canvas_arr[1] = second_;
        canvas_arr[2] = minute_;
        canvas_arr[3] = hour_;
    }

    color fragment_shader(const vertex& v_in) override
    {
        color out;
        out.r = static_cast<uint8_t>(v_in.r * 255);
        out.g = static_cast<uint8_t>(v_in.g * 255);
        out.b = static_cast<uint8_t>(v_in.b * 255);

        for (const auto& it : canvas_arr)
        {
            if (it != nullptr)
            {
                color check = from_texture(v_in, it);
                if (check != color{ 0, 0, 0 })
                {
                    out = check;
                    break;
                }
            }
        }
        return out;
    }

private:
    color from_texture(const vertex& sample, canvas* texture)
    {
        uint32_t x   = static_cast<uint32_t>(std::abs(std::round(sample.x)));
        uint32_t y   = static_cast<uint32_t>(std::abs(std::round(sample.y)));
        color    out = texture->get_pixel(x, y);

        return out;
    }
};

} // namespace gfx

#endif // __GFX_H__