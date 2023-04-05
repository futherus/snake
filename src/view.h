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
    Vec2i field_sz_;
    Vec2i win_pos_;
    Vec2i win_size_;
    std::unique_ptr<sf::RenderWindow> win_;

    static constexpr int32_t tile_sz_ = 16;

    sf::Color getColor( Color color)
    {
        switch (color)
        {
            case Color::Black:
                return sf::Color::Black;
            case Color::Green:
                return sf::Color::Green;
            case Color::Blue:
                return sf::Color::Blue;
            case Color::Cyan:
                return sf::Color::Cyan;
            case Color::Magenta:
                return sf::Color::Magenta;
            case Color::Yellow:
                return sf::Color::Yellow;
            case Color::Red:
                return sf::Color::Red;
            case Color::White:
                return sf::Color::White;
            default:
                assert(0 && "Unsupported color");
        }

        return sf::Color::Cyan;
    }

public:
    GuiView( Vec2i win_pos,
             Vec2i field_sz)
        : field_sz_{ field_sz}
        , win_pos_{ win_pos}
        , win_size_{ field_sz.x * tile_sz_, field_sz.y * tile_sz_}
        , win_{
            std::make_unique<sf::RenderWindow>(
                sf::VideoMode( win_size_.x, win_size_.y), "Snake")
          }
    {
        win_->setVisible( false);
        win_->setPosition( win_pos);
    }

    GuiView( const GuiView& that) = delete;
    GuiView( GuiView&& that) = delete;
    GuiView& operator=( const GuiView& that) = delete;
    GuiView& operator=( GuiView&& that) = delete;
    ~GuiView() override = default;

    void draw( const Model* model, bool is_changed) override
    {
        win_->clear(sf::Color::White);

        auto&& snakes = model->getSnakes();
        for (auto&& uptr : snakes)
        {
            const Snake* s = uptr.get();

            for (auto&& pos : s->getPosition())
            {
                sf::RectangleShape tile( {tile_sz_, tile_sz_});
                tile.setFillColor( getColor( s->getColor()));
                tile.setPosition( pos.x * tile_sz_, 
                                  pos.y * tile_sz_);
                win_->draw( tile);

            }
        }

        auto&& apples = model->getApples();
        for (auto&& uptr : apples)
        {
            const Apple* a = uptr.get();
            Vec2i pos = a->getPosition();

            sf::CircleShape tile{ tile_sz_ / 2.f};
            tile.setFillColor( getColor( a->getColor()));
            tile.setPosition( pos.x * tile_sz_, 
                              pos.y * tile_sz_);
            win_->draw( tile);
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

    static constexpr int COLOR_GRAY = 16;

    void initColorPairs()
    {
        init_pair( 1 + static_cast<short>( Color::Black),   COLOR_BLACK,   COLOR_GRAY);
        init_pair( 1 + static_cast<short>( Color::Green),   COLOR_GREEN,   COLOR_GRAY);
        init_pair( 1 + static_cast<short>( Color::Blue),    COLOR_BLUE,    COLOR_GRAY);
        init_pair( 1 + static_cast<short>( Color::Cyan),    COLOR_CYAN,    COLOR_GRAY);
        init_pair( 1 + static_cast<short>( Color::Magenta), COLOR_MAGENTA, COLOR_GRAY);
        init_pair( 1 + static_cast<short>( Color::Yellow),  COLOR_YELLOW,  COLOR_GRAY);
        init_pair( 1 + static_cast<short>( Color::Red),     COLOR_RED,     COLOR_GRAY);
        init_pair( 1 + static_cast<short>( Color::White),   COLOR_WHITE,   COLOR_GRAY);
    }

    short getColorPair( Color color)
    {
        return 1 + static_cast<short>( color);
    }

public:
    TuiView() = default;

    void draw( const Model* model, bool is_changed) override
    {
        Vec2i sz = model->getField()->getSize();
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

            wattron( win_, COLOR_PAIR( getColorPair( Color::Black)));
            for (int32_t j = 0; j < sz.y; j++)
            {
                for (int32_t i = 0; i < sz.x; i++)
                {
                    mvwaddch( win_, 1 + j, 2 + 2 * i, ' ');
                }
            }
            wattroff( win_, COLOR_PAIR( getColorPair( Color::Black)));

            win_size_ = new_size;
            win_pos_ = new_pos;
            field_size_ = sz;

            is_changed = true;
        }

        if (is_changed)
        {
            wattron( win_, COLOR_PAIR( getColorPair( Color::Black)));
            for (int32_t j = 0; j < sz.y; j++)
            {
                for (int32_t i = 0; i < sz.x; i++)
                {
                    mvwaddch( win_, 1 + j, 1 + 2 * i, '-');
                }
            }
            wattroff( win_, COLOR_PAIR( getColorPair( Color::Black)));

            auto&& snakes = model->getSnakes();
            for (auto&& uptr : snakes)
            {
                const Snake* s = uptr.get();

                wattron( win_, COLOR_PAIR( getColorPair( s->getColor())));
                // wattron(  win_, COLOR_PAIR( getColorPair( Color::Magenta)));
                char ch = '@';
                for (auto&& pos : s->getPosition())
                {
                    mvwaddch( win_, 1 + pos.y, 1 + 2 * pos.x, ch);
                    ch = '#';
                }
                wattroff( win_, COLOR_PAIR( getColorPair( s->getColor())));
                // wattroff(  win_, COLOR_PAIR( getColorPair( Color::Magenta)));
            }

            auto&& apples = model->getApples();
            for (auto&& uptr : apples)
            {
                const Apple* a = uptr.get();
                Vec2i pos = a->getPosition();

                wattron(  win_, COLOR_PAIR( getColorPair( a->getColor())));
                mvwaddch( win_, 1 + pos.y, 1 + 2 * pos.x, '*');
                wattroff( win_, COLOR_PAIR( getColorPair( a->getColor())));
            }

            refresh();
            wrefresh( win_);
        }
    }

    void activate() override
    {
        initscr();
        start_color();
        cbreak();
        noecho();
        keypad( stdscr, TRUE);
        curs_set( 0);

        initColorPairs();
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
                case '0':
                    event.type = sf::Event::KeyPressed;
                    event.key.code = sf::Keyboard::Num0;
                    break;
                case '1':
                    event.type = sf::Event::KeyPressed;
                    event.key.code = sf::Keyboard::Num1;
                    break;
                case '2':
                    event.type = sf::Event::KeyPressed;
                    event.key.code = sf::Keyboard::Num2;
                    break;
                case '3':
                    event.type = sf::Event::KeyPressed;
                    event.key.code = sf::Keyboard::Num3;
                    break;
                case '4':
                    event.type = sf::Event::KeyPressed;
                    event.key.code = sf::Keyboard::Num4;
                    break;
                case '5':
                    event.type = sf::Event::KeyPressed;
                    event.key.code = sf::Keyboard::Num5;
                    break;
                case '6':
                    event.type = sf::Event::KeyPressed;
                    event.key.code = sf::Keyboard::Num6;
                    break;
                case '7':
                    event.type = sf::Event::KeyPressed;
                    event.key.code = sf::Keyboard::Num7;
                    break;
                case '8':
                    event.type = sf::Event::KeyPressed;
                    event.key.code = sf::Keyboard::Num8;
                    break;
                case '9':
                    event.type = sf::Event::KeyPressed;
                    event.key.code = sf::Keyboard::Num9;
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

class ViewManager final : public IView
{
private:
    std::unique_ptr<GuiView> gui;
    std::unique_ptr<TuiView> tui;
    SwitchableView* active_;

public:
    ViewManager( Model* model)
        : gui{ std::make_unique<GuiView>( Vec2i{100, 100}, model->getField()->getSize())}
        , tui{ std::make_unique<TuiView>()}
        , active_{ gui.get()}
    {
        active_->activate();
    }

    ~ViewManager()
    {
        active_->deactivate();
    }

    void draw( const Model* model, bool is_changed) override
    {
        active_->draw( model, is_changed);
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
