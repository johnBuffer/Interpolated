#pragma once
#include "interpolated/interpolated.hpp"
#include "peztool/core/system.hpp"

/// Very basic renderer that draws a circle at the position 'circle_position'
struct Renderer final : public pez::Renderer<>
{
    Interpolated<Vec2f> circle_position{};

    void render(pez::RenderContext& context) override
    {
        float constexpr radius{200.0f};
        sf::CircleShape shape{radius};
        shape.setPosition(circle_position);
        context.draw(shape);
    }
};















