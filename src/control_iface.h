#ifndef CONTROL_IFACE_H
#define CONTROL_IFACE_H

namespace py
{

class Snake;

class IControl
{
protected:
    Snake* snake_;

public:
    IControl() = default;

    void setSnake( Snake* snake) { snake_ = snake; }
};

} // namespace py

#endif // CONTROL_IFACE_H