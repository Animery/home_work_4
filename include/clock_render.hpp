#pragma once

#include "../include/triangle_interpolated_render.hpp"

namespace gfx
{

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

struct grayscale : program
{
    color fragment_shader(const vertex& v_in) override
    {
        color out;

        float tex_x = v_in.x_tex; // 0..1
        float tex_y = v_in.y_tex; // 0..1

        canvas* texture0 = uniforms_.texture0;

        size_t tex_width  = texture0->getWidth();
        size_t tex_height = texture0->getHeight();

        size_t t_x = static_cast<size_t>((tex_width - 1) * tex_x);
        size_t t_y = static_cast<size_t>((tex_height - 1) * tex_y);

        out = texture0->get_pixel(t_x, t_y);

        uint8_t gray = static_cast<uint8_t>(0.2125 * out.r + 0.7152 * out.g +
                                            0.0721 * out.b);

        out.r = gray;
        out.g = gray;
        out.b = gray;

        return out;
    }
};

struct rotate_image : program
{
    vertex vertex_shader(const vertex& v_in) override
    {
        vertex out = v_in;

        const double scale = uniforms_.f4;

        out.x -= (uniforms_.f5 * 0.5);
        out.y -= (uniforms_.f6 * 0.5);

        out.x *= scale;
        out.y *= scale;

        // rotate
        double alpha = (3.14159) * (uniforms_.f7 / 30) * 1;
        double x     = out.x;
        double y     = out.y;
        out.x        = x * std::cos(alpha) - y * std::sin(alpha);
        out.y        = x * std::sin(alpha) + y * std::cos(alpha);

        out.x += (uniforms_.f5 * 0.5);
        out.y += (uniforms_.f6 * 0.5);

        return out;
    }

    color from_circle(const vertex& sample)
    {
        uint32_t u = static_cast<uint32_t>(std::abs(std::round(sample.x)));
        uint32_t v = static_cast<uint32_t>(std::abs(std::round(sample.y)));
        color out = uniforms_.texture1->get_pixel(u, v);

        return out;
    }

    color fragment_shader(const vertex& v_in) override
    {
        color out;
        color out1 = from_circle(v_in);

        float tex_x = v_in.x_tex; // 0...1
        float tex_y = v_in.y_tex; // 0...1

        canvas* texture = uniforms_.texture0;

        size_t tex_width  = texture->getWidth();
        size_t tex_height = texture->getHeight();

        size_t t_x = static_cast<size_t>((tex_width - 1) * tex_x);
        size_t t_y = static_cast<size_t>((tex_height - 1) * tex_y);

        out = texture->get_pixel(t_x, t_y);

        if (out == color{ 0, 0, 0 })
        {
            out = out1;
        }
        return out;
    }
};


} // namespace gfx