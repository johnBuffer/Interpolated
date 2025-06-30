#pragma once
#include <cassert>
#include "SFML/Graphics.hpp"

#include "../utils/vec.hpp"
#include "../utils/events.hpp"


namespace pez
{

/// Represents a render layer used to sort draw calls and apply different transforms to objects
struct Layer
{
    /// Identifies the layer in the layer container
    using ID = uint64_t;

    /// The target texture where objects will be drawn
    sf::RenderTarget* texture_ptr;
    sf::Transform transform;

    bool transform_changed{true};
    Vec2f const center;
    float scale = 1.0f;
    Vec2f offset;

    explicit
    Layer(Vec2u size_, sf::RenderTarget& target_)
        : texture_ptr{&target_}
        , center{static_cast<Vec2f>(size_) * 0.5f}
    {
    }

    void moveView(sf::Vector2f v)
    {
        offset += v / scale;
        transform_changed = true;
    }

    void zoom(float zoom)
    {
        scale *= zoom;
        transform_changed = true;
    }

    void setViewPosition(Vec2f position)
    {
        offset = position;
        transform_changed = true;
    }

    void setZoom(float zoom)
    {
        scale = zoom;
        transform_changed = true;
    }

    [[nodiscard]]
    sf::FloatRect getViewport(float margin = 0.0f) const
    {
        Vec2f const size{(2.0f / scale) * center + 2.0f * Vec2f{margin, margin}};
        return {offset - size * 0.5f, size};
    }

    [[nodiscard]]
    float getZoom() const
    {
        return scale;
    }

    /// Draws the object on the texture, applying the transform
    void draw(sf::Drawable& drawable)
    {
        texture_ptr->draw(drawable, getTransform());
    }

    /// Draws the object on the texture, applying the transform
    void draw(sf::Drawable& drawable, sf::RenderStates states)
    {
        states.transform = getTransform() * states.transform;
        texture_ptr->draw(drawable, states);
    }

    sf::Transform const& getTransform()
    {
        if (transform_changed) {
            transform_changed = false;
            transform = {};
            transform.translate(center);
            transform.scale({scale, scale});
            transform.translate(-offset);
        }
        return transform;
    }
};

/// Stores all layers and routes draw calls to relevant layers, applying the corresponding transformations
class RenderContext
{
public:
    /// RenderContext
    explicit
    RenderContext(sf::RenderWindow& window, sf::Vector2u size)
        : m_window{window}
        , m_size{size}
        , m_window_size{m_window.getSize()}
        , m_size_f{static_cast<sf::Vector2f>(size)}
    {
        m_mouse_position = sf::Mouse::getPosition(window);
        m_scale = {static_cast<float>(m_window_size.x) / m_size_f.x,
                   static_cast<float>(m_window_size.y) / m_size_f.y};
    }

    /// Creates a new layer and registers it
    Layer::ID registerLayer()
    {
        m_layers.emplace_back(m_size, m_window);
        return m_layers.size() - 1;
    }

    /// Draw directly to the window, skips layers
    void draw(sf::Drawable& drawable)
    {
        m_window.draw(drawable);
    }

    /// Draw directly to the window, skips layers
    void draw(sf::Drawable& drawable, sf::RenderStates states)
    {
        m_window.draw(drawable, states);
    }

    /// Dispatch the draw call to the target Layer
    void draw(sf::Drawable& drawable, Layer::ID layer)
    {
        assert(layer < m_layers.size());
        m_layers[layer].draw(drawable);
    }

    /// Dispatch the draw call to the target Layer
    void draw(sf::Drawable& drawable, sf::RenderStates states, Layer::ID layer)
    {
        assert(layer < m_layers.size());
        m_layers[layer].draw(drawable, states);
    }

    /// Creates a world layer and a HUD layer. The world layer has its viewport controlled by mouse.
    void createDefaultLayers(EventHandler& handler)
    {
        // Create the layers
        m_world_layer = registerLayer();
        m_hud_layer = registerLayer();

        /*float constexpr zoom_factor     = 1.2f;
        float constexpr zoom_factor_inv = 1.0f / zoom_factor;
        handler.onMouseScrolled([this, zoom_factor, zoom_factor_inv](sf::Event::MouseWheelScrollEvent event) {
            // Ensure that there is actually a delta (many 0s on MacOS)
            if (event.delta == 0.0f) {
                return;
            }
            Layer& world_layer = m_layers[m_world_layer];
            if (event.delta > 0.0f) {
                world_layer.zoom(zoom_factor);
            } else {
                world_layer.zoom(zoom_factor_inv);
            }
        });

        handler.onMouseMoved([this](sf::Event::MouseMoveEvent const&) {
            Layer& world_layer = m_layers[m_world_layer];
            if (sf::Mouse::isButtonPressed(sf::Mouse::Button::Left)) {
                auto const new_position = sf::Mouse::getPosition(m_window);
                world_layer.moveView(static_cast<Vec2f>(m_mouse_position - new_position));
            }
            updateMousePosition();
        });*/
    }

    [[nodiscard]]
    Layer::ID getWorldLayerID() const
    {
        return m_world_layer;
    }

    [[nodiscard]]
    Layer::ID getHudLayerID() const
    {
        return m_hud_layer;
    }

    [[nodiscard]]
    Layer& getLayer(Layer::ID id)
    {
        assert(id < m_layers.size());
        return m_layers[id];
    }

    [[nodiscard]]
    Layer const& getLayer(Layer::ID id) const
    {
        assert(id < m_layers.size());
        return m_layers[id];
    }

    [[nodiscard]]
    Layer& getWorldLayer()
    {
        return getLayer(getWorldLayerID());
    }

    [[nodiscard]]
    Layer const& getWorldLayer() const
    {
        return getLayer(getWorldLayerID());
    }

    [[nodiscard]]
    Layer& getHudLayer()
    {
        return getLayer(getHudLayerID());
    }

    void clear(sf::Color clear_color = {50, 50, 50})
    {
        m_window.clear(clear_color);
    }

    void renderLayers()
    {
        m_window.display();
    }

    void setDefaultView(Vec2f view_position, float zoom)
    {
        sf::View view;
        view.setCenter(view_position);
        view.zoom(zoom);
        m_window.setView(view);
    }

    void setDefaultView(sf::View const& view)
    {
        m_window.setView(view);
    }

    [[nodiscard]]
    Vec2f getRenderSize() const
    {
        return m_size_f;
    }

    [[nodiscard]]
    Vec2i getMousePosition() const
    {
        return m_mouse_position;
    }

    [[nodiscard]]
    Vec2f getMouseWorldPosition() const
    {
        Layer const& world_layer = getWorldLayer();
        Vec2f const mouse_position_f{m_mouse_position};
        return (mouse_position_f - static_cast<Vec2f>(m_window.getSize()) * 0.5f) / world_layer.scale + world_layer.offset;
    }

private:
    /// The window
    sf::RenderWindow& m_window;
    /// Stores all the layers
    std::vector<Layer> m_layers;
    /// Render size
    sf::Vector2u m_size;
    /// Window size
    sf::Vector2u m_window_size;
    /// Float version of the render size
    sf::Vector2f m_size_f;
    /// Scale factor
    sf::Vector2f m_scale;
    /// Default world layer
    Layer::ID m_world_layer = 0;
    /// Default HUD layer
    Layer::ID m_hud_layer = 0;
    /// Click position
    Vec2i m_mouse_position;

private:
    void updateMousePosition()
    {
        m_mouse_position = sf::Mouse::getPosition(m_window);
    }
};

}
