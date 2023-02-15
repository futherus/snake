#include "model.h"

namespace py
{

void Snake::tryMove(Shift shift)
{$$
    sf::Vector2u new_pos = pos_[0];
    switch (shift)
    {
        case Shift::Left:
            new_pos.x--;
            break;
        case Shift::Right:
            new_pos.x++;
            break;
        case Shift::Up:
            new_pos.y--;
            break;
        case Shift::Down:
            new_pos.y++;
            break;
    }

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
            assert(0 && "Not handled ObjectType");
            break;
    }
}

} // namespace py
