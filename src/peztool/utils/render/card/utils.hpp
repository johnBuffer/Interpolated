#pragma once
#include "../../vec.hpp"
#include "../../math.hpp"

namespace pez
{

struct CardGeometryGenerator
{
    float radius     = 0.0f;
    Vec2f size       = {};
    uint32_t quality = 64;
    uint32_t start   = 0;
    uint32_t skip    = 0;
    sf::Color color  = sf::Color::White;

    [[nodiscard]]
    uint64_t getVertexCount() const
    {
        return getVertexCount(quality);
    }

    static uint64_t getVertexCount(uint32_t quality)
    {
        // One additional point per arc plus closing the loop
        return quality + 5;
    }

    void generateArc(sf::VertexArray* va, Vec2f center, float angle_start, uint32_t* global_index, Vec2f offset) const
    {
        uint32_t const arc_quality = quality / 4;
        float const da{Constant32::TwoPi / static_cast<float>(quality)};
        sf::VertexArray& vertex_array{*va};

        for (uint32_t i(0); i < arc_quality + 1; ++i) {
            const auto fi = static_cast<float>(i);
            const float angle = angle_start + fi * da;
            vertex_array[*global_index].position = center + radius * Vec2f{cos(angle), sin(angle)} + offset;
            vertex_array[*global_index].color = color;
            (*global_index) += 1 + skip;
        }
    }

    void generateVertex(sf::VertexArray* va, Vec2f offset = {}) const
    {
        uint32_t global_index{start};
        sf::VertexArray& vertex_array{*va};

        // Bottom right
        generateArc(va, {size.x - radius, size.y - radius}, 0.0f, &global_index, offset);
        // Bottom left
        generateArc(va, {radius, size.y - radius}, Constant32::Pi * 0.5f, &global_index, offset);
        // Top left
        generateArc(va, {radius, radius}, Constant32::Pi, &global_index, offset);
        // Top right
        generateArc(va, {size.x - radius, radius}, Constant32::Pi * 1.5f, &global_index, offset);

        // Close the loop
        vertex_array[global_index].position = {size.x + offset.x, size.y - radius + offset.y};
        vertex_array[global_index].color = color;
    }
};

}