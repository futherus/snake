#include "model.h"

namespace py
{

void Snake::setDirection(Direction direction)
{$$
    switch (direction)
    {
        case Direction::Left:
            dir_shift_ = {-1, 0};
            break;
        case Direction::Right:
            dir_shift_ = {1, 0};
            break;
        case Direction::Up:
            dir_shift_ = {0, -1};
            break;
        case Direction::Down:
            dir_shift_ = {0, 1};
            break;
        default:
            assert(0 && "not handled Direction");
            break;
    }
}

void Snake::tryMove()
{$$
    if (dir_shift_ == Vec2i{0, 0})
        return;

    Vec2i new_pos = pos_[0] + dir_shift_;

    ObjectType tile = field_->checkTile(new_pos);

    switch (tile)
    {
        case ObjectType::OutOfBorders:
            break;
        case ObjectType::Empty:
            pos_.pop_back();
            pos_.insert(pos_.begin(), new_pos);
            break;
        case ObjectType::Apple:
            pos_.insert(pos_.begin(), new_pos);
            field_->resetApple();
            break;
        case ObjectType::Snake:
            break;
        default:
            assert(0 && "not handled ObjectType");
            break;
    }
}

} // namespace py
