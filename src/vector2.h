#ifndef VEC2_H
#define VEC2_H

#include <cstdint>
#include <SFML/System/Vector2.hpp>

template <typename T>
class Vec2
{
public:
    T x;
    T y;

    Vec2() = default;

    Vec2( T X, T Y)
        : x(X), y(Y)
    {}

    template <typename U>
    explicit Vec2( const Vec2<U>& vector)
        : x( static_cast<T>( vector.x))
        , y( static_cast<T>( vector.y))
    {}

    Vec2 operator-() const
    {
        return {-x, -y};
    }

    Vec2& operator+=( const Vec2<T>& rhs)
    {
        x += rhs.x;
        y += rhs.y;

        return *this;
    }

    Vec2& operator-=( const Vec2<T>& rhs)
    {
        x -= rhs.x;
        y -= rhs.y;

        return *this;
    }

    Vec2& operator*=( const T& rhs)
    {
        x *= rhs;
        y *= rhs;

        return *this;
    }

    Vec2& operator/=( const T& rhs)
    {
        x /= rhs;
        y /= rhs;

        return *this;
    }

    bool operator==( const Vec2& rhs) const
    {
        return (x == rhs.x) && (y == rhs.y);
    }

    bool operator!=( const Vec2& rhs) const
    {
        return !(*this == rhs);
    }

    operator sf::Vector2<T>()
    {
        return {x, y};
    }

    Vec2( const sf::Vector2<T>& that)
        : x{ that.x}
        , y{ that.y}
    {}
};

template <typename T>
inline Vec2<T> operator+( Vec2<T> lhs, const Vec2<T>& rhs)
{
    return lhs += rhs;
}

template <typename T>
inline Vec2<T> operator-( Vec2<T> lhs, const Vec2<T>& rhs)
{
    return lhs -= rhs;
}

template <typename T>
inline Vec2<T> operator*( Vec2<T> lhs, const T& rhs)
{
    return lhs *= rhs;
}

template <typename T>
inline Vec2<T> operator*( const T& lhs, Vec2<T> rhs)
{
    return rhs *= lhs;
}

template <typename T>
inline Vec2<T> operator/( Vec2<T> lhs, const T& rhs)
{
    return lhs /= rhs;
}

using Vec2u = Vec2<uint32_t>;
using Vec2i = Vec2<int32_t>;

#endif // VEC2_H
