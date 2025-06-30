#pragma once
#include <cmath>
#include <cstdint>

#include "peztool/utils/math.hpp"

namespace pez
{

// The easing function to use
enum class InterpolationFunction
{
    None,
    Linear,
    EaseInOutExponential,
    EaseInOutCirc,
    EaseInOutQuint,
    EaseOutBack,
    EaseOutElastic,
    EaseInBack,
    Sigmoid
};

struct Interpolation
{
    static float dumbPow(float x, uint32_t p)
    {
        float res = 1.0f;
        for (uint32_t i(p); i--;) {
            res *= x;
        }
        return res;
    }

    static float linear(float t)
    {
        return t;
    }

    static float easeInOut(float t)
    {
        if (t < 0.5f) {
            return std::pow(2.0f, 20.0f * t - 10.0f) * 0.5f;
        }
        return (2.0f - std::pow(2.0f, -20.0f * t + 10.0f)) * 0.5f;
    }

    static float easeOutBack(float t)
    {
        constexpr float c1 = 1.70158f;
        constexpr float c3 = c1 + 1.0f;
        return 1.0f + c3 * dumbPow(t - 1.0f, 3) + c1 * dumbPow(t - 1.0f, 2);
    }

    static float easeInOutQuint(float t)
    {
        if (t < 0.5f) {
            return  16.0f * dumbPow(t, 5);
        }
        return 1.0f - dumbPow(-2.0f * t + 2, 5) * 0.5f;
    }

    static float easeInBack(float t) {
        float const c1 = 1.70158f;
        float const c3 = c1 + 1.0f;

        return c3 * t * t * t - c1 * t * t;
    }

    static float easeOutElastic(float t) {
        float constexpr c4 = Constant32::TwoPi / 3.0f;

        if (t == 0.0f) {
            return 0.0f;
        }

        if (t == 1.0f) {
            return 1.0f;
        }

        return std::pow(2.0f, -10.0f * t) * std::sin((t * 10.0f - 0.75f) * c4) + 1.0f;
    }

    static float getInterpolationValue(float t, InterpolationFunction interpolation)
    {
        switch (interpolation) {
            default:
                return t;
            case InterpolationFunction::None:
                return 1.0f;
            case InterpolationFunction::Linear:
                return t;
            case InterpolationFunction::EaseInOutExponential:
                return Interpolation::easeInOut(t);
            case InterpolationFunction::EaseInOutQuint:
                return Interpolation::easeInOutQuint(t);
            case InterpolationFunction::EaseOutBack:
                return Interpolation::easeOutBack(t);
            case InterpolationFunction::EaseInBack:
                return Interpolation::easeInBack(t);
            case InterpolationFunction::EaseOutElastic:
                return Interpolation::easeOutElastic(t);
        }
    }
};

}
