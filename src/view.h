#ifndef VIEW_H
#define VIEW_H

#include <iostream>
#include <ncurses.h>
#include "model.h"

namespace py
{

class IView
{
protected:
    Field* field_;
    Snake* snake_;
    Apple* apple_;

public:
    IView(Field* field,
          Snake* snake,
          Apple* apple)
        : field_(field)
        , snake_(snake)
        , apple_(apple)
    {}

    virtual ~IView() = default;

    virtual void draw() = 0;
};

// class GuiView final : public IView
// {
// private:

// public:
//     GuiView() = default;

//     void draw() override { std::printf("Not implemented\n"); }

// };

class TuiView final : public IView
{
private:
    WINDOW* win_;
    sf::Vector2u win_pos_;
    sf::Vector2u win_size_;

public:
    TuiView(Field* field,
            Snake* snake,
            Apple* apple)
        : IView(field, snake, apple)
        , win_()
    {}

    void run()
    {
        initscr();
        cbreak();
        noecho();
        keypad(stdscr, TRUE);
        refresh();

        sf::Vector2u sz = field_->getSize();
        win_size_ = sz + sf::Vector2u{2, 2};

        win_pos_.x = (COLS  - win_size_.x) / 2;
        win_pos_.y = (LINES - win_size_.y) / 2;

        win_ = newwin(win_size_.y, win_size_.x, win_pos_.y, win_pos_.x);
        box(win_, 0, 0);
        draw();

        int ch = 0;
        while((ch = getch()) != 'q')
        {   switch(ch)
            {   case KEY_LEFT:
                    snake_->tryMove(py::Snake::Shift::Left);
                    draw();
                    break;
                case KEY_RIGHT:
                    snake_->tryMove(py::Snake::Shift::Right);
                    draw();
                    break;
                case KEY_UP:
                    snake_->tryMove(py::Snake::Shift::Up);
                    draw();
                    break;
                case KEY_DOWN:
                    snake_->tryMove(py::Snake::Shift::Down);
                    draw();
                    break;  
            }
        }

        delwin(win_);
        endwin();
    }

private:
    void draw() override
    {
        sf::Vector2u sz = field_->getSize();

        for (uint32_t j = 0; j < sz.y; j++)
        {
            for (uint32_t i = 0; i < sz.x; i++)
            {
                char ch = 0;
                switch (field_->checkTile({i, j}))
                {
                    case ObjectType::Empty:
                        ch = '-';
                        break;
                    case ObjectType::Apple:
                        ch = '*';
                        break;
                    case ObjectType::Snake:
                        ch = '#';
                        break;
                    case ObjectType::OutOfBorders:
                    default:
                        assert(0 && "out of borders");
                        break;
                }

                mvaddch(win_pos_.y+1 + j, win_pos_.x+1 + i, ch);
            }
        }

        wrefresh(win_);
        // refresh();
    }
};

}

#endif // VIEW_H
