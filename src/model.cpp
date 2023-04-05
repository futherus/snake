#include "model.h"

namespace py
{

Object Field::empty_{ ObjectType::Empty};
Object Field::out_of_borders_{ ObjectType::OutOfBorders};

Apple* Field::findClosest( Vec2i pos)
{
    Apple* closest = nullptr;
    int closest_dist2 = INT32_MAX;
    for (size_t row = 0; row < tiles_.size(); row++)
    {
        for (size_t col = 0; col < tiles_[row].size(); col++)
        {
            if (tiles_[row][col]->getType() == ObjectType::Apple)
            {
                Vec2i diff = Vec2i{col, row} - pos;
                int dist2 = diff.x * diff.x + diff.y * diff.y;
                if (dist2 < closest_dist2)
                {
                    closest_dist2 = dist2;
                    closest = static_cast<Apple*>( tiles_[row][col]);
                }
            }
        }
    }

    assert( closest);
    return closest;
}

Snake::Snake( Model* model,
              Color color)
    : Object{ ObjectType::Snake}
    , model_{ model}
    , color_{ color}
    , pos_{}
    , dir_shift_{}
{}

void Snake::destroy()
{
    for (auto& pos : pos_)
        model_->getField()->release( pos);

    pos_.clear();
    model_->addInactive( this);
}

void Snake::addPosition( Vec2i pos)
{
    model_->getField()->occupy( pos, this);

    pos_.push_back( pos);
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
    Object* tile = field->checkTile( new_pos);

    switch (tile->getType())
    {
        case ObjectType::OutOfBorders:
            this->destroy();
            return true;

        case ObjectType::Empty:
            field->release( pos_.back());
            pos_.pop_back();
            pos_.insert( pos_.begin(), new_pos);
            field->occupy( new_pos, this);
            return true;

        case ObjectType::Apple:
            static_cast<Apple*>( tile)->destroy();
            pos_.insert( pos_.begin(), new_pos);
            field->occupy( new_pos, this);
            return true;

        case ObjectType::Snake:
            this->destroy();
            return true;
        default:
            assert( 0 && "not handled ObjectType");
            return false;
    }

    assert( 0 && "fallthrough");
}

void Apple::destroy()
{
    model_->getField()->release( pos_);
    model_->addInactive( this);
    color_ = Color::Red;
}

void Apple::setPosition( Vec2i pos)
{
    Field* field = model_->getField();

    field->occupy( pos, this);
    pos_ = pos;
}

} // namespace py
