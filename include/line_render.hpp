#pragma once

#include "canvas.hpp"

struct line_render : protected irender
{
    line_render(canvas& buffer_, size_t width, size_t height);
    void draw_line(const position start, const position end, color);
    void draw_circle(const position start, const int radius, color);
    void clear(color) override;

protected:
    void   set_pixel(position, color) override;
    pixels pixels_positions(const position start, const position end);
    void   draw_pix(pixels& pix, color col) override;

private:
    canvas&      buffer;
    const size_t w;
    const size_t h;
};