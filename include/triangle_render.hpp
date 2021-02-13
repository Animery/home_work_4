#pragma once

#include "line_render.hpp"

struct triangle_render : public line_render
{

    triangle_render(canvas& buffer);
    void draw_triangle(position v0, position v1, position v2, color color);

protected:
    virtual pixels pixels_positions_triangle(position v0,
                                             position v1,
                                             position v2);
};
