#pragma once
#include <map>
#include <functional>
#include <iostream>

#include "SFML/Window/Window.hpp"


namespace pez
{

/**
 *
 */
class EventHandler
{
public:
    using KeyPressedCallback = std::function<void(sf::Event::KeyPressed)>;

    explicit
    EventHandler(sf::Window& window)
        : m_window{&window}
    {
        m_main_on_key_pressed_callback = [this](sf::Event::KeyPressed const event) {
            auto const it{m_key_callbacks.find(event.code)};
            if (it != m_key_callbacks.end()) {
                it->second(event);
            }
        };
    }

    void processEvents()
    {
        m_window->handleEvents(m_main_on_key_pressed_callback);
    }

    void onKeyPressed(sf::Keyboard::Key key_code, KeyPressedCallback const& callback)
    {
        m_key_callbacks[key_code] = callback;
    }

private:
    sf::Window* m_window = nullptr;

    KeyPressedCallback m_main_on_key_pressed_callback;

    std::unordered_map<sf::Keyboard::Key, KeyPressedCallback> m_key_callbacks;
};

}
