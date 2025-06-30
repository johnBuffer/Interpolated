#pragma once
#include "./interpolation.hpp"


namespace pez
{

struct Interpolable
{
public:
    virtual ~Interpolable() = default;

    /// Checks if the interpolation is over
    [[nodiscard]]
    virtual bool isDone() const
    {
        return getTimeRatio() >= 1.0f;
    }

    /// Sets the function used for interpolation
    void setInterpolationFunction(InterpolationFunction function)
    {
        m_function = function;
    }

    /// Sets the interpolation speed
    void setInterpolationSpeed(float speed)
    {
        m_speed = speed;
    }

protected:
    /// Returns the current time ratio
    [[nodiscard]]
    float getTimeRatio() const
    {
        float const current_time = getTime();
        return std::max(0.0f, (current_time - m_start_time) * m_speed);
    }

    /// Returns the current value ratio
    [[nodiscard]]
    float getValueRatio() const
    {
        return Interpolation::getInterpolationValue(getTimeRatio(), m_function);
    }

    /// Sets the current time to now
    void reset()
    {
        m_start_time = getTime();
    }

    /// Sets the start time in order to make the @p Interpolable done
    void setDone()
    {
        // Doubling the value to be sure
        m_start_time = getTime() - 2.0f / m_speed;
    }

    /// Returns the current time. Has to be defined by the user.
    [[nodiscard]]
    virtual float getTime() const = 0;

private:
    /// The starting time for the interpolation
    float m_start_time = 0.0f;
    /// The time multiplier to set interpolation speed
    float m_speed = 1.0f;
    /// The function that will be used for interpolation
    InterpolationFunction m_function = InterpolationFunction::EaseInOutQuint;
};
}
