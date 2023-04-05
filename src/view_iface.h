#ifndef VIEW_IFACE_H
#define VIEW_IFACE_H

#include "SFML/Window/Event.hpp"

namespace py
{

class Model;

using Event = sf::Event;
using Keyboard = sf::Keyboard;

class IView
{
public:
    IView() = default;
    IView(const IView& that) = delete;
    IView(IView&& that) = delete;
    IView& operator=(const IView& that) = delete;
    IView& operator=(IView&& that) = delete;
    virtual ~IView() = default;

    virtual void draw( const Model* model, bool is_changed) = 0;
};

enum class Color
{
    Black,
    Green,
    Blue,
    Cyan,
    Magenta,
    Yellow,
    Red,
    White,
};

}

#endif // VIEW_IFACE_H