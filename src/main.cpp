#include <stdio.h>
#include "view.h"
#include <memory>

int main(int argc, char* argv[])
{
    std::unique_ptr<py::IView> v;
    if (argc > 1 && argv[1] == std::string("--gui"))
        v.reset(new py::GuiView);
    else
        v.reset(new py::AsciiView);

    v->draw();

    return 0;
}
