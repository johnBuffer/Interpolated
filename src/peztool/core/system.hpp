#pragma once
#include <tuple>

#include "../utils/thread_pool.hpp"
#include "../utils/signal.hpp"
#include "./container.hpp"
#include "./render.hpp"
#include "./scene.hpp"
#include "./entity.hpp"

namespace pez
{

/// Base system class
template<typename TRequiredEntity = RequiredEntity<>, typename TRequiredProcessor = RequiredSystems<>,
         typename TRequiredRenderers = RequiredSystems<>>
class System
{
private:
    TRequiredEntity    m_entities;
    TRequiredProcessor m_processors;
    TRequiredRenderers m_renderers;

public:
    virtual ~System() = default;

    void setScene(SceneBase* scene)
    {
        m_scene_base = scene;
    }

    template<typename TProvided>
    void loadEntities(TProvided& provided)
    {
        std::apply([this, &provided](auto&&... args) { (updatePointer(args, provided), ...); }, m_entities.hub);
    }

    template<typename TProvided>
    void loadProcessors(TProvided& provided)
    {
        std::apply([this, &provided](auto&&... args) { (updatePointer(args, provided), ...); }, m_processors.hub);
    }

    template<typename TProvided>
    void loadRenderers(TProvided& provided)
    {
        std::apply([this, &provided](auto&&... args) { (updatePointer(args, provided), ...); }, m_renderers.hub);
    }

    virtual void onInitialized()
    {

    }

    [[nodiscard]]
    size_t getExecutionTimeUs() const
    {
        return m_execution_time_us;
    }

    [[nodiscard]]
    float getExecutionTimeMs() const
    {
        return static_cast<float>(m_execution_time_us) * 0.001f;
    }

protected:
    template<typename TEntity>
    TEntity& get(size_t id)
    {
        return m_entities.template get<TEntity>(id);
    }

    template<typename TEntity, typename... TArgs>
    siv::ID create(TArgs&&... args)
    {
        return m_entities.template create<TEntity>(std::forward<TArgs>(args)...);
    }

    template<typename TProcessor>
    TProcessor& getProcessor() const
    {
        TProcessor* ptr{std::get<ObjectView<TProcessor>>(m_processors.hub)};
        // The processor has not been fetched yet, this call should not be made before onInitialized()
        assert(ptr != nullptr);
        return *ptr;
    }

    template<typename TRenderer>
    TRenderer& getRenderer() const
    {
        TRenderer* ptr{std::get<ObjectView<TRenderer>>(m_renderers.hub)};
        // The renderer has not been fetched yet, this call should not be made before onInitialized()
        assert(ptr != nullptr);
        return *ptr;
    }

    template<typename T, typename TProvided>
    void updatePointer(ObjectView<T>& required, TProvided& provided)
    {
        auto& fetched = std::get<ObjectPtr<T>>(provided);
        required      = fetched.get();
    }

    template<typename TEntity, typename TCallback>
    void foreach(TCallback&& callback)
    {
        auto& container = m_entities.template getContainer<TEntity>();
        for (auto& entity : container) {
            callback(entity);
        }
    }

    template<typename TEntity, typename TCallback>
    void parallelForeachEnumerate(TCallback&& callback) {
        static_assert(std::is_convertible_v<TEntity*, Entity*>, "Can only iterate on Entity derived objects");
        auto& data = m_entities.template getContainer<TEntity>().getData();
        auto const count = data.size();

        auto& tp{Singleton<ThreadPool>::get()};
        tp.dispatch(count, [&data, callback](uint32_t const start, uint32_t const end) {
            for (uint32_t i{start}; i < end; ++i) {
                if (!data[i].removeRequested()) {
                    callback(i, data[i]);
                }
            }
        });
    }

    template<typename TEntity, typename TCallback>
    void parallelForeach(TCallback&& callback) {
        static_assert(std::is_convertible_v<TEntity*, Entity*>, "Can only iterate on Entity derived objects");
        auto& data = m_entities.template getContainer<TEntity>().getData();
        auto const count = data.size();

        auto& tp{Singleton<ThreadPool>::get()};
        tp.dispatch(count, [&data, callback](uint32_t const start, uint32_t const end) {
            for (uint32_t i{start}; i < end; ++i) {
                if (!data[i].removeRequested()) {
                    callback(data[i]);
                }
            }
        });
    }

    template<typename TEntity>
    size_t getCount()
    {
        return m_entities.template getCount<TEntity>();
    }

    void startTimer()
    {
        m_clock.restart();
    }

    void stopTimer()
    {
        m_execution_time_us = m_clock.getElapsedTime().asMicroseconds();
    }

    template<typename TSignal>
    void emit(TSignal const& signal)
    {
        Dispatcher<TSignal>::emit(signal);
    }

    template<typename TSignal, typename TSub>
    void subscribe(TSub* subscriber)
    {
        Dispatcher<TSignal>::subscribe([subscriber](TSignal const& signal) {
            subscriber->onSignal(signal);
        });
    }

protected:
    SceneBase* m_scene_base = nullptr;

    /// Clocked used to perform profiling
    sf::Clock m_clock;
    size_t m_execution_time_us{0};
};

/// Base class for all processors
template<typename TRequiredEntity = RequiredEntity<>, typename TRequiredProcessor = RequiredSystems<>, typename TRequiredRenderers = RequiredSystems<>>
class Processor : public System<TRequiredEntity, TRequiredProcessor, TRequiredRenderers>
{
public:
    using SystemBase = System<TRequiredEntity, TRequiredProcessor, TRequiredRenderers>;

    virtual ~Processor() = default;

    void updateInternal(float dt)
    {
        if (App::isRunning() || ignore_pause) {
            SystemBase::startTimer();
            update(dt);
            SystemBase::stopTimer();
        }
    }

    virtual void update(float dt) = 0;

    bool ignore_pause{false};
};

/// Base class for all renderers
template<typename TRequiredEntity = RequiredEntity<>, typename TRequiredProcessor = RequiredSystems<>, typename TRequiredRenderers = RequiredSystems<>>
class Renderer : public System<TRequiredEntity, TRequiredProcessor, TRequiredRenderers>
{
public:
    using SystemBase = System<TRequiredEntity, TRequiredProcessor, TRequiredRenderers>;

    virtual ~Renderer() = default;

    void renderInternal(RenderContext& context)
    {
        SystemBase::startTimer();
        render(context);
        SystemBase::stopTimer();
    }

    virtual void render(RenderContext& context) = 0;

    void setStore(ResourcesStore const& store)
    {
        m_resources = &store;
    }

    void setZoom(float zoom)
    {
        SystemBase::m_scene_base->setZoom(zoom);
    }

    void setCameraPosition(Vec2f position)
    {
        SystemBase::m_scene_base->setCameraPosition(position);
    }

    [[nodiscard]]
    sf::Font const* getFont(std::string const& name) const
    {
        // This method should not be called before onInitialized
        assert(m_resources != nullptr);
        return m_resources->getFont(name);
    }

    [[nodiscard]]
    sf::Texture const* getTexture(std::string const& name) const
    {
        return m_resources->getTexture(name);
    }

private:
    ResourcesStore const* m_resources = nullptr;
};
}  // namespace pez
