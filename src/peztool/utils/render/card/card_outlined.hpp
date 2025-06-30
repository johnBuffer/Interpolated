#pragma once

#pragma once
#include "SFML/Graphics.hpp"
#include "../../../utils/vec.hpp"

#include "./card.hpp"
#include "./card_empty.hpp"

namespace pez
{

struct CardOutlined : public sf::Drawable, public sf::Transformable
{
    Card      background;
    CardEmpty outline;

    CardOutlined(Vec2f size_, float corner_radius_, float thickness, sf::Color color)
        : background{size_, corner_radius_, color}
        , outline{size_, corner_radius_ + thickness, sf::Color::White}
    {
        setOuterSize(size_, thickness);
    }

    void setOuterSize(Vec2f size, bool skip_geometry_update = false)
    {
        outline.setSize(size);
        float const thickness{outline.thickness};
        background.setCornerRadius(outline.corner_radius - thickness, skip_geometry_update);
        background.setSize(outline.size - 2.0f * Vec2f{thickness, thickness}, skip_geometry_update);
    }

    void setOuterSize(Vec2f size, float thickness, bool skip_geometry = false)
    {
        outline.setSize(size, skip_geometry);
        setOutlineThickness(thickness, skip_geometry);
    }

    void setFillColor(sf::Color color)
    {
        background.setColor(color);
    }

    void setOutlineColor(sf::Color color)
    {
        outline.setColor(color);
    }

    /// Sets the thickness of the outline outside of the background (global size = inner_size + thickness)
    void setOutlineThickness(float thickness, bool skip_geometry_update = false)
    {
        outline.setThickness(thickness, skip_geometry_update);
        background.setCornerRadius(outline.corner_radius - thickness, skip_geometry_update);
        background.setSize(outline.size - 2.0f * Vec2f{thickness, thickness}, skip_geometry_update);
    }

    void setOutlineShadowSize(float size_, bool skip_geometry_update = false)
    {
        outline.setShadowSize(size_, skip_geometry_update);
    }

    void setBackgroundShadowSize(float size_, bool skip_geometry_update = false)
    {
        background.setShadowSize(size_, skip_geometry_update);
    }

    void setRadius(float r, bool skip_geometry_update = false)
    {
        outline.corner_radius = r;
        background.corner_radius = r - getThickness();
        updateGeometry(skip_geometry_update);
    }

    [[nodiscard]]
    float getThickness() const
    {
        return outline.thickness;
    }

    void draw(sf::RenderTarget& target, sf::RenderStates states) const override
    {
        states.transform *= getTransform();
        target.draw(outline, states);
        states.transform.translate(outline.thickness, outline.thickness);
        target.draw(background, states);
    }

    [[nodiscard]]
    Vec2f getOutlineSize() const
    {
        return outline.size;
    }

    void updateGeometry(bool skip)
    {
        outline.updateGeometry(skip);
        background.updateGeometry(skip);
    }
};

}

