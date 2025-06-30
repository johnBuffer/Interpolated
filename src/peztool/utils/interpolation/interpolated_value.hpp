#pragma once
#include "peztool/core/vec.hpp"
#include "peztool/peztool.hpp"

#include "./interpolation.hpp"

namespace pez
{

template<typename TValueType>
struct InterpolatedValue
{
    static constexpr float time_margin = 0.0f;

    InterpolationFunction interpolation_function = InterpolationFunction::EaseInOutExponential;

    // Data m_attributes
    float start_time = 0.0f;

    TValueType start_value = {};
    TValueType target_value = {};

private:
    float m_speed = 1.0f;

public:
    InterpolatedValue() = default;

    explicit
    InterpolatedValue(const TValueType &value)
    {
        setValueInstant(value);
    }

    InterpolatedValue(const TValueType &value, float speed)
        : start_value{value}, m_speed{speed}
    {}

    void setSpeed(float speed) {
        /*const float now = pez::core::getTime();
        start_time = now - (now - start_time) * (speed / speed_);*/
        m_speed = speed;
    }

    [[nodiscard]]
    TValueType get() const
    {
        return getCurrentValue();
    }

    void setValueInstant(const TValueType &value)
    {
        start_value = value;
        target_value = value;
        // Ensure time is over
        updateStartTime(2.0f / m_speed);
    }

    InterpolatedValue& operator=(const TValueType& new_value)
    {
        setValue(new_value);
        return *this;
    }

    void operator+=(const TValueType &value) {
        this->operator=(target_value + value);
    }

    [[nodiscard]]
    operator TValueType() const {
        return getCurrentValue();
    }

    [[nodiscard]]
    float getElapsedTime() const {
        return (App::getTime() - start_time) * m_speed;
    }

    [[nodiscard]]
    float getCurrentT() const {
        const float t = getElapsedTime();
        return Interpolation::getInterpolationValue(t, interpolation_function);
    }

    [[nodiscard]]
    TValueType getCurrentValue() const {
        const float t = getCurrentT();
        if (getElapsedTime() < 1.0f) {
            const float inv_ratio = 1.0f - t;
            return start_value * inv_ratio + target_value * t;
        }
        return target_value;
    }

    [[nodiscard]]
    bool isDone() const {
        float const elapsed = getElapsedTime();

        return (elapsed > (1.0f + time_margin)) ||
               (interpolation_function == InterpolationFunction::None) ||
               (start_value == target_value) ||
               (getCurrentValue() == target_value);
    }

    void updateStartTime(float offset = 0.0f) {
        start_time = App::getTime() - offset;
    }

    [[nodiscard]]
    const TValueType& getTargetValue() const {
        return target_value;
    }

    void setValue(TValueType const& new_value, float speed, InterpolationFunction interpolation) {
        if (interpolation == InterpolationFunction::None) {
            start_value = new_value;
        } else {
            start_value = getCurrentValue();
        }
        target_value = new_value;
        updateStartTime();
        // Update settings
        m_speed = speed;
        interpolation_function = interpolation;
    }

    void setInterpolation(InterpolationFunction interpolation) {
        interpolation_function = interpolation;
    }

    void setValue(const TValueType &new_value) {
        start_value = getCurrentValue();
        target_value = new_value;
        updateStartTime();
    }
};

using OldInterpolatedFloat = InterpolatedValue<float>;
using OldInterpolatedVec2  = InterpolatedValue<Vec2f>;
using OldInterpolatedVec3  = InterpolatedValue<Vec3f>;
using OldInterpolatedVec4  = InterpolatedValue<Vec4f>;

}
