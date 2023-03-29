#ifndef CONTROL_IFACE_H
#define CONTROL_IFACE_H

namespace py
{

class Snake;
class Apple;
class Model;

class IControl
{
protected:
    Snake* snake_;
    const Model* model_;

public:
    IControl( const Model* model)
        : snake_{}
        , model_{ model}
    {}

    void setSnake( Snake* snake) { snake_ = snake; }
};

} // namespace py

#endif // CONTROL_IFACE_H