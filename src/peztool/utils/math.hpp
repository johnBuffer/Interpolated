#pragma once
#include <cmath>


namespace pez
{

template<typename TReal>
struct Constant
{
    static constexpr TReal Pi = TReal{3.14159265359};
    static constexpr TReal TwoPi = TReal{2} * Pi;
    static constexpr TReal HalfPi = Pi * TReal{0.5};
    static constexpr TReal GoldenRatio = TReal{1.6180339887};
};

using Constant32 = Constant<float>;
using Constant64 = Constant<double>;

template<typename TReal>
TReal radToDeg(TReal rad)
{
    constexpr TReal factor{180.0f / Constant<TReal>::Pi};
    return rad * factor;
}

template<typename TReal>
TReal degToRad(TReal deg)
{
    constexpr TReal factor{Constant<TReal>::Pi / 180.0f};
    return deg * factor;
}

template <template<typename > class TVec, class TReal>
TReal dot(TVec<TReal> a, TVec<TReal> b)
{
    return a.x * b.x + a.y * b.y;
}

template <template<typename > class TVec, class TReal>
TReal cross(TVec<TReal> a, TVec<TReal> b)
{
    return a.x * b.y - a.y * b.x;
}

template <template<typename > class TVec, class TReal>
TReal length2(TVec<TReal> v)
{
    return v.x * v.x + v.y * v.y;
}

template <template<typename > class TVec, class TReal>
TReal length(TVec<TReal> v)
{
    return std::sqrt(length2(v));
}

template <template<typename > class TVec, class TReal>
TVec<TReal> normalize(TVec<TReal> v)
{
    TReal const inv_length{TReal{1} / length(v)};
    return {v.x * inv_length, v.y * inv_length};
}

template <template<typename > class TVec, class TReal>
TVec<TReal> normal(TVec<TReal> v)
{
    return {-v.y, v.x};
}

template <template<typename > class TVec, class TReal>
TVec<TReal> rotate(TVec<TReal> v, TReal theta)
{

    TReal const cs = cos(theta);
    TReal const sn = sin(theta);

    TReal const nx = v.x * cs - v.y * sn;
    TReal const ny = v.x * sn + v.y * cs;

    return {nx, ny};
}

template <template<typename > class TVec, class TReal>
TReal angle(TVec<TReal> a, TVec<TReal> b)
{
    return std::atan2(cross(a, b), dot(a, b));
}

template <template<typename > class TVec, class TReal>
TReal angle(TVec<TReal> a)
{
    //return std::atan2(a.y, a.x);
    return angle({1.0f, 0.0f}, a);
}

}
