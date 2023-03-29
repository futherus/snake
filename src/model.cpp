#include "model.h"

namespace py
{

Object Field::empty_{ ObjectType::Empty};
Object Field::out_of_borders_{ ObjectType::OutOfBorders};

Snake::Snake( Model* model,
              const std::vector<Vec2i>& pos)
    : Object{ ObjectType::Snake}
    , model_{ model}
    , pos_{ pos}
    , dir_shift_{}
{
    for (auto& pos : pos_)
        model_->getField()->occupy( pos, this);
}

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

bool Snake::tryMove()
{$$
    if (dir_shift_ == Vec2i{0, 0})
        return false;

    Vec2i new_pos = pos_[0] + dir_shift_;
    Field* field = model_->getField();
    Object* tile = field->checkTile(new_pos);

    switch (tile->getType())
    {
        case ObjectType::OutOfBorders:
            return false;

        case ObjectType::Empty:
            field->release( pos_.back());
            pos_.pop_back();
            pos_.insert(pos_.begin(), new_pos);
            field->occupy( new_pos, this);
            return true;

        case ObjectType::Apple:
            static_cast<Apple*>( tile)->destroy();
            pos_.insert(pos_.begin(), new_pos);
            field->occupy( new_pos, this);
            return true;

        case ObjectType::Snake:
            return false;
        default:
            assert(0 && "not handled ObjectType");
            return false;
    }

    assert( 0 && "fallthrough");
}

void Apple::destroy()
{
    model_->getField()->release( pos_);
    model_->addInactive( this);
    is_active_ = false;
}

void Apple::reset()
{$$
    Vec2i new_pos;
    Field* field = model_->getField();
    Vec2i size = field->getSize();

    do
    {
        new_pos = {
            uniform_distr( 0, size.x),
            uniform_distr( 0, size.y),
        };
    } while (field->checkTile(new_pos)->getType() != ObjectType::Empty);

    field->occupy( new_pos, this);
    pos_ = new_pos;
    is_active_ = true;
}

} // namespace py
