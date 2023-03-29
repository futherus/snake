#include <stdio.h>
#include "view.h"
#include "model.h"
#include "control.h"
#include "view.h"
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

#if 0
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

#endif

$$
    py::HumanControl ctl1{py::Keyboard::Up,
                         py::Keyboard::Left,
                         py::Keyboard::Down,
                         py::Keyboard::Right
    };

    // py::HumanControl ctl2{py::Keyboard::W,
    //                      py::Keyboard::A,
    //                      py::Keyboard::S,
    //                      py::Keyboard::D
    // };

    py::AiControl ctl2{ 0};

    py::Model model;
    py::Snake* snake1 = model.createSnake( {{1, 1},
                                           {2, 1},
                                           {3, 1},
                                           {4, 1}}
    );

    py::Snake* snake2 = model.createSnake( {{1, 3},
                                           {2, 3},
                                           {3, 3},
                                           {4, 3}}
    );

    model.createApple();
    model.createApple();
    model.createApple();

$$
    ctl1.setSnake( snake1);
    ctl2.setSnake( snake2);

$$
    model.init();
$$
    py::ViewManager vm;
    sf::Clock clock;

    bool exit = false;
    bool is_changed = true;
    while (!exit)
    {
        py::Event event;
        while (vm.pollEvent( event))
        {
            ctl1.onEvent( event);
            ctl2.onEvent( event);
            if (vm.onEvent( event) == py::AppState::EXIT)
                exit = true;
        }

        if (clock.getElapsedTime().asMilliseconds() > 250)
        {
            clock.restart();
            is_changed = model.turn();
        }

        vm.draw( model.getField(), is_changed);
        is_changed = false;
    }

    return 0;
}
