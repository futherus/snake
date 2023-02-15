#ifndef VIEW_H
#define VIEW_H

#include <iostream>
#include <ncurses.h>
#include <assert.h>
#include <SFML/Graphics.hpp>

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

class GuiView final : public IView
{
private:
    std::unique_ptr<sf::RenderWindow> win_;

    uint32_t tile_sz_;
    Vector2u win_pos_;
    Vector2u win_size_;

public:
    GuiView(Field* field,
            Snake* snake,
            Apple* apple)
        : IView(field, snake, apple)
        , win_()
        , tile_sz_(16)
        , win_pos_()
        , win_size_()
    {}

    void run()
    {
        Vector2u size{800, 600};
        win_.reset(new sf::RenderWindow(sf::VideoMode(size.x, size.y), "My window"));

        win_size_ = field_->getSize() * tile_sz_;
        win_pos_ = {(size.x - win_size_.x) / 2, (size.y - win_size_.y) / 2};

        // run the program as long as the window is open
        while (win_->isOpen())
        {
            // check all the window's events that were triggered since the last iteration of the loop
            sf::Event event;
            while (win_->pollEvent(event))
            {
                switch (event.type)
                {
                    // "close requested" event: we close the window
                    case  sf::Event::Closed:
                    {
                        win_->close();
                        break;
                    }
                    case sf::Event::KeyPressed:
                    {
                        switch (event.key.code)
                        {
                            case sf::Keyboard::Left:
                                snake_->tryMove(py::Snake::Shift::Left);
                                break;
                            case sf::Keyboard::Right:
                                snake_->tryMove(py::Snake::Shift::Right);
                                break;
                            case sf::Keyboard::Up:
                                snake_->tryMove(py::Snake::Shift::Up);
                                break;
                            case sf::Keyboard::Down:
                                snake_->tryMove(py::Snake::Shift::Down);
                                break;
                            default:
                                break;
                        }

                        break;
                    }
                }
 
            }

            win_->clear();

            draw();

            win_->display();
        }

        win_.reset(nullptr);
    }

    void draw() override
    {
        Vector2u sz = field_->getSize();

        for (uint32_t j = 0; j < sz.y; j++)
        {
            for (uint32_t i = 0; i < sz.x; i++)
            {
                sf::Color col;
                switch (field_->checkTile({i, j}))
                {
                    case ObjectType::Empty:
                        col = sf::Color::White;
                        break;
                    case ObjectType::Apple:
                        col = sf::Color::Red;
                        break;
                    case ObjectType::Snake:
                        col = sf::Color::Green;
                        break;
                    case ObjectType::OutOfBorders:
                    default:
                        assert(0 && "out of borders");
                        break;
                }

                sf::RectangleShape tile({tile_sz_, tile_sz_});
                tile.setFillColor(col);
                tile.setPosition(win_pos_.x + i * tile_sz_, 
                                 win_pos_.y + j * tile_sz_);
                win_->draw(tile);
            }
        }
    }
};

class TuiView final : public IView
{
private:
    WINDOW* win_;
    Vector2u win_pos_;
    Vector2u win_size_;

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

        Vector2u sz = field_->getSize();
        win_size_ = sz + Vector2u{2, 2};

        win_pos_.x = (COLS  - win_size_.x) / 2;
        win_pos_.y = (LINES - win_size_.y) / 2;

        win_ = newwin(win_size_.y, win_size_.x, win_pos_.y, win_pos_.x);
        box(win_, 0, 0);
        draw();

        int ch = 0;
        while((ch = getch()) != 'q')
        {   switch(ch)
            {
                case KEY_LEFT:
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
                default:
                    break;
            }
        }

        delwin(win_);
        endwin();
    }

private:
    void draw() override
    {
        Vector2u sz = field_->getSize();

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
