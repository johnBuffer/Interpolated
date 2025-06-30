#pragma once
#include "peztool/peztool.hpp"
#include "renderer.hpp"
#include "interpolation/functions.hpp"

/** The scene that will be used for the demo.
 * A scene is a collection of Entities and systems.
 * Entities store data and systems apply transformation on them.
 * Systems can be:
 *  - Processors: applying transformations on data
 *  - Renderers: used to draw data representation on screen
 * This struct is a very simple Scene with no entities and processors.
 * It simply has a basic renderer that draw a circle.
 */
struct DemoScene final : public pez::Scene<pez::EntityPack<>, pez::SystemPack<>, pez::SystemPack<Renderer>>
{
    /// Called after all systems are created
    void onInitialized() override
    {
    }

    /// Defines events callbacks
    void registerEvents(pez::EventHandler& handler) override
    {
        // Exit with Esc
        handler.onKeyPressed(sf::Keyboard::Key::Escape, [](sf::Event::KeyPressed const&) {
            pez::App::exit();
        });
        // Update the circle's position when Space is pressed
        handler.onKeyPressed(sf::Keyboard::Key::Space, [this](sf::Event::KeyPressed const&) {
            // A new arbitrary position
            Vec2f constexpr new_position{960.0f, 540.0f};
            Renderer& renderer{getRenderer<Renderer>()};
            // Set animation's duration
            renderer.circle_position.setDuration(1.25f);
            // Set animation's transition
            renderer.circle_position.transition = TransitionFunction::EaseOutElastic;
            // Assign the new value to circle_position
            renderer.circle_position = new_position;
        });
    }
};