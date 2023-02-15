#ifndef VECTOR2_H
#define VECTOR2_H

#include <cstdint>
#include <SFML/System/Vector2.hpp>

template <typename T>
class Vector2
{
public:
    T x;
    T y;

    Vector2() = default;

    Vector2(T X, T Y)
        : x(X), y(Y)
    {}

    Vector2 operator-() const
    {
        return {-x, -y};
    }

    Vector2& operator+=(const Vector2<T>& rhs)
    {
        x += rhs.x;
        y += rhs.y;

        return *this;
    }

    Vector2& operator-=(const Vector2<T>& rhs)
    {
        x -= rhs.x;
        y -= rhs.y;

        return *this;
    }

    Vector2& operator*=(const T& rhs)
    {
        x *= rhs;
        y *= rhs;

        return *this;
    }

    Vector2& operator/=(const T& rhs)
    {
        x /= rhs;
        y /= rhs;

        return *this;
    }

    bool operator==(const Vector2& rhs) const
    {
        return (x == rhs.x) && (y == rhs.y);
    }

    operator sf::Vector2<T>()
    {
        return {x, y};
    }

    Vector2(const sf::Vector2<T>& that)
        : x(that.x), y(that.y)
    {}
};

template <typename T>
inline Vector2<T> operator+(Vector2<T> lhs, const Vector2<T>& rhs)
{
    return lhs += rhs;
}

template <typename T>
inline Vector2<T> operator-(Vector2<T> lhs, const Vector2<T>& rhs)
{
    return lhs -= rhs;
}

template <typename T>
inline Vector2<T> operator*(Vector2<T> lhs, const T& rhs)
{
    return lhs *= rhs;
}

template <typename T>
inline Vector2<T> operator*(const T& lhs, Vector2<T> rhs)
{
    return rhs *= lhs;
}

template <typename T>
inline Vector2<T> operator/(Vector2<T> lhs, const T& rhs)
{
    return lhs /= rhs;
}

using Vector2u = Vector2<uint32_t>;
using Vector2i = Vector2<int32_t>;

#endif // VECTOR2_H
