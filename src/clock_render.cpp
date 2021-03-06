// #include "../include/triangle_interpolated_render.hpp"
#include "../include/clock_render.hpp"

#include <SDL2/SDL.h>

#include <algorithm>
#include <chrono>
#include <cmath>
#include <cstdlib>
#include <ctime>
#include <iostream>

int main(int, char**)
{
    constexpr uint16_t width        = 320 * 4;
    constexpr uint16_t height       = 240 * 4;
    constexpr size_t   refresh_rate = 1000 / 4;

    // using namespace std;

    if (0 != SDL_Init(SDL_INIT_EVERYTHING))
    {
        std::cerr << SDL_GetError() << std::endl;
        return EXIT_FAILURE;
    }

    SDL_Window* window = SDL_CreateWindow("runtime soft render",
                                          SDL_WINDOWPOS_CENTERED,
                                          SDL_WINDOWPOS_CENTERED,
                                          width,
                                          height,
                                          SDL_WINDOW_OPENGL);
    if (window == nullptr)
    {
        std::cerr << SDL_GetError() << std::endl;
        return EXIT_FAILURE;
    }

    SDL_Renderer* renderer =
        SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (renderer == nullptr)
    {
        std::cerr << SDL_GetError() << std::endl;
        return EXIT_FAILURE;
    }

    const color black = { 0, 0, 0 };
    gfx::program   gfx_00;

    // TEXTURE arrow
    canvas                texture_arrow(width, height);
    triangle_interpolated arrow(texture_arrow);

    // clang-format off
    const double w_arrow = texture_arrow.getWidth();
    const double h_arrow = texture_arrow.getHeight();
    //                    x                       y                r  g  b  tx     ty
    vertexMap arrow_vM{ { w_arrow / 2 - 1,        h_arrow / 2 - 1, 0, 0, 1, 1/2,   1/2, 0 },
                        { (33 * w_arrow)/64 - 1 , h_arrow / 3,     1, 0, 0, 33/64, 1/3, 0 },
                        { (31 * w_arrow)/64 - 1,  h_arrow / 3,     1, 0, 0, 31/64, 1/3, 0 },
                        { w_arrow / 2 - 1,        h_arrow / 8,     0, 0, 1, 1/2,   1/6, 0 } };
    // clang-format on
    std::vector<uint16_t> indexes_arrow_v{ { 0, 1, 2, 1, 2, 3 } };
    arrow.clear(black);
    arrow.set_gfx_program(gfx_00);
    arrow.draw_triangles(arrow_vM, indexes_arrow_v);
    // TEXTURE

    // TEXTURE circle
    canvas                texture_cirle(width, height);
    triangle_interpolated circle(texture_cirle);

    // clang-format off
    const double w_circle = texture_cirle.getWidth();
    const double h_circle = texture_cirle.getHeight();
    //                    x                  y                 r  g  b  tx   ty
    vertexMap circle_vM{ { w_circle / 2 - 1, h_circle / 2 - 1, 1, 0, 0, 1/2, 1/2, 0 },
                         { w_circle / 2 - 1, h_circle / 6,     0, 0, 1, 1/2, 1/6, 0 } };
    // clang-format on
    std::vector<uint16_t> indexes_circle_v{ { 0, 1 } };
    circle.clear(black);
    circle.set_gfx_program(gfx_00);
    circle.draw_circle(circle_vM, indexes_circle_v, Flagcircle::ROUND);
    // TEXTURE

    canvas                image(width, height);
    triangle_interpolated board(image);
    // clang-format off
    //                              x          y           r  g  b  tx ty
    std::vector<vertex> board_vM{ { 0,         0,          1, 1, 1, 0, 0, 0 },
                                  { width - 1, height - 1, 1, 1, 1, 1, 1, 0 },
                                  { 0,         height - 1, 1, 1, 1, 0, 1, 0 },
                                  { width - 1, 0,          1, 1, 1, 1, 0, 0 } };
    // clang-format on
    std::vector<uint16_t> indexes_board_v{ { 0, 1, 2, 0, 3, 1 } };

    // gfx::program      program01;
    gfx::grayscale    gfx_01;
    gfx::rotate_image gfx_02;

    std::array<gfx_program*, 2> programs{ &gfx_01, &gfx_02 };
    size_t                      current_program_index = 1;
    gfx_program* current_program = programs.at(current_program_index);

    void*     pixels = image.data();
    const int depth  = sizeof(color) * 8;
    const int pitch  = width * sizeof(color);
    const int rmask  = 0x000000ff;
    const int gmask  = 0x0000ff00;
    const int bmask  = 0x00ff0000;
    const int amask  = 0;

    board.set_gfx_program(*current_program);

    bool   continue_loop  = true;
    double scale          = 1;
    auto   previous_timer  = std::chrono::high_resolution_clock::now();
    double second_current = 0;
    double second_last    = 0;
    time_t rawtime;
    tm*    timeinfo;

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
                board.set_gfx_program(*current_program);
            }
            else if (e.type == SDL_MOUSEMOTION)
            {
            }
            else if (e.type == SDL_MOUSEWHEEL)
            {
                if (e.wheel.y < 0)
                {
                    if (scale > 0.67) // 160 / 240 = 0.67
                    {
                        scale -= 0.01;
                    }
                    std::cout << "scale: " << scale << std::endl;
                }
                else
                {
                    if (scale < 1.25) // 320 / 240 = 1.25
                    {
                        scale += 0.01;
                    }

                    std::cout << "scale: " << scale << std::endl;
                }
            }
        }
        auto now_timer   = std::chrono::high_resolution_clock::now();
        auto delta_timer = std::chrono::duration_cast<std::chrono::milliseconds>(
            now_timer - previous_timer);

        if (delta_timer.count() > refresh_rate)
        {
            board.clear(black);

            time(&rawtime);
            timeinfo = localtime(&rawtime);

            second_current = timeinfo->tm_sec;
            if (second_current != second_last)
            {
                std::cout << "seconds: " << second_current << std::endl;
            }
            second_last = second_current;

            const double w_in_ren = image.getWidth();
            const double h_in_ren = image.getHeight();
            current_program->set_uniforms(uniforms{ 0,
                                                    0,
                                                    0,
                                                    0,
                                                    scale,
                                                    w_in_ren,
                                                    h_in_ren,
                                                    second_current,
                                                    &texture_arrow,
                                                    &texture_cirle });

            board.draw_triangles(board_vM, indexes_board_v);

            SDL_Surface* bitmapSurface = SDL_CreateRGBSurfaceFrom(pixels,
                                                                  width,
                                                                  height,
                                                                  depth,
                                                                  pitch,
                                                                  rmask,
                                                                  gmask,
                                                                  bmask,
                                                                  amask);
            if (bitmapSurface == nullptr)
            {
                std::cerr << SDL_GetError() << std::endl;
                return EXIT_FAILURE;
            }
            SDL_Texture* bitmapTex =
                SDL_CreateTextureFromSurface(renderer, bitmapSurface);
            if (bitmapTex == nullptr)
            {
                std::cerr << SDL_GetError() << std::endl;
                return EXIT_FAILURE;
            }
            SDL_FreeSurface(bitmapSurface);

            SDL_RenderClear(renderer);
            SDL_RenderCopy(renderer, bitmapTex, nullptr, nullptr);

            SDL_RenderPresent(renderer);

            SDL_DestroyTexture(bitmapTex);
            previous_timer = now_timer;
        }
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);

    SDL_Quit();

    return EXIT_SUCCESS;
}