#ifndef VIEW_H
#define VIEW_H

#include <iostream>

namespace py
{

class IView
{
private:

public:
    IView() = default;
    virtual ~IView() = default;

    virtual void draw() = 0;
};

class GuiView : public IView
{
private:

public:
    GuiView() = default;

    void draw() override { std::printf("Not implemented\n"); }

};

class AsciiView : public IView
{
private:

public:
    AsciiView() = default;

    void draw() override { std::printf("Hello world!\n"); }
};

}

#endif // VIEW_H
