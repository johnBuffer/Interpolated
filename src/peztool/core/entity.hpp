#pragma once
#include "peztool/utils/index_vector.hpp"

namespace pez
{

struct Entity
{
    explicit
    Entity(siv::ID const id)
        : m_id{id}
    {}
    virtual ~Entity() = default;

    [[nodiscard]]
    siv::ID getID() const
    {
        return m_id;
    }

    void remove()
    {
        m_remove_requested = true;
    }

    [[nodiscard]]
    bool removeRequested() const
    {
        return m_remove_requested;
    }

private:
    siv::ID m_id;
    bool m_remove_requested{false};
};

}