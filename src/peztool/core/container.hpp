#pragma once
#include <memory>
#include "../utils/index_vector.hpp"

namespace pez
{
/// Managed pointer to object
template<typename T>
using ObjectPtr = std::unique_ptr<T>;
/// Non owning pointer to object
template<typename T>
using ObjectView = T*;


template<typename... TObjects>
using Hub = std::tuple<ObjectPtr<TObjects>...>;

template<typename... TObjects>
using HubView = std::tuple<ObjectView<TObjects>...>;

// Entities
template<typename T>
using EntityContainer = siv::Vector<T>;

template<typename... TEntities>
using EntityHub = Hub<EntityContainer<TEntities>...>;

template<typename... TEntities>
using EntityHubView = HubView<EntityContainer<TEntities>...>;

template<typename... TEntities>
struct EntityPack
{
    EntityHub<TEntities...> hub;

    template<typename T>
    EntityContainer<T>& getContainer()
    {
        using Container = ObjectPtr<EntityContainer<T>>;
        return *std::get<Container>(hub);
    }

    template<typename TEntity>
    TEntity& get(siv::ID id)
    {
        EntityContainer<TEntity>& container = getContainer<TEntity>();
        return container[id];
    }

    template<typename TEntity, typename... TArgs>
    siv::ID create(TArgs&&... args)
    {
        EntityContainer<TEntity>& container{getContainer<TEntity>()};
        siv::ID const id{container.getNextID()};
        return container.emplace_back(id, std::forward<TArgs>(args)...);
    }

    template<typename TEntity>
    size_t getCount()
    {
        return getContainer<TEntity>().size();
    }
};

template<typename... TEntities>
struct RequiredEntity
{
    EntityHubView<TEntities...> hub;

    template<typename T>
    EntityContainer<T>& getContainer()
    {
        using ContainerView = ObjectView<EntityContainer<T>>;
        return *std::get<ContainerView>(hub);
    }

    template<typename TEntity>
    TEntity& get(siv::ID id)
    {
        EntityContainer<TEntity>& container = getContainer<TEntity>();
        return container[id];
    }

    template<typename TEntity, typename... TArgs>
    siv::ID create(TArgs&&... args)
    {
        EntityContainer<TEntity>& container{getContainer<TEntity>()};
        siv::ID const id{container.getNextID()};
        return container.emplace_back(id, std::forward<TArgs>(args)...);
    }

    template<typename TEntity>
    size_t getCount()
    {
        return getContainer<TEntity>().size();
    }
};

// Systems
template<typename... TProcessors>
using SystemHub = Hub<TProcessors...>;

template<typename... TProcessors>
using ProcessorHubView = HubView<TProcessors...>;

template<typename... TSystems>
struct SystemPack
{
    SystemHub<TSystems...> hub;

    template<typename T>
    T& get()
    {
        return *std::get<ObjectPtr<T>>(hub);
    }
};

template<typename... TProcessors>
struct RequiredSystems
{
    ProcessorHubView<TProcessors...> hub;
};
}