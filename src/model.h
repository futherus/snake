#ifndef MODEL_H
#define MODEL_H

#include <vector>
#include <list>
#include <memory>
#include <algorithm>
#include <assert.h>

#include "debug.h"
#include "rand.h"
#include "vector2.h"
#include "view_iface.h"
#include "control_iface.h"

namespace py
{

enum class ObjectType
{
    OutOfBorders,
    Empty,
    Snake,
    Apple
};

class Object
{
private:
    ObjectType type_;

public:
    Object( ObjectType type)
        : type_{ type}
    {}

    ObjectType getType() const { return type_; }
};

class Field final
{
private:
    Vec2i size_;

    std::vector<std::vector<Object*>> tiles_;
    static Object empty_;
    static Object out_of_borders_;

public:
    Field( Vec2i size)
        : size_{ size}
        , tiles_{}
    {
        tiles_.resize( size.y);
        for (auto& row : tiles_)
        {
            row.resize( size.x);
            for (auto& tile : row)
                tile = &empty_;
        }
    }

    Vec2i getSize() const { return size_; }

    Object* checkTile(Vec2i pos) const
    {
        if (!contains(pos))
            return &out_of_borders_;

        return tiles_.at( pos.y).at( pos.x);
    }

    void occupy( Vec2i pos, Object* obj)
    {
        $M("%d, %d\n", pos.x, pos.y);
        assert( tiles_.at( pos.y).at( pos.x) == &empty_ && "Tile is already occupied");
        tiles_.at( pos.y).at( pos.x) = obj;
    }

    void release( Vec2i pos)
    {
        tiles_.at( pos.y).at(pos.x) = &empty_;
    }

    bool contains(Vec2i pos) const { return 0 <= pos.x && pos.x < size_.x &&
                                            0 <= pos.y && pos.y < size_.y; }
};

class Model;

class Apple final : public Object
{
private:
    Model* model_;

    Vec2i pos_;
    bool is_active_;

public:
    Apple( Model* model)
        : Object{ ObjectType::Apple}
        , model_{ model}
        , pos_{}
        , is_active_{ false}
    {}

    void destroy();
    void reset();

    // Vec2i getPos() const { return pos_; }
};

class Snake final : public Object
{
public:
    enum class Direction
    {
        Left,
        Right,
        Up,
        Down
    };

private:
    Model* model_;

    std::vector<Vec2i> pos_;

    Vec2i dir_shift_;

public:
    Snake( Model* model,
           const std::vector<Vec2i>& pos);

    // bool occupies(Vec2i pos)
    // {
    //     return std::find(pos_.begin(), pos_.end(), pos) != pos_.end();
    // }

    void tryMove();
    void setDirection(Direction direction);
};

class Model final
{
private:
    std::vector<std::unique_ptr<Snake>> snakes_;
    std::vector<std::unique_ptr<Apple>> apples_;
    std::unique_ptr<Field> field_;

    std::vector<Apple*> inactive_apples_;

public:
    Model()
        : snakes_{}
        , apples_{}
        , field_{ std::make_unique<Field>( Vec2i{20, 20})}
    {}

    Snake* createSnake( const std::vector<Vec2i> pos)
    {
        snakes_.push_back( std::make_unique<Snake>( this, pos));

        return snakes_.back().get();
    }

    Apple* createApple()
    {
        apples_.push_back( std::make_unique<Apple>( this));
        addInactive( apples_.back().get());

        return apples_.back().get();
    }

    void addInactive( Apple* apple)
    {
        inactive_apples_.push_back( apple);
    }

    void init()
    {
        for (auto* apple : inactive_apples_)
            apple->reset();

        inactive_apples_.clear();
    }

    void turn()
    {
        // FIXME: remove apple reseting from tryMove
        for (auto& snake : snakes_)
            snake->tryMove();

        for (auto* apple : inactive_apples_)
            apple->reset();

        inactive_apples_.clear();
    }

    Field* getField() { return field_.get(); }
};

}

#endif // MODEL_H