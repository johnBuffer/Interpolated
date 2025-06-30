#pragma once
#include "../../peztool.hpp"
#include "../../utils/vec.hpp"
#include "../color_utils.hpp"
#include "./interpolable.hpp"


namespace pez
{

/// Base class for standard InterpolatedData
template<typename TData>
struct InterpolatedData : public Interpolable
{
public:
    InterpolatedData(TData const& value, InterpolationFunction function, float speed)
    {
        setValueDirect(value);
        setInterpolationFunction(function);
        setInterpolationSpeed(speed);
    }

    InterpolatedData(TData const& value, InterpolationFunction function)
        : InterpolatedData(value, function, 1.0f)
    {
    }

    InterpolatedData(TData const& value, float speed)
        : InterpolatedData(value, InterpolationFunction::EaseInOutQuint, speed)
    {
    }

    explicit
    InterpolatedData(TData const& value)
        : InterpolatedData(value, InterpolationFunction::EaseInOutQuint, 1.0f)
    {
    }

    InterpolatedData()
        : InterpolatedData({}, InterpolationFunction::EaseInOutQuint, 1.0f)
    {}

    ~InterpolatedData() override = default;

    /// Sets a new target for the value
    void setValue(TData const& value)
    {
        m_start_value  = getCurrentValue();
        m_target_value = value;
        m_delta        = m_target_value - m_start_value;
        reset();
    }

    /// Instantly sets the current value to the provided one
    void setValueDirect(TData const& value)
    {
        m_start_value  = value;
        m_target_value = value;
        m_delta        = {};
        setDone();
    }

    /// Adds the provided offset to the current value
    void offsetValue(TData const& offset)
    {
        setValue(getCurrentValue() + offset);
    }

    /// Instantly adds the provided offset to the current value
    void offsetValueDirect(TData const& offset)
    {
        setValueDirect(getCurrentValue() + offset);
    }

    /// Casts the value to the underlying type by returning the current value
    operator TData() const
    {
        return getCurrentValue();
    }

    [[nodiscard]]
    virtual TData getCurrentValue() const
    {
        if (!isDone()) {
            return m_start_value + m_delta * getValueRatio();
        }
        return m_target_value;
    }

    InterpolatedData& operator=(TData const& value)
    {
        setValue(value);
        return *this;
    }

    void operator+=(TData const& value)
    {
        offsetValue(value);
    }

protected:
    [[nodiscard]]
    float getTime() const override
    {
        float const time{App::getTime()};
        return time;
    }

private:
    TData m_start_value;
    TData m_target_value;
    TData m_delta;
};

using InterpolatedFloat = InterpolatedData<float>;
using InterpolatedVec2  = InterpolatedData<Vec2f>;
using InterpolatedVec3  = InterpolatedData<Vec3f>;
using InterpolatedVec4  = InterpolatedData<Vec4f>;


/// Specialization for Colors
template<>
struct InterpolatedData<sf::Color> final : public Interpolable
{
public:
    void setValue(sf::Color color)
    {
        m_start_value  = getVec4CurrentValue();
        m_target_value = pez::getVec<Vec4f>(color);
        m_delta        = m_target_value - m_start_value;
        reset();
    }

    void setValueDirect(sf::Color color)
    {
        m_start_value  = pez::getVec<Vec4f>(color);
        m_target_value = m_start_value;
        m_delta        = {};
        setDone();
    }

    void operator=(sf::Color color)
    {
        setValue(color);
    }

    /// Casts the value to the underlying type by returning the current value
    operator sf::Color() const
    {
        return getCurrentValue();
    }

    [[nodiscard]]
    sf::Color getCurrentValue() const
    {
        return pez::getColor(getVec4CurrentValue());
    }

private:
    [[nodiscard]]
    float getTime() const override
    {
        return App::getTime();
    }

    [[nodiscard]]
    Vec4f getVec4CurrentValue() const
    {
        float const ratio = getValueRatio();
        if (ratio < 1.0f) {
            return m_start_value + m_delta * ratio;
        }
        return m_target_value;
    }

    Vec4f m_start_value;
    Vec4f m_target_value;
    Vec4f m_delta;
};

}