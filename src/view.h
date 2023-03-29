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

#if 0
    AppState run() override
    {
        Vec2i size{800, 600};
        std::unique_ptr<sf::RenderWindow> win =
            std::make_unique<sf::RenderWindow>(sf::VideoMode(size.x, size.y), "My window");

        Vec2i win_size = field_->getSize() * tile_sz_;
        Vec2i win_pos = {(size.x - win_size.x) / 2, (size.y - win_size.y) / 2};

        // FIXME
        sf::Clock clock;

        // run the program as long as the window is open
        while (win->isOpen())
        {
            // check all the window's events that were triggered since the last iteration of the loop
            sf::Event event;
            while (win->pollEvent(event))
            {
                switch (event.type)
                {
                    // "close requested" event: we close the window
                    case sf::Event::Closed:
                    {
                        return AppState::EXIT;
                    }
                    case sf::Event::KeyPressed:
                    {
                        switch (event.key.code)
                        {
                            case sf::Keyboard::Left:
                                snake_->setDirection(py::Snake::Direction::Left);
                                break;
                            case sf::Keyboard::Right:
                                snake_->setDirection(py::Snake::Direction::Right);
                                break;
                            case sf::Keyboard::Up:
                                snake_->setDirection(py::Snake::Direction::Up);
                                break;
                            case sf::Keyboard::Down:
                                snake_->setDirection(py::Snake::Direction::Down);
                                break;
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
            }

            if (clock.getElapsedTime().asMilliseconds() > 500)
            {
                clock.restart();
                snake_->tryMove();
            }

            win->clear();

            draw(win.get(), win_pos, win_size);

            win->display();
        }

        assert(0 && "fallthrough");
        return AppState::EXIT;
    }
#endif

    void draw( const Field* field) override
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
    }

    void display() override
    {
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
public:
    TuiView() = default;

    void draw(const Field* field) override
    {
        std::cerr << "Unimplemented draw\n";
    }

    void display() override
    {
        std::cerr << "Unimplemented display\n";
    }

    void activate() override
    {
        std::cerr << "Unimplemented activate\n";
    }

    void deactivate() override
    {
        std::cerr << "Unimplemented deactivate\n";
    }

    bool pollEvent(sf::Event&)
    {
        std::cerr << "Unimplemented pollEvent\n";
        return false;
    }

    AppState onEvent(sf::Event&)
    {
        std::cerr << "Unimplemented onEvent\n";
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
        , tui{}
        , active_{ gui.get()}
    {
        active_->activate();
    }

    void draw( const Field* field) override
    {
        active_->draw( field);
    }

    void display() override
    {
        active_->display();
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
