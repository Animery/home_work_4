#pragma once

#ifndef __ARROW_H__
#define __ARROW_H__

#include "gfx.hpp"

class arrow : public canvas
{
public:
    arrow(size_t width_,
          size_t height_,
          double arrow_width_,
          double arrow_height_,
          color  main_color);
    ~arrow();

    void clear() { std::fill(std::begin(*this), std::end(*this), main_color); }
    void draw_arrow(const gfx::my_uniforms& a_uniforms);

private:
    double w_a        = 0;
    double h_a        = 0;
    color  main_color = { 0, 0, 0 };

    vertexMap arrow_vM;
    std::vector<uint16_t> indexes_arrow_v;

    gfx::rotate_arrow gfx;
};
#endif // __ARROW_H__