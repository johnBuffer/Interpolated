#pragma once
#include <iostream>
#include <map>
#include <memory>

#include <SFML/Graphics.hpp>


namespace pez
{

template<typename TStorage>
struct Store
{
public:
    Store() = default;

    TStorage* create(std::string name)
    {
        if (find(name)) {
            std::cout << "[ERROR] Name '" << name << "' already used" << std::endl;
            return nullptr;
        }
        auto obj = std::make_unique<TStorage>();
        TStorage* ptr = obj.get();
        m_store[name] = std::move(obj);
        return ptr;
    }

    [[nodiscard]]
    TStorage const* get(std::string const& asset_name) const noexcept
    {
        TStorage const* ptr = find(asset_name);
        if (!ptr) {
            std::cout << "[WARNING] Could not find '" << asset_name << "'" << std::endl;
        }
        return ptr;
    }

    [[nodiscard]]
    TStorage const* find(std::string const& asset_name) const noexcept
    {
        auto it = m_store.find(asset_name);
        if (it != m_store.end()) {
            return it->second.get();
        }
        return nullptr;
    }

private:
    std::map<std::string, std::unique_ptr<TStorage>> m_store;
};

}