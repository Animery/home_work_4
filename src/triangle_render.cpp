#include "../include/triangle_render.hpp"

triangle_render::triangle_render(canvas& buffer)
    : line_render(buffer)
{
}

pixels triangle_render::pixels_positions_triangle(position v0,
                                                  position v1,
                                                  position v2)
{
    pixels result;

    for (auto [start, end] :
         { std::pair{ v0, v1 }, std::pair{ v1, v2 }, std::pair{ v2, v0 } })
    {
        for (auto pos : line_render::pixels_positions(start, end))
        {
            result.push_back(pos);
        }
    }
    return result;
}

void triangle_render::draw_triangle(position v0,
                                    position v1,
                                    position v2,
                                    color    color)
{
    pixels triangles_edge_pixels = pixels_positions_triangle(v0, v1, v2);
    draw_pix(triangles_edge_pixels, color);
}