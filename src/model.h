#ifndef MODEL_H
#define MODEL_H

#include <vector>
#include <list>
#include <memory>
#include <algorithm>
#include <assert.h>

#include "debug.h"
#include "rand.h"
#include "vector2.h"

namespace py
{

class Field;

enum class ObjectType
{
    OutOfBorders,
    Empty,
    Snake,
    Apple
};

class Apple final
{
private:
    Vector2u pos_;

public:
    Apple(Vector2u pos = {0, 0})
        : pos_(pos)
    {}

    void setPos(Vector2u pos) { pos_ = pos; }
    bool occupies(Vector2u pos) const { return pos_ == pos; }
};

class Snake final
{
private:
    std::vector<Vector2u> pos_;

    Field* field_;

public:
    Snake(std::vector<Vector2u>&& pos)
        : pos_(pos)
        , field_()
    {}

    void setField(Field* field) { field_ = field; }

    enum class Shift
    {
        Left,
        Right,
        Up,
        Down
    };

    bool occupies(Vector2u pos)
    {
        return std::find(pos_.begin(), pos_.end(), pos) != pos_.end();
    }

    void tryMove(Shift shift);
};

class Field final
{
private:
    Vector2u size_;

    Apple* apple_;
    Snake* snake_;

public:
    Field(Vector2u size, Apple* apple, Snake* snake)
        : size_(size)
        , apple_(apple)
        , snake_(snake)
    {}

    Vector2u getSize() { return size_; }

    ObjectType checkTile(Vector2u pos) const
    {
        if (!contains(pos))
        {
            // $M("Out of borders\n");
            return ObjectType::OutOfBorders;
        }
        if (apple_->occupies(pos))
        {
            // $M("Apple\n");
            return ObjectType::Apple;
        }
        if (snake_->occupies(pos))
        {
            // $M("Snake\n");
            return ObjectType::Snake;
        }

        // $M("Empty\n");
        return ObjectType::Empty;
    }

    void resetApple()
    {$$
        Vector2u new_pos;

        do
        {
            new_pos = {
                uniform_distr(0, size_.x),
                uniform_distr(0, size_.y),
            };
        } while (checkTile(new_pos) != ObjectType::Empty);

        apple_->setPos(new_pos);
    }

    bool contains(Vector2u pos) const { return pos.x < size_.x && pos.y < size_.y; }

};

}

#endif // MODEL_H