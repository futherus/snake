#ifndef CONTROL_H
#define CONTROL_H

#include <array>
#include "model.h"
#include "rand.h"

namespace py
{

class HumanControl : public IControl
{
private:
    Keyboard::Key left_;
    Keyboard::Key right_;
    Keyboard::Key up_;
    Keyboard::Key down_;

public:
    HumanControl( const Model* model,
                  Keyboard::Key up,
                  Keyboard::Key left,
                  Keyboard::Key down,
                  Keyboard::Key right)
        : IControl{ model}
        , left_{ left}
        , right_{ right}
        , up_{ up}
        , down_{ down}
    {}

    void onEvent( const Event& event)
    {
        if (event.type != Event::KeyPressed)
            return;

        if (event.key.code == left_)
            snake_->setDirection( Snake::Direction::Left);
        else if (event.key.code == right_)
            snake_->setDirection( Snake::Direction::Right);
        else if (event.key.code == up_)
            snake_->setDirection( Snake::Direction::Up);
        else if (event.key.code == down_)
            snake_->setDirection( Snake::Direction::Down);
    }
};

class AiControl : public IControl
{
private:
    std::array<Vec2i, 4> getDirections( Vec2i diff)
    {
        std::array<Vec2i, 4> priors;

        if (diff.x == 0)
        {
            assert( diff.y && "difference is zero");

            int ysgn = signum( diff.y);
            priors[0] = { 0,  ysgn};
            priors[1] = { 1,     0};
            priors[2] = {-1,     0};
            priors[3] = { 0, -ysgn};
        }
        else if (diff.y == 0)
        {
            assert( diff.x && "difference is zero");

            int xsgn = signum( diff.x);
            priors[0] = { xsgn,  0};
            priors[1] = {    0,  1};
            priors[2] = {    0, -1};
            priors[3] = {-xsgn,  0};

        }
        else if (std::abs( diff.x) > std::abs( diff.y))
        {
            int xsgn = signum( diff.x);
            int ysgn = signum( diff.y);
            priors[0] = { xsgn,     0};
            priors[1] = {    0,  ysgn};
            priors[2] = {-xsgn,     0};
            priors[3] = {    0, -ysgn};
        }
        else
        {
            int xsgn = signum( diff.x);
            int ysgn = signum( diff.y);
            priors[0] = {    0,  ysgn};
            priors[1] = { xsgn,     0};
            priors[2] = {    0, -ysgn};
            priors[3] = {-xsgn,     0};
        }

        return priors;
    }

public:
    AiControl( const Model* model)
        : IControl{ model}
    {}

    void onTurn()
    {
        auto&& pos = snake_->getPosition();
        const Field* field = model_->getField();
        const Apple* apple = field->findClosest( pos.front());
        Vec2i apple_pos = apple->getPosition();
        $M( "closest apple = (%d, %d)\n", apple_pos.x, apple_pos.y);

        auto dirs = getDirections( apple_pos - pos.front());
        for (auto dir : dirs)
        {
            $M("(%d, %d) ", dir.x, dir.y);
        }
        $M("\n");
        for (auto dir : dirs)
        {
            auto type = field->checkTile( pos.front() + dir)->getType();
            if ( type == ObjectType::Empty || type == ObjectType::Apple)
            {
                snake_->setDirection( dir);

                break;
            }
        }
    }
};

} // namespace py


#endif // CONTROL_H