#include <stdio.h>
#include "view.h"
#include "model.h"
#include "control.h"
#include "view.h"
#include "ncurses.h"
#include <memory>
#include <string>

int DELAY = 300;

void checkDelay( py::Event& event)
{
    if (event.type == py::Event::KeyPressed)
    {
        switch (event.key.code)
        {
            case py::Keyboard::Num1:
                DELAY = 100;
                break;
            case py::Keyboard::Num2:
                DELAY = 200;
                break;
            case py::Keyboard::Num3:
                DELAY = 300;
                break;
            case py::Keyboard::Num4:
                DELAY = 400;
                break;
            case py::Keyboard::Num5:
                DELAY = 500;
                break;
            case py::Keyboard::Num6:
                DELAY = 600;
                break;
            case py::Keyboard::Num7:
                DELAY = 700;
                break;
            case py::Keyboard::Num8:
                DELAY = 800;
                break;
            default:
                break;
        }
    }
}

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
    py::Model model( {40, 40});

    // py::HumanControl ctl1{ &model,
    //                        py::Keyboard::Up,
    //                        py::Keyboard::Left,
    //                        py::Keyboard::Down,
    //                        py::Keyboard::Right
    // };

    // py::HumanControl ctl2{ &model,
    //                        py::Keyboard::W,
    //                        py::Keyboard::A,
    //                        py::Keyboard::S,
    //                        py::Keyboard::D
    // };

    py::AiControl ctl1{ &model};
    py::AiControl ctl2{ &model};
    py::AiControl ctl3{ &model};
    py::AiControl ctl4{ &model};
    py::AiControl ctl5{ &model};
    py::AiControl ctl6{ &model};


    py::Snake* snake1 = model.createSnake(py::Color::Green);
    py::Snake* snake2 = model.createSnake(py::Color::White);
    py::Snake* snake3 = model.createSnake(py::Color::Blue);
    py::Snake* snake4 = model.createSnake(py::Color::Yellow);
    py::Snake* snake5 = model.createSnake(py::Color::Cyan);
    py::Snake* snake6 = model.createSnake(py::Color::Magenta);

    model.createApple();
    model.createApple();
    model.createApple();
    model.createApple();
    model.createApple();
    model.createApple();
    model.createApple();

$$
    ctl1.setSnake( snake1);
    ctl2.setSnake( snake2);
    ctl3.setSnake( snake3);
    ctl4.setSnake( snake4);
    ctl5.setSnake( snake5);
    ctl6.setSnake( snake6);

$$
    model.init();
$$
    py::ViewManager vm( &model);
    sf::Clock clock;

    bool exit = false;
    bool is_changed = true;
    while (!exit)
    {
        py::Event event;
        while (vm.pollEvent( event))
        {
            // ctl1.onEvent( event);
            // ctl2.onEvent( event);
            checkDelay( event);
            if (vm.onEvent( event) == py::AppState::EXIT)
                exit = true;
        }

        if (clock.getElapsedTime().asMilliseconds() > DELAY)
        {
            clock.restart();
            ctl1.onTurn();
            ctl2.onTurn();
            ctl3.onTurn();
            ctl4.onTurn();
            ctl5.onTurn();
            ctl6.onTurn();

            is_changed = model.turn();
        }

        vm.draw( &model, is_changed);
        is_changed = false;
    }

    return 0;
}
