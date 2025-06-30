#pragma once

#pragma once
#include "SFML/Graphics.hpp"
#include "../../../utils/vec.hpp"
#include "utils.hpp"

namespace pez
{

struct CardEmpty : public sf::Drawable, public sf::Transformable
{
    sf::VertexArray va;
    sf::VertexArray va_shadow;

    Vec2f           position;
    Vec2f           size;
    float           corner_radius = 0.0f;
    float           shadow_size   = 8.0f;
    float           thickness     = 8.0f;
    sf::Color       color;

    uint32_t quality = 64;

    CardEmpty() = default;

    CardEmpty(Vec2f size_, float corner_radius_, sf::Color color_)
        : va{sf::PrimitiveType::TriangleStrip}
    , va_shadow{sf::PrimitiveType::TriangleStrip}
    , size{size_}
    , corner_radius{corner_radius_}
    , color{color_}
    {
        updateGeometry();
    }

    void setColor(sf::Color color_)
    {
        color = color_;
        uint64_t const vertex_count = va.getVertexCount();
        for (uint64_t i{vertex_count}; i--;) {
            va[i].color = color;
        }
    }

    void updateGeometry(bool skip = false)
    {
        // Sometimes we want to change multiple attribute and do only one update for all of them
        if (skip) {
            return;
        }
        generateGeometry(va);
        generateGeometryShadow(va_shadow, {0, 0, 0, 30}, {0, 0, 0, 0});
    }

    void setShadowSize(float size_, bool skip_geometry_update = false)
    {
        shadow_size = size_;
        updateGeometry(skip_geometry_update);
    }

    void setThickness(float thickness_, bool skip_geometry_update = false)
    {
        thickness = thickness_;
        updateGeometry(skip_geometry_update);
    }

    void draw(sf::RenderTarget& target, sf::RenderStates states) const override
    {
        states.transform *= getTransform();
        if (shadow_size > 0.0f) {
            target.draw(va_shadow, states);
        }
        target.draw(va, states);
    }

    void setWidth(float width, bool skip_geometry_update = false)
    {
        size.x = width;
        updateGeometry(skip_geometry_update);
    }

    void setSize(Vec2f size_, bool skip_geometry_update = false)
    {
        size = size_;
        updateGeometry(skip_geometry_update);
    }

    void generateGeometry(sf::VertexArray& vertex_array)
    {
        const float radius = std::min(std::min(corner_radius, size.x * 0.5f), size.y * 0.5f);

        CardGeometryGenerator generator;
        generator.quality = quality;
        vertex_array.resize(2 * generator.getVertexCount());

        generator.size = size;
        generator.radius = radius;
        generator.start = 0;
        generator.skip = 1;
        generator.color = color;
        generator.generateVertex(&vertex_array);

        generator.size = {size.x - 2.0f * thickness, size.y - 2.0f * thickness};
        generator.radius = radius - thickness;
        generator.start = 1;
        generator.skip = 1;
        generator.color = color;
        generator.generateVertex(&vertex_array, Vec2f{thickness, thickness});
    }

    void generateGeometryShadow(sf::VertexArray& vertex_array, sf::Color color_in, sf::Color color_out)
    {
        const float radius = std::min(std::min(corner_radius, size.x * 0.5f), size.y * 0.5f);

        CardGeometryGenerator generator;
        generator.quality = quality;
        vertex_array.resize(2 * generator.getVertexCount());

        generator.size = size;
        generator.radius = radius;
        generator.start = 0;
        generator.skip = 1;
        generator.color = color_in;
        generator.generateVertex(&vertex_array);

        generator.size = {size.x + 2.0f * shadow_size, size.y + 2.0f * shadow_size};
        generator.radius = radius + shadow_size;
        generator.start = 1;
        generator.skip = 1;
        generator.color = color_out;
        generator.generateVertex(&vertex_array, -Vec2f{shadow_size, shadow_size});
    }
};
}
