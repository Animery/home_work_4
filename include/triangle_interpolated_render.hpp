#pragma once

#include "triangle_indexed_render.hpp"

/*FULL = 0
ROUND = 1*/
enum class Flagcircle
{
    FULL,
    ROUND
};

/*double x  = 0;
    double y  = 0;
    double r  = 0;
    double g  = 0;
    double b  = 0;
    double x_tex  = 0;
    double y_tex  = 0;
    double f7 = 0;  ?*/
struct vertex
{
    double x     = 0; /// x
    double y     = 0; /// y
    double r     = 0; /// r
    double g     = 0; /// g
    double b     = 0; /// b
    double x_tex = 0; /// u (texture coordinate)
    double y_tex = 0; /// v (texture coordinate)
    double f7    = 0; /// ?
    double length() const { return std::sqrt(x * x + y * y); };
};

using vertexMap = std::vector<vertex>;

double interpolate(const double first, const double second, const double t);

vertex interpolate(const vertex& v0, const vertex& v1, const double t);

/*
double f0 = 0;
double f1 = 0;
double f2 = 0;
double f3 = 0;
double f4 = 0;                  // scale
double f5 = 0;                  // width  Image
double f6 = 0;                  // height Image
double f7 = 0;                  // time_from_start
canvas* texture0 = nullptr;     // ptr Texture
canvas* texture1 = nullptr;     // ptr Texture
*/
struct uniforms
{
    double  f0       = 0;
    double  f1       = 0;
    double  f2       = 0;
    double  f3       = 0;
    double  f4       = 0;       // scale
    double  f5       = 0;       // width  Image
    double  f6       = 0;       // height Image
    double  f7       = 0;       // time_from_start
    canvas* texture0 = nullptr; // ptr Texture
    canvas* texture1 = nullptr; // ptr Texture
};

struct gfx_program
{
    virtual ~gfx_program()                             = default;
    virtual void   set_uniforms(const uniforms&)       = 0;
    virtual vertex vertex_shader(const vertex& v_in)   = 0;
    virtual color  fragment_shader(const vertex& v_in) = 0;
};

struct triangle_interpolated : triangle_indexed_render
{
    triangle_interpolated(canvas& buffer);
    void set_gfx_program(gfx_program& program) { program_ = &program; }
    void draw_triangles(vertexMap& vertexes, std::vector<uint16_t>& indexes);
    void draw_circle(vertexMap&,
                     std::vector<uint16_t>&,
                     Flagcircle flag = Flagcircle::FULL);

private:
    vertexMap rasterize_triangle(const vertex& v0,
                                 const vertex& v1,
                                 const vertex& v2);
    void      raster_horizontal_triangle(const vertex& single,
                                         const vertex& left,
                                         const vertex& right,
                                         vertexMap&    out);

    void raster_one_horizontal_line(const vertex& left_vertex,
                                    const vertex& right_vertex,
                                    vertexMap&    out);
    void draw_pix(const vertexMap& v_map);

    vertexMap rasterize_circle_vertex(const vertex& start,
                                      const vertex& border,
                                      Flagcircle    flag);
    void      rasterize_round_vertex(const vertex& border,
                                     double        radius,
                                     vertexMap&    out);
    void      rasterize_line_circle(const vertex& left_vertex,
                                    const vertex& right_vertex,
                                    double        radius,
                                    vertexMap&    out);

    gfx_program* program_ = nullptr;
};