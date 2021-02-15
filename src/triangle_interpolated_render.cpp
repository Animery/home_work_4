#include "../include/triangle_interpolated_render.hpp"

#include <algorithm>
#include <iostream>

constexpr double accuracy_ratio = 1.45;

std::ostream& operator<<(std::ostream& out, const position& v)
{
    out << "x: " << v.x << "\t";
    out << "y: " << v.y;
    return out;
}

double interpolate(const double first, const double second, const double t)
{
    assert(t >= 0.0);
    assert(t <= 1.0);
    return first + (second - first) * t;
}

vertex interpolate(const vertex& v0, const vertex& v1, const double t)
{
    return {
        interpolate(v0.x, v1.x, t),         interpolate(v0.y, v1.y, t),
        interpolate(v0.r, v1.r, t),         interpolate(v0.g, v1.g, t),
        interpolate(v0.b, v1.b, t),         interpolate(v0.x_tex, v1.x_tex, t),
        interpolate(v0.y_tex, v1.y_tex, t), interpolate(v0.f7, v1.f7, t)
    };
}

triangle_interpolated::triangle_interpolated(canvas& buffer)
    : triangle_indexed_render(buffer)
{
}

void triangle_interpolated::draw_triangles(vertexMap&             vertexes,
                                           std::vector<uint16_t>& indexes)
{
    const size_t size = indexes.size();
    for (size_t i = 0; i < size / 3; ++i)
    {
        const uint16_t index_A = indexes.at(i * 3 + 0);
        const uint16_t index_B = indexes.at(i * 3 + 1);
        const uint16_t index_C = indexes.at(i * 3 + 2);

        const vertex& v0 = vertexes.at(index_A);
        const vertex& v1 = vertexes.at(index_B);
        const vertex& v2 = vertexes.at(index_C);

        const vertex v0_ = program_->vertex_shader(v0);
        const vertex v1_ = program_->vertex_shader(v1);
        const vertex v2_ = program_->vertex_shader(v2);

        const vertexMap interpoleted = rasterize_triangle(v0_, v1_, v2_);

        draw_pix(interpoleted);
    }
}

vertexMap triangle_interpolated::rasterize_triangle(const vertex& v0,
                                                    const vertex& v1,
                                                    const vertex& v2)
{
    vertexMap out;

    std::array<const vertex*, 3> sort_vertex{ &v0, &v1, &v2 };
    std::sort(begin(sort_vertex),
              end(sort_vertex),
              [](const vertex* left, const vertex* right) {
                  return left->y < right->y;
              });

    const vertex& top_vertex    = *sort_vertex.at(0);
    const vertex& middle_vertex = *sort_vertex.at(1);
    const vertex& bottom_vertex = *sort_vertex.at(2);

    position top_pos{ static_cast<int32_t>(std::round(top_vertex.x)),
                      static_cast<int32_t>(std::round(top_vertex.y)) };
    position mid_pos{ static_cast<int32_t>(std::round(middle_vertex.x)),
                      static_cast<int32_t>(std::round(middle_vertex.y)) };
    position bottom_pos{ static_cast<int32_t>(std::round(bottom_vertex.x)),
                         static_cast<int32_t>(std::round(bottom_vertex.y)) };

    // Here 3 quik and durty HACK if triangle consist from same points
    int konst = 2;
    if (top_pos == mid_pos)
    {
        // just render line start -> middle
        position delta = top_pos - bottom_pos;
        size_t   count_pixels =
            konst * (std::abs(delta.x) + std::abs(delta.y) + 1);
        std::cout << "1-count_pixels: " << count_pixels << std::endl;
        for (size_t i = 0; i < count_pixels; ++i)
        {
            double t      = static_cast<double>(i) / count_pixels;
            vertex vertex = interpolate(top_vertex, bottom_vertex, t);
            out.push_back(vertex);
        }

        return out;
    }

    if (top_pos == bottom_pos)
    {
        // just render line start -> middle
        position delta = top_pos - mid_pos;
        size_t   count_pixels =
            konst * (std::abs(delta.x) + std::abs(delta.y) + 1);
        std::cout << "2-count_pixels: " << count_pixels << std::endl;
        for (size_t i = 0; i < count_pixels + 1; ++i)
        {
            double t      = static_cast<double>(i) / count_pixels;
            vertex vertex = interpolate(top_vertex, middle_vertex, t);
            out.push_back(vertex);
        }

        return out;
    }

    if (bottom_pos == mid_pos)
    {
        // just render line start -> middle
        position delta = top_pos - mid_pos;
        size_t   count_pixels =
            konst * (std::abs(delta.x) + std::abs(delta.y) + 1);

        std::cout << "3-count_pixels: " << count_pixels << std::endl;
        for (size_t i = 0; i < count_pixels + 1; ++i)
        {
            double t      = static_cast<double>(i) / count_pixels;
            vertex vertex = interpolate(bottom_vertex, top_vertex, t);
            out.push_back(vertex);
        }

        return out;
    }

    pixels long_side = pixels_positions(top_pos, bottom_pos);

    auto it_middle =
        std::find_if(begin(long_side),
                     std::end(long_side),
                     [&](const position& pos) { return pos.y == mid_pos.y; });
    assert(it_middle != std::end(long_side));
    position mid_pos_second = *it_middle;

    double t           = 0;
    double length_long = (bottom_pos - top_pos).length();
    if (length_long > 0)
    {

        double length_mid = (mid_pos_second - top_pos).length();
        t                 = length_mid / length_long;
    }

    vertex second_middle_vertex = interpolate(top_vertex, bottom_vertex, t);
    // top_triangle
    raster_horizontal_triangle(
        top_vertex, middle_vertex, second_middle_vertex, out);
    // bottom_triangle
    raster_horizontal_triangle(
        bottom_vertex, middle_vertex, second_middle_vertex, out);

    return out;
}

