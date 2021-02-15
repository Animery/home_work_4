#include "../include/triangle_interpolated_render.hpp"

#include <SDL2/SDL.h>

#include <cmath>
#include <cstdlib>

#include <algorithm>
#include <chrono>
#include <iostream>

int main(int, char**)
{
    using namespace std;

    if (0 != SDL_Init(SDL_INIT_EVERYTHING))
    {
        cerr << SDL_GetError() << endl;
        return EXIT_FAILURE;
    }

    constexpr uint16_t width  = 320*4;
    constexpr uint16_t height = 240*4;

    SDL_Window* window = SDL_CreateWindow("runtime soft render",
                                          SDL_WINDOWPOS_CENTERED,
                                          SDL_WINDOWPOS_CENTERED,
                                          width,
                                          height,
                                          SDL_WINDOW_OPENGL);
    if (window == nullptr)
    {
        cerr << SDL_GetError() << endl;
        return EXIT_FAILURE;
    }

    SDL_Renderer* renderer =
        SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (renderer == nullptr)
    {
        cerr << SDL_GetError() << endl;
        return EXIT_FAILURE;
    }

    const color black = { 0, 0, 0 };

    canvas texture(0, 0);
    texture.load_image("Star.ppm");

    canvas image(width, height);

    triangle_interpolated interpolated_render(image);

    struct program : gfx_program
    {
        uniforms uniforms_;
        void     set_uniforms(const uniforms& a_uniforms) override
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

            float tex_x = v_in.x_tex; // 0..1
            float tex_y = v_in.y_tex; // 0..1

            canvas* texture = uniforms_.texture0;

            size_t tex_width  = texture->getWidth();
            size_t tex_height = texture->getHeight();

            size_t t_x = static_cast<size_t>((tex_width - 1) * tex_x);
            size_t t_y = static_cast<size_t>((tex_height - 1) * tex_y);

            out = texture->get_pixel(t_x, t_y);

            return out;
        }
    } program01;

    struct grayscale : program
    {
        color fragment_shader(const vertex& v_in) override
        {
            color out;

            float tex_x = v_in.x_tex; // 0..1
            float tex_y = v_in.y_tex; // 0..1

            canvas* texture = uniforms_.texture0;

            size_t tex_width  = texture->getWidth();
            size_t tex_height = texture->getHeight();

            size_t t_x = static_cast<size_t>((tex_width - 1) * tex_x);
            size_t t_y = static_cast<size_t>((tex_height - 1) * tex_y);

            out = texture->get_pixel(t_x, t_y);

            uint8_t gray = static_cast<uint8_t>(
                0.2125 * out.r + 0.7152 * out.g + 0.0721 * out.b);

            out.r = gray;
            out.g = gray;
            out.b = gray;

            return out;
        }
    } program02;

    struct rotate_image : program
    {
        vertex vertex_shader(const vertex& v_in) override
        {
            vertex out = v_in;

            const double scale = 0.5;

            out.x -= (uniforms_.f5 * scale);
            out.y -= (uniforms_.f6 * scale);

            out.x *= scale;
            out.y *= scale;

            // rotate
            double alpha = (3.14159 / 2) * uniforms_.f7 * 1;
            double x     = out.x;
            double y     = out.y;
            out.x        = x * std::cos(alpha) - y * std::sin(alpha);
            out.y        = x * std::sin(alpha) + y * std::cos(alpha);

            out.x += (uniforms_.f5 * scale);
            out.y += (uniforms_.f6 * scale);

            return out;
        }
    } program03;

    std::array<gfx_program*, 3> programs{ &program01, &program02, &program03 };
    size_t                      current_program_index = 0;
    gfx_program* current_program = programs.at(current_program_index);

    const uint16_t w = width;
    const uint16_t h = height;

    // clang-format off
    //                                x  y          r  g  b  tx ty
    std::vector<vertex> triangle_v{ { 0, 0,         1, 1, 1, 0, 0, 0 },
                                    { w - 1, h - 1, 1, 1, 1, 1, 1, 0 },
                                    { 0, h - 1,     1, 1, 1, 0, 1, 0 },
                                    { w - 1, 0,     1, 1, 1, 1, 0, 0 } };
    // clang-format on
    std::vector<uint16_t> indexes_v{ { 0, 1, 2, 0, 3, 1 } };

    void*     pixels = image.data();
    const int depth  = sizeof(color) * 8;
    const int pitch  = width * sizeof(color);
    const int rmask  = 0x000000ff;
    const int gmask  = 0x0000ff00;
    const int bmask  = 0x00ff0000;
    const int amask  = 0;

    interpolated_render.set_gfx_program(*current_program);

    bool continue_loop = true;

    while (continue_loop)
    {
        SDL_Event e;
        while (SDL_PollEvent(&e))
        {
            if (e.type == SDL_QUIT)
            {
                continue_loop = false;
                break;
            }
            else if (e.type == SDL_KEYUP)
            {
                current_program_index =
                    (current_program_index + 1) % programs.size();
                current_program = programs.at(current_program_index);

                interpolated_render.set_gfx_program(*current_program);
            }
            else if (e.type == SDL_MOUSEMOTION)
            {
            }
            else if (e.type == SDL_MOUSEWHEEL)
            {
            }
        }

        interpolated_render.clear(black);
        double       time_from_start = SDL_GetTicks() / 1000.0;
        const double w_in_ren        = image.getWidth();
        const double h_in_ren        = image.getHeight();
        current_program->set_uniforms(uniforms{
            0, 0, 0, 0, 0, w_in_ren, h_in_ren, time_from_start, &texture });

        interpolated_render.draw_triangles(triangle_v, indexes_v);

        SDL_Surface* bitmapSurface = SDL_CreateRGBSurfaceFrom(
            pixels, width, height, depth, pitch, rmask, gmask, bmask, amask);
        if (bitmapSurface == nullptr)
        {
            cerr << SDL_GetError() << endl;
            return EXIT_FAILURE;
        }
        SDL_Texture* bitmapTex =
            SDL_CreateTextureFromSurface(renderer, bitmapSurface);
        if (bitmapTex == nullptr)
        {
            cerr << SDL_GetError() << endl;
            return EXIT_FAILURE;
        }
        SDL_FreeSurface(bitmapSurface);

        SDL_RenderClear(renderer);
        SDL_RenderCopy(renderer, bitmapTex, nullptr, nullptr);
        SDL_RenderPresent(renderer);

        SDL_DestroyTexture(bitmapTex);
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);

    SDL_Quit();

    return EXIT_SUCCESS;
}
