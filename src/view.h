#ifndef VIEW_H
#define VIEW_H

#include <iostream>
#include <map>
#include <ncurses.h>
#include <poll.h>
#include <unistd.h>
#include <assert.h>
#include <SFML/Graphics.hpp>

#include "model.h"
#include "view_iface.h"

namespace py
{

enum class AppState
{
    RUNNING,
    TUI,
    GUI,
    EXIT,
};

class SwitchableView : public IView
{
public:
    virtual bool pollEvent( Event& event) = 0;
    virtual AppState onEvent( Event& event) = 0;

    virtual void activate() = 0;
    virtual void deactivate() = 0;
};

class GuiView final : public SwitchableView
{
private:
    std::unique_ptr<sf::RenderWindow> win_;
    Vec2i win_pos_;
    Vec2i win_size_;

    int32_t tile_sz_;

public:
    GuiView(Vec2i win_pos,
            Vec2i win_size)
        : win_{
            std::make_unique<sf::RenderWindow>( sf::VideoMode(win_size.x, win_size.y),
                                                "My window")
          }
        , win_pos_{ win_pos}
        , win_size_{ win_size}
        , tile_sz_{ 16}
    {
        win_->setVisible( false);
        win_->setPosition( win_pos);
    }

    GuiView( const GuiView& that) = delete;
    GuiView( GuiView&& that) = delete;
    GuiView& operator=( const GuiView& that) = delete;
    GuiView& operator=( GuiView&& that) = delete;
    ~GuiView() override = default;

    void draw( const Field* field, bool is_changed) override
    {
        Vec2i field_sz = field->getSize();
        Vec2i pixels_sz = field_sz * tile_sz_;
        Vec2i pixels_pos = {(win_size_.x - pixels_sz.x) / 2, (win_size_.y - pixels_sz.y) / 2};

        for (uint32_t j = 0; j < field_sz.y; j++)
        {
            for (uint32_t i = 0; i < field_sz.x; i++)
            {
                sf::Color col;
                switch (field->checkTile( {i, j})->getType())
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
                        assert( 0 && "out of borders");
                        break;
                }

                sf::RectangleShape tile({tile_sz_, tile_sz_});
                tile.setFillColor( col);
                tile.setPosition( pixels_pos.x + i * tile_sz_, 
                                  pixels_pos.y + j * tile_sz_);
                win_->draw( tile);
            }
        }

        win_->display();
    }

    void activate() override
    {
        win_->setVisible( true);
    }

    void deactivate() override
    {
        win_->setVisible( false);
    }

    bool pollEvent( Event& event)
    {
        return win_->pollEvent( event);
    }

    AppState onEvent( Event& event) override
    {
        switch ( event.type)
        {
            case sf::Event::Closed:
            {
                return AppState::EXIT;
            }
            case sf::Event::KeyPressed:
            {
                switch ( event.key.code)
                {
                    case sf::Keyboard::C:
                        return AppState::TUI;
                    default:
                        break;
                }

                break;
            }
            default:
            {
                break;
            }
        }

        return AppState::RUNNING;
    }
};

class TuiView final : public SwitchableView
{
private:
    WINDOW* win_;
    Vec2i win_pos_;
    Vec2i win_size_;

    Vec2i field_size_;

public:
    TuiView() = default;

    void draw( const Field* field, bool is_changed) override
    {
        Vec2i sz = field->getSize();
        if (!win_ || field_size_ != sz)
        {
            if (win_)
                delwin( win_);

            Vec2i new_size = sz + Vec2i{1, 2};
            new_size.x *= 2;

            Vec2i new_pos{
                (COLS  - new_size.x) / 2,
                (LINES - new_size.y) / 2
            };

            win_ = newwin( new_size.y, new_size.x, new_pos.y, new_pos.x);
            box( win_, 0, 0);

            win_size_ = new_size;
            win_pos_ = new_pos;
            field_size_ = sz;

            is_changed = true;
        }

        if (is_changed)
        {
            for (int32_t j = 0; j < sz.y; j++)
            {
                for (int32_t i = 0; i < sz.x; i++)
                {
                    char ch = 0;
                    switch (field->checkTile( {i, j})->getType())
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

                    mvwaddch( win_, 1 + j, 1 + 2 * i, ch);
                    mvwaddch( win_, 1 + j, 2 + 2 * i, ' ');
                }
            }

            refresh();
            wrefresh( win_);
        }
    }

    void activate() override
    {
        initscr();
        cbreak();
        noecho();
        keypad( stdscr, TRUE);
    }

    void deactivate() override
    {
        delwin( win_);
        win_ = nullptr;
        endwin();
    }

