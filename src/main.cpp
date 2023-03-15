#include <stdio.h>
#include "view.h"
#include "model.h"
#include "ncurses.h"
#include <memory>
#include <string>

int main(int argc, char *argv[])
{
    // std::unique_ptr<py::IView> v;
    // if (argc > 1 && argv[1] == std::string("--gui"))
    //     v.reset(new py::GuiView);
    // else
    //     v.reset(new py::AsciiView);

    // v->draw();

    std::unique_ptr<py::Apple> apple(new py::Apple);
    std::unique_ptr<py::Snake> snake(new py::Snake({{1, 1},
                                                    {2, 1},
                                                    {3, 1},
                                                    {4, 1}}));

    std::unique_ptr<py::Field> field(new py::Field(
        {20, 20},
        apple.get(),
        snake.get()));

    snake->setField(field.get());
    field->resetApple();

    py::TuiView tui_view(field.get(), snake.get(), apple.get());
    py::GuiView gui_view(field.get(), snake.get(), apple.get());

    py::IView *view = &tui_view;

    while (true)
    {
        switch (view->run())
        {
        case py::AppState::EXIT:
            return 0;
        case py::AppState::GUI:
            view = &gui_view;
            break;
        case py::AppState::TUI:
            view = &tui_view;
            break;
        default:
            assert(0 && "Invalid run option");
            break;
        }
    }

    // sf::Clock clock;
    // while (!sf::Keyboard::isKeyPressed(sf::Keyboard::Q))
    // {
    //     if (clock.getElapsedTime().asMilliseconds() > 100)
    //     {
    //         if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left))
    //         {
    //             snake->tryMove(py::Snake::Shift::Left);
    //             clock.restart();
    //         }
    //         else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right))
    //         {
    //             snake->tryMove(py::Snake::Shift::Right);
    //             clock.restart();
    //         }
    //         else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up))
    //         {
    //             snake->tryMove(py::Snake::Shift::Up);
    //             clock.restart();
    //         }
    //         else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down))
    //         {
    //             snake->tryMove(py::Snake::Shift::Down);
    //             clock.restart();
    //         }
    //     }
    //     refresh();
    //     field->draw();
    // }

    return 0;
}
