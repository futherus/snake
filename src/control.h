#ifndef CONTROL_H
#define CONTROL_H

#include <array>
#include "model.h"

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
    HumanControl( Keyboard::Key up,
                  Keyboard::Key left,
                  Keyboard::Key down,
                  Keyboard::Key right)
        : IControl{}
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
    int delay_;

public:
    AiControl( int delay)
        : delay_{ delay}
    {}

    void onEvent( const Event& event)
    {
        static int tick = 0;
        static Snake::Direction dir = Snake::Direction::Down;

        if (tick++ == delay_)
        {
            tick = 0;
            Snake::Direction new_dir;
            do
            {
                new_dir = static_cast<Snake::Direction>( uniform_distr(0, 3));
            }
            while (new_dir == dir);

            dir = new_dir;
            snake_->setDirection( dir);
        }
    }
};

} // namespace py


#endif // CONTROL_H