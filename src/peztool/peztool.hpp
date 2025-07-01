#pragma once
#include <SFML/Graphics.hpp>

#include "peztool/core/scene.hpp"
#include "peztool/core/static_interface.hpp"
#include "utils/thread_pool.hpp"


namespace pez
{
/** This class is responsible for storing entities, renderers, and processors
 *
 */
class App
{
public:
    App(sf::Vector2u window_size, sf::Vector2u render_size, std::string const& title, sf::State state, uint32_t thread_count = 1)
        : m_window{sf::VideoMode{{window_size.x, window_size.y}}, title, sf::Style::Default, state, []{
                sf::ContextSettings settings{};
                settings.antiAliasingLevel = 8;
                return settings;
            }()}
        , m_render_size{render_size}
    {
        setTickRate(120, true);
        setMouseCursorVisible(true);

        // Create default singletons
        // The number of threads to use
        uint32_t thread_to_use;
        if (thread_count > 1) {
            thread_to_use = thread_count - 1;
        } else if (thread_count == 0) {
            thread_to_use = std::thread::hardware_concurrency();
        } else {
            thread_to_use = 1;
        }
        std::cout << "Using " << thread_to_use << " threads for multithreading." << std::endl;
        Singleton<ThreadPool>::create(thread_to_use);

        GlobalInstance<App>::instance = this;
    }

    void setTickRate(uint32_t tick_rate, bool sync_window_frame_limit)
    {
        m_tick_rate = tick_rate;
        if (sync_window_frame_limit) {
            setWindowFrameRateLimit(tick_rate);
        }
        m_dt = 1.0f / static_cast<float>(m_tick_rate);
    }

    void setWindowFrameRateLimit(uint32_t const frame_rate_limit)
    {
        m_window.setFramerateLimit(frame_rate_limit);
    }

    void setMouseCursorVisible(bool const b) { m_window.setMouseCursorVisible(b); }

    /// Processes events and checks if the apps must exit
    void run()
    {
        while (m_window.isOpen()) {
            tick(m_dt);
        }
    }

    /// Closes the window and stops the application
    void close() { m_window.close(); }

    void tick(float const dt)
    {
        if (m_current_scene) {
            m_current_scene->setRunning(m_running);
            m_current_scene->tick(dt);
        } else {
            std::cout << "No scene, exiting" << std::endl;
            exit();
        }
        if (m_running) {
            // Update time
            m_time += dt;
        }
    }

    void toggleMaxFramerateInternal()
    {
        if (m_frame_rate_unlocked) {
            setWindowFrameRateLimit(m_tick_rate);
        } else {
            setWindowFrameRateLimit(0);
        }

        m_frame_rate_unlocked = !m_frame_rate_unlocked;
    }

    template<typename TScene, typename... TArgs>
    TScene& addScene(TArgs&&... args)
    {
        static_assert(std::is_base_of_v<SceneBase, TScene>);
        m_current_scene = std::make_unique<TScene>(std::forward<TArgs>(args)...);;
        m_current_scene->initialize(m_window, m_render_size);
        return dynamic_cast<TScene&>(*m_current_scene);
    }

    static float getTime()
    {
        return GlobalInstance<App>::instance->m_time;
    }

    static void exit()
    {
        GlobalInstance<App>::instance->close();
    }

    static sf::Vector2u getRenderSize()
    {
        return GlobalInstance<App>::instance->m_render_size;
    }

    static void toggleMaxFramerate()
    {
        GlobalInstance<App>::instance->toggleMaxFramerateInternal();
    }

    static void setFramerateLimit(uint32_t const frame_rate_limit)
    {
        GlobalInstance<App>::instance->m_window.setFramerateLimit(frame_rate_limit);
        GlobalInstance<App>::instance->m_frame_rate_unlocked = false;
    }

    static ThreadPool& getThreadPool()
    {
        return Singleton<ThreadPool>::get();
    }

    static void togglePause()
    {
        GlobalInstance<App>::instance->m_running = !(GlobalInstance<App>::instance->m_running);
    }

    static bool isRunning()
    {
        return GlobalInstance<App>::instance->m_running;
    }

private:
    sf::RenderWindow m_window;
    sf::Vector2u m_render_size;

    uint32_t m_tick_rate;
    float    m_dt;
    float    m_time = 0.0f;

    bool m_running = true;
    bool m_frame_rate_unlocked = false;

    std::unique_ptr<SceneBase> m_current_scene = nullptr;
};

}  // namespace pez
