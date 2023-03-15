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
    Vec2i pos_;

public:
    Apple(Vec2i pos = {0, 0})
        : pos_(pos)
    {}

    void setPos(Vec2i pos) { pos_ = pos; }
    bool occupies(Vec2i pos) const { return pos_ == pos; }
};

class Snake final
{
public:
    enum class Direction
    {
        Left,
        Right,
        Up,
        Down
    };

private:
    std::vector<Vec2i> pos_;

    Field* field_;

    Vec2i dir_shift_;

public:
    Snake(std::vector<Vec2i>&& pos)
        : pos_(pos)
        , field_()
        , dir_shift_()
    {}

    void setField(Field* field) { field_ = field; }

    bool occupies(Vec2i pos)
    {
        return std::find(pos_.begin(), pos_.end(), pos) != pos_.end();
    }

    void tryMove();
    void setDirection(Direction direction);
};

class Field final
{
private:
    Vec2i size_;

    Apple* apple_;
    Snake* snake_;

public:
    Field(Vec2i size, Apple* apple, Snake* snake)
        : size_(size)
        , apple_(apple)
        , snake_(snake)
    {}

    Vec2i getSize() { return size_; }

    ObjectType checkTile(Vec2i pos) const
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
        Vec2i new_pos;

        do
        {
            new_pos = {
                uniform_distr(0, size_.x),
                uniform_distr(0, size_.y),
            };
        } while (checkTile(new_pos) != ObjectType::Empty);

        apple_->setPos(new_pos);
    }

    bool contains(Vec2i pos) const { return 0 <= pos.x && pos.x < size_.x &&
                                            0 <= pos.y && pos.y < size_.y; }

};

}

#endif // MODEL_H