#include "arrow.hpp"

arrow::arrow(size_t width_,
             size_t height_,
             double arrow_width_,
             double arrow_height_,
             color  main_color_)
    : canvas(width_, height_)
{
    resize(width_ * height_);

    w_a        = arrow_width_ / 2;
    h_a        = arrow_height_;
    main_color = main_color_;
    double w = static_cast<double>(width);
    double h = static_cast<double>(height);    
    // clang-format off
    //             x                  y                      r  g  b  tx                 ty
    arrow_vM = { { w / 2,         h / 2,            0, 0, 1, 1/2,               1/2, 0 },
                 {(w / 2) - w_a, (h / 2) - h_a / 2, 1, 0, 0, (w / 2) - w_a, 1/3, 0 },
                 {(w / 2) + w_a, (h / 2) - h_a / 2, 1, 0, 0, (w / 2) + w_a, 1/3, 0 },
                 { w / 2,        (h / 2) - h_a,     0, 0, 1, 1/2,               1/6, 0 } };
    // clang-format on
    indexes_arrow_v = { { 0, 1, 2, 1, 2, 3 } };
}

arrow::~arrow()
{
}

void arrow::draw_arrow(const gfx::my_uniforms& a_uniforms)
{
    triangle_interpolated triangles(*this);
    triangles.clear(main_color);
    gfx.set_uniforms(a_uniforms);
    triangles.set_gfx_program(gfx);
    triangles.draw_triangles(arrow_vM, indexes_arrow_v);
}
