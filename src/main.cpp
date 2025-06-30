#include "./demo_scene.hpp"
#include "peztool/peztool.hpp"

int main()
{
    // Define windows and renderer constants
    Vec2u constexpr window_size{1920, 1080};
    Vec2u constexpr render_size{window_size};
    // Create the app
    pez::App app(window_size, render_size, "InterpolatedDemo", sf::State::Windowed);
    app.addScene<DemoScene>();
    // Start the main loop
    app.run();
    return 0;
}
