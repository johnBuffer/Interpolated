#pragma once
#include <memory>

namespace pez
{

template<typename TInstance>
struct GlobalInstance
{
    static TInstance* instance;
};

template<typename TInstance>
TInstance* GlobalInstance<TInstance>::instance = nullptr;


/// Singleton storage
template<typename TInstance>
struct Singleton
{
    static std::unique_ptr<TInstance> s_instance;

    static TInstance& get()
    {
        return *s_instance;
    }

    template<typename... TArgs>
    static void create(TArgs&&... args)
    {
        s_instance = std::make_unique<TInstance>(std::forward<TArgs>(args)...);
    }
};

template<typename TInstance>
std::unique_ptr<TInstance> Singleton<TInstance>::s_instance = {};

}