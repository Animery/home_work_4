#include "../include/triangle_interpolated_render.hpp"

#include <chrono>
#include <iostream>

int main(int, char**)
{
    auto        begin_time = std::chrono::high_resolution_clock::now();
    const color black      = { 0, 0, 0 };

    constexpr uint16_t width  = 320 * 2;
    constexpr uint16_t height = 240 * 2;

    canvas image(width, height);

    triangle_interpolated interpolated_render(image);

    struct program : gfx_program
    {
        uint16_t width{};
        uint16_t height{};

        void set_uniforms(const uniforms& a_uniforms) override
        {
            width  = static_cast<uint16_t>(a_uniforms.f0);
            height = static_cast<uint16_t>(a_uniforms.f1);
        }
        vertex vertex_shader(const vertex& v_in) override
        {
            vertex out = v_in;

            // // rotate
            // double alpha = 3.14159; // 180 degree
            // double x     = out.x;
            // double y     = out.y;
            // out.x        = x * std::cos(alpha) - y * std::sin(alpha);
            // out.y        = x * std::sin(alpha) + y * std::cos(alpha);

            // // scale into 3 times
            // // out.x *= 0.5;
            // // out.y *= 0.5;

            // // move
            // out.x += (width / 1) - (width/2);
            // out.y += (height / 1);

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
    } program01;

    program01.set_uniforms(uniforms{ static_cast<double>(image.getWidth()),
                                     static_cast<double>(image.getHeight()) });

    // drow round
    line_render render(image);

    render.clear(black);

    render.draw_circle(position{ width / 2, height / 2 }, 50, color{ 0, 0, 255 });

    image.save_image("05_round.ppm");

    // drow Circle
    interpolated_render.clear(black);
    interpolated_render.set_gfx_program(program01);

    vertexMap triangle_v{
        { width / 2 - 1, 0, 1, 0, 0, width / 2 - 1, 0, 0 },
        { 0, height / 2 - 1, 0, 1, 0, 0, height / 2 - 1, 0 },
        { width / 2 - 1, height - 1, 0, 0, 1, width / 2 - 1, height - 1, 0 }
    };
    std::vector<uint16_t> indexes_tri_v{ 0, 1, 2 };

    vertexMap             circle_v{ { width / 2, height / 2, 1, 0, 0, 1, 1, 0 },
                        { width / 2, height / 2 - 50, 0, 0, 1, 1, 1, 0 } };
    std::vector<uint16_t> indexes_circle_v{ 0, 1 };
    interpolated_render.draw_circle(circle_v, indexes_circle_v);
    // interpolated_render.draw_triangles(triangle_v, indexes_tri_v);
    image.save_image("05_interpolated.ppm");

    canvas                texture(width, height);
    triangle_interpolated arrow(texture);
    // clang-format off
    const double w_tex = texture.getWidth();
    const double h_tex = texture.getHeight();
    //                    x                         y              r  g  b  tx     ty
    vertexMap arrow_vM{ { w_tex / 2 - 1,            h_tex / 2 - 1, 0, 0, 1, 1/2,   1/2, 0 },
                        { w_tex / 2 - 1 + w_tex/64, h_tex / 3,     1, 0, 0, 33/64, 1/3, 0 },
                        { w_tex / 2 - 1 - w_tex/64, h_tex / 3,     1, 0, 0, 31/64, 1/3, 0 },
                        { w_tex / 2 - 1,            h_tex / 6,     0, 0, 1, 1/2,   1/6, 0 } };
    // clang-format on
    std::vector<uint16_t> indexes_arrow_v{ { 0, 1, 2, 1, 2, 3 } };
    arrow.clear(black);
    arrow.set_gfx_program(program01);
    arrow.draw_triangles(arrow_vM, indexes_arrow_v);
    texture.save_image("arrow.ppm");


    // texture example
    struct program_tex : gfx_program
    {
        std::vector<color> texture;
        uint16_t           width{};
        uint16_t           height{};

        void set_uniforms(const uniforms& a_uniforms) override
        {
            width  = static_cast<uint16_t>(a_uniforms.f0);
            height = static_cast<uint16_t>(a_uniforms.f1);
        }
        vertex vertex_shader(const vertex& v_in) override
        {
            vertex out = v_in;

            out.x += width / 4;

            return out;
        }
        color fragment_shader(const vertex& v_in) override
        {
            color out;

            out.r = static_cast<uint8_t>(v_in.r * 255);
            out.g = static_cast<uint8_t>(v_in.g * 255);
            out.b = static_cast<uint8_t>(v_in.b * 255);

            color from_texture = sample2d(v_in);
            out.r              = from_texture.r;
            out.g              = from_texture.g;
            out.b              = from_texture.b;
            return out;
        }

        void set_texture(const std::vector<color>& tex) { texture = tex; }

        color sample2d(const vertex& sample)
        {
            uint32_t u = static_cast<uint32_t>(std::round(sample.x));
            uint32_t v = static_cast<uint32_t>(std::round(sample.y));

            color c = texture.at(v * width + u);
            return c;
        }
    } program02;
    program02.set_uniforms(uniforms{ static_cast<double>(image.getWidth()),
                                     static_cast<double>(image.getHeight()) });
    program02.set_texture(image);

    interpolated_render.set_gfx_program(program02);

    interpolated_render.clear(black);

    interpolated_render.draw_triangles(triangle_v, indexes_tri_v);

    image.save_image("06_textured_triangle.ppm");

    canvas test_image(0, 0);
    test_image.load_image("06_textured_triangle.ppm");

    if (image != test_image)
    {
        std::cerr << "triangle_interpolated: image != image_loaded\n";
        return 1;
    }
    else
    {
        std::cout << "triangle_interpolated: image == image_loaded\n";
    }

    auto end_time = std::chrono::high_resolution_clock::now();
    std::cout << "triangle_interpolated time: "
              << std::chrono::duration_cast<std::chrono::microseconds>(
                     end_time - begin_time)
                     .count()
              << " microseconds\n"
              << std::endl;

    return 0;
}
