#include <stdio.h>
#include "view.h"
#include "model.h"
#include "ncurses.h"
#include <memory>

int main(int argc, char* argv[])
{
    // std::unique_ptr<py::IView> v;
    // if (argc > 1 && argv[1] == std::string("--gui"))
    //     v.reset(new py::GuiView);
    // else
    //     v.reset(new py::AsciiView);

    // v->draw();

    std::unique_ptr<py::Apple> apple(new py::Apple);
    std::unique_ptr<py::Snake> snake(new py::Snake({
        {10, 10},
        {11, 10},
        {12, 10},
        {13, 10}
    }));

    std::unique_ptr<py::Field> field(new py::Field(
        {20, 20},
        apple.get(),
        snake.get()
    ));

    snake->setField(field.get());
    field->resetApple();

    py::TuiView view(field.get(), snake.get(), apple.get());
    view.run();

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