void triangle_interpolated::raster_horizontal_triangle(const vertex& single,
                                                       const vertex& left,
                                                       const vertex& right,
                                                       vertexMap&    out)
{
    size_t num_of_hlines =
        static_cast<size_t>(std::round(std::abs(single.y - left.y)));

    if (num_of_hlines > 0)
    {
        for (size_t i = 0; i <= num_of_hlines; ++i)
        {
            double t_vertical   = static_cast<double>(i) / num_of_hlines;
            vertex left_vertex  = interpolate(left, single, t_vertical);
            vertex right_vertex = interpolate(right, single, t_vertical);

            raster_one_horizontal_line(left_vertex, right_vertex, out);
        }
    }
    else
    {
        raster_one_horizontal_line(left, right, out);
    }
}

void triangle_interpolated::raster_one_horizontal_line(
    const vertex& left_vertex, const vertex& right_vertex, vertexMap& out)
{
    size_t num_of_pixels_in_line =
        // use +1 pixels to garantee no empty black pixels
        static_cast<size_t>(
            std::round(std::abs(left_vertex.x - right_vertex.x))) +
        1;

    if (num_of_pixels_in_line > 0)
    {
        for (size_t p = 0; p <= num_of_pixels_in_line; ++p)
        {
            double t_pixel = static_cast<double>(p) / num_of_pixels_in_line;
            vertex pixel   = interpolate(left_vertex, right_vertex, t_pixel);
            out.push_back(pixel);
        }
    }
    else
    {
        out.push_back(left_vertex);
    }
}

void triangle_interpolated::draw_pix(const vertexMap& v_map)
{
    for (const auto& i : v_map)
    {
        const color    col = program_->fragment_shader(i);
        const position pos{ static_cast<int32_t>(std::round(i.x)),
                            static_cast<int32_t>(std::round(i.y)) };
        set_pixel(pos, col);
    }
}

void triangle_interpolated::draw_circle(vertexMap&             vertexes,
                                        std::vector<uint16_t>& indexes)
{

    const size_t size = indexes.size();
    for (size_t i = 0; i < size / 2; ++i)
    {
        const uint16_t index_center = indexes.at(i * 3 + 0);
        const uint16_t index_border = indexes.at(i * 3 + 1);

        const vertex& v0 = vertexes.at(index_center);
        const vertex& v1 = vertexes.at(index_border);

        const vertex start_v  = program_->vertex_shader(v0);
        const vertex border_v = program_->vertex_shader(v1);

        const vertexMap interpoleted =
            rasterize_circle_vertex(start_v, border_v);

        draw_pix(interpoleted);
    }
}

vertexMap triangle_interpolated::rasterize_circle_vertex(const vertex& start,
                                                         const vertex& border)
{
    vertexMap result;
    result.push_back(start);

    double radius = std::abs((start.x + start.y) - (border.x + border.y));

    // need reserve  ??  dont work
    result.reserve(
        static_cast<size_t>(radius * radius * 3.14 * (accuracy_ratio * 4)));

    rasterize_round_vertex(border, radius, result);
    // hack for accuracy
    rasterize_round_vertex(border, radius - 1, result);

    size_t size = result.size();
    std::cout << "SIZE_round = " << size << "\tsize_result: " << result.size()
              << std::endl;
    for (size_t i = 1; i < size; i++)
    {
        rasterize_line_circle(result[0], result[i], radius, result);
    }
    std::cout << "SIZE_round = " << size << "\tsize_result: " << result.size()
              << "\tresult.capacity(): " << result.capacity() << std::endl;
    std::cout << "radius = " << radius << std::endl;
    return result;
}

void triangle_interpolated::rasterize_round_vertex(const vertex& border,
                                                   double        radius,
                                                   vertexMap&    out)
{
    int x     = 0;
    int y     = radius;
    int delta = 1 - 2 * radius;
    int error = 0;

    while (y >= 0)
    {
        // clang-format off
        out.push_back(vertex{ out[0].x + x, out[0].y + y, border.r, border.g, border.b, border.x_tex, border.y_tex, border.f7});
        out.push_back(vertex{ out[0].x + x, out[0].y - y, border.r, border.g, border.b, border.x_tex, border.y_tex, border.f7 });
        out.push_back(vertex{ out[0].x - x, out[0].y + y, border.r, border.g, border.b, border.x_tex, border.y_tex, border.f7 });
        out.push_back(vertex{ out[0].x - x, out[0].y - y, border.r, border.g, border.b, border.x_tex, border.y_tex, border.f7 });
        // clang-format on

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
}

void triangle_interpolated::rasterize_line_circle(const vertex& left_vertex,
                                                  const vertex& right_vertex,
                                                  double        radius,
                                                  vertexMap&    out)
{
    // use * accuracy_ratio pixels to garantee no empty black pixels
    // need  more  accuracy_ratio
    size_t num_of_pixels_in_line = static_cast<size_t>(radius * accuracy_ratio);
    if (num_of_pixels_in_line > 0)
    {
        for (size_t p = 0; p < num_of_pixels_in_line + 1; ++p)
        {
            double t_pixel = static_cast<double>(p) / num_of_pixels_in_line;
            vertex pixel   = interpolate(left_vertex, right_vertex, t_pixel);
            out.push_back(pixel);
        }
    }
    else
    {
        out.push_back(left_vertex);
    }
}