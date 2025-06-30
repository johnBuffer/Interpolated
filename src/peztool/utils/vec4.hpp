#pragma once

template<typename TData>
struct Vector4
{
    TData x{};
    TData y{};
    TData z{};
    TData w{};

    Vector4() = default;

    Vector4(TData x_, TData y_, TData z_, TData w_)
        : x{x_}, y{y_}, z{z_}, w{w_}
    {}

    Vector4& operator-=(Vector4 const& other)
    {
        x -= other.x;
        y -= other.y;
        z -= other.z;
        w -= other.w;
        return *this;
    }

    Vector4& operator*=(TData f)
    {
        x *= f;
        y *= f;
        z *= f;
        w *= f;
        return *this;
    }
};

template<typename TData>
Vector4<TData> operator*(Vector4<TData> v, TData f)
{
    return {v.x * f, v.y * f, v.z * f, v.w * f};
}

template<typename TData>
bool operator==(Vector4<TData> const& a, Vector4<TData> const& b)
{
    return (a.x == b.x) && (a.y == b.y) && (a.z == b.z) && (a.w == b.w);
}

template<typename TData>
Vector4<TData> operator+(Vector4<TData> const& a, Vector4<TData> const& b)
{
    return {a.x + b.x, a.y + b.y, a.z + b.z, a.w + b.w};
}

template<typename TData>
Vector4<TData> operator-(Vector4<TData> const& a, Vector4<TData> const& b)
{
    return {a.x - b.x, a.y - b.y, a.z - b.z, a.w - b.w};
}