#include "../include/line_render.hpp"

#include <algorithm>

line_render::line_render(canvas& buffer_, size_t width, size_t height)
    : buffer(buffer_)
    , w(width)
    , h(height)
{
}

void line_render::clear(color fill_color)
{
    std::fill(begin(buffer), end(buffer), fill_color);
}

void line_render::set_pixel(position pos, color mark_color)
{
    const size_t pos_in_line =
        static_cast<unsigned>(pos.y) * w + static_cast<unsigned>(pos.x);
    if (pos_in_line >= buffer.size())
    {
        return;
    }
    buffer[pos_in_line] = mark_color;
}

pixels line_render::pixels_positions(const position start, const position end)
{
    pixels result;
    int    x1 = start.x;
    int    y1 = start.y;
    int    x2 = end.x;
    int    y2 = end.y;

    const int deltaX = abs(x2 - x1);
    const int deltaY = abs(y2 - y1);
    const int signX  = x1 < x2 ? 1 : -1;
    const int signY  = y1 < y2 ? 1 : -1;
    int       error  = deltaX - deltaY;

    result.push_back(position{ x2, y2 });

    while (x1 != x2 || y1 != y2)
    {
        result.push_back(position{ x1, y1 });
        int error2 = error * 2;
        if (error2 > -deltaY)
        {
            error -= deltaY;
            x1 += signX;
        }
        if (error2 < deltaX)
        {
            error += deltaX;
            y1 += signY;
        }
    }

    return result;
}

void line_render::draw_circle(const position start, const int radius, color color)
{

    pixels result;
    int    x     = 0;
    int    y     = radius;
    int    delta = 1 - 2 * radius;
    int    error = 0;
    while (y >= 0)
    {
        result.push_back(position{ start.x + x, start.y + y });
        result.push_back(position{ start.x + x, start.y - y });
        result.push_back(position{ start.x - x, start.y + y });
        result.push_back(position{ start.x - x, start.y - y });

        error = 2 * (delta + y) - 1;
        if (delta < 0 && error <= 0)
        {
            ++x;
            delta += 2 * x + 1;
            continue;
        }
        error = 2 * (delta - x) - 1;
        if (delta > 0 && error > 0)
        {
            --y;
            delta += 1 - 2 * y;
            continue;
        }
        ++x;
        delta += 2 * (x - y);
        --y;
    }

    for (const auto& pos : result)
        set_pixel(pos, color);
}

void line_render::draw_pix(pixels& pix, color color)
{
    for (const auto& i : pix)
        set_pixel(i, color);
}

void line_render::draw_line(const position start, const position end, color color)
{
    pixels pix = pixels_positions(start, end);

    draw_pix(pix, color);
    // for (const auto& pos : pix)
    //     set_pixel(pos, col);

    // std::for_each(
    //     begin(l), std::end(l), [&](auto& pos) { set_pixel(pos, col); });
}
