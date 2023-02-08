#include <stdio.h>
// #include "view.h"
#include "model.h"
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
        {40, 40},
        apple.get(),
        snake.get()
    ));

    snake->setField(field.get());

    field->resetApple();
    field->draw();
    std::printf("\n\n");

    snake->tryMove(py::Snake::Shift::Down);
    field->draw();
    std::printf("\n\n");

    return 0;
}