    bool pollEvent( sf::Event& event)
    {
        struct pollfd fds[1];
        fds[0].fd = STDIN_FILENO;
        fds[0].events = POLLIN;

        if (poll( fds, 1, 0) > 0)
        {
            int ch = getch();

            switch (ch)
            {
                case KEY_LEFT:
                    event.type = sf::Event::KeyPressed;
                    event.key.code = sf::Keyboard::Left;
                    break;
                case KEY_RIGHT:
                    event.type = sf::Event::KeyPressed;
                    event.key.code = sf::Keyboard::Right;
                    break;
                case KEY_UP:
                    event.type = sf::Event::KeyPressed;
                    event.key.code = sf::Keyboard::Up;
                    break;
                case KEY_DOWN:
                    event.type = sf::Event::KeyPressed;
                    event.key.code = sf::Keyboard::Down;
                    break;
                case 'w':
                    event.type = sf::Event::KeyPressed;
                    event.key.code = sf::Keyboard::W;
                    break;
                case 'a':
                    event.type = sf::Event::KeyPressed;
                    event.key.code = sf::Keyboard::A;
                    break;
                case 's':
                    event.type = sf::Event::KeyPressed;
                    event.key.code = sf::Keyboard::S;
                    break;
                case 'd':
                    event.type = sf::Event::KeyPressed;
                    event.key.code = sf::Keyboard::D;
                    break;
                case 'c':
                    event.type = sf::Event::KeyPressed;
                    event.key.code = sf::Keyboard::C;
                    break;
                case 'q':
                    event.type = sf::Event::KeyPressed;
                    event.key.code = sf::Keyboard::Q;
                    break;
                default:
                    assert( 0 && "Unimplemented key");
                    break;
            }

            return true;
        }

        return false;
    }

    AppState onEvent( sf::Event& event) override
    {
        switch ( event.type)
        {
            case sf::Event::KeyPressed:
            {
                switch ( event.key.code)
                {
                    case sf::Keyboard::C:
                        return AppState::GUI;
                    case sf::Keyboard::Q:
                        return AppState::EXIT;
                    default:
                        break;
                }

                break;
            }
            default:
            {
                break;
            }
        }

        return AppState::RUNNING;
    }
};

#if 0
class TuiView final : public IView
{
private:
    WINDOW* win_;
    Vec2i win_pos_;
    Vec2i win_size_;

public:
    TuiView(Field* field,
            Snake* snake,
            Apple* apple)
        : IView(field, snake, apple)
        , win_()
        , win_pos_()
        , win_size_()
    {}

    TuiView(const TuiView& that) = delete;
    TuiView(TuiView&& that) = delete;
    TuiView& operator=(const TuiView& that) = delete;
    TuiView& operator=(TuiView&& that) = delete;
    ~TuiView() override = default;

    AppState run() override
    {
        initscr();
        cbreak();
        noecho();
        keypad(stdscr, TRUE);
        refresh();

        Vec2i sz = field_->getSize();
        win_size_ = sz + Vec2i{2, 2};

        win_pos_.x = (COLS  - win_size_.x) / 2;
        win_pos_.y = (LINES - win_size_.y) / 2;

        win_ = newwin(win_size_.y, win_size_.x, win_pos_.y, win_pos_.x);
        box(win_, 0, 0);
        draw();

        struct pollfd fds[1];
        fds[0].fd = STDIN_FILENO;
        fds[0].events = POLLIN;

        // FIXME:
        sf::Clock clock;

        while(true)
        {
            int timeout = 500 - clock.getElapsedTime().asMilliseconds();
            if (timeout < 0)
                timeout = 0;

            int ret = poll(fds, 1, timeout);
            if (ret > 0)
            {
                int ch = getch();
                switch(ch)
                {
                    case KEY_LEFT:
                        snake_->setDirection(py::Snake::Direction::Left);
                        break;
                    case KEY_RIGHT:
                        snake_->setDirection(py::Snake::Direction::Right);
                        break;
                    case KEY_UP:
                        snake_->setDirection(py::Snake::Direction::Up);
                        break;
                    case KEY_DOWN:
                        snake_->setDirection(py::Snake::Direction::Down);
                        break;
                    case 'c':
                        delwin(win_);
                        endwin();
                        return AppState::GUI;
                    case 'q':
                        delwin(win_);
                        endwin();
                        return AppState::EXIT;
                    default:
                        break;
                }
            }

            if (clock.getElapsedTime().asMilliseconds() > 500)
            {
                clock.restart();
                snake_->tryMove();
                draw();
            }
        }

        delwin(win_);
        endwin();
        return AppState::EXIT;
    }

private:
    void draw()
    {
        Vec2i sz = field_->getSize();

        for (int32_t j = 0; j < sz.y; j++)
        {
            for (int32_t i = 0; i < sz.x; i++)
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
        refresh();
    }
};

#endif

class ViewManager final : public IView
{
private:
    std::unique_ptr<GuiView> gui;
    std::unique_ptr<TuiView> tui;
    SwitchableView* active_;

public:
    ViewManager()
        : gui{ std::make_unique<GuiView>( Vec2i{100, 100}, Vec2i{800, 600})}
        , tui{ std::make_unique<TuiView>()}
        , active_{ tui.get()}
    {
        active_->activate();
    }

    ~ViewManager()
    {
        active_->deactivate();
    }

    void draw( const Field* field, bool is_changed) override
    {
        active_->draw( field, is_changed);
    }

    bool pollEvent( Event& event)
    {
        return active_->pollEvent( event);
    }

    AppState onEvent( Event& event)
    {
        AppState state = active_->onEvent( event);

        switch (state)
        {
            case AppState::GUI:
                active_->deactivate();
                active_ = gui.get();
                active_->activate();
                state = AppState::RUNNING;
                break;
            case AppState::TUI:
                active_->deactivate();
                active_ = tui.get();
                active_->activate();
                state = AppState::RUNNING;
                break;
            default:
                break;
        }

        return state;
    }
};

}

#endif // VIEW_H
