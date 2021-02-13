#include "../include/triangle_indexed_render.hpp"

triangle_indexed_render::triangle_indexed_render(canvas& buffer)
    : triangle_render(buffer)
{
}

void triangle_indexed_render::draw_triangles(std::vector<position>& vertexes,
                                             std::vector<uint16_t>&  indexes,
                                             color                  color)
{
    pixels       triangles_edge_pixels;
    const size_t size = indexes.size();

    for (size_t i = 0; i < size / 3; ++i)
    {
        // uint8_t index_A = indexes[i * 3 + 0];
        // uint8_t index_B = indexes[i * 3 + 1];
        // uint8_t index_C = indexes[i * 3 + 2];
        const uint16_t index_A = indexes.at(i * 3 + 0);
        const uint16_t index_B = indexes.at(i * 3 + 1);
        const uint16_t index_C = indexes.at(i * 3 + 2);
        

        for (auto&& pos : pixels_positions_triangle(
                 vertexes[index_A], vertexes[index_B], vertexes[index_C]))
            triangles_edge_pixels.push_back(pos);
    }

    draw_pix(triangles_edge_pixels, color);
}