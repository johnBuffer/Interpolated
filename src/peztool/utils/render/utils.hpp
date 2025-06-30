#pragma once
#include <SFML/Graphics.hpp>

#include "../vec.hpp"

namespace pez
{

template<typename TVec2>
void generateLine(sf::VertexArray* vertex_array, uint32_t index, TVec2 point_1, TVec2 point_2, float width, sf::Color color, float offset = 0.0f)
{
    sf::Vector2f const p1_p2  = point_2 - point_1;
    float const        length = std::sqrt(p1_p2.x * p1_p2.x + p1_p2.y * p1_p2.y);
    sf::Vector2f const v      = p1_p2 / length;
    sf::Vector2f const n      = {-v.y, v.x};

    Vec2f const offset_v = v * (offset * 0.5f);
    Vec2f const normal_v = n * 0.5f * width;

    sf::VertexArray& va{*vertex_array};

    va[index + 0].position = point_1 + offset_v + normal_v;
    va[index + 1].position = point_2 - offset_v + normal_v;
    va[index + 2].position = point_2 - offset_v - normal_v;
    va[index + 3].position = point_1 + offset_v - normal_v;

    va[index + 0].color = color;
    va[index + 1].color = color;
    va[index + 2].color = color;
    va[index + 3].color = color;
}

inline void stripText(sf::Text& text)
{
    sf::Vector2f const origin{text.getOrigin()};
    auto const bounds = text.getLocalBounds();
    sf::Vector2f const offset{bounds.left, bounds.top};
    text.setOrigin(origin + sf::Vector2f{offset.x, offset.y});
}

}
