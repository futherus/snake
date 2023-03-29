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

    void dbg_dump()
    {
        for (auto& row : tiles_)
        {
            for (auto& tile : row)
            {
                char ch = '?';
                if (tile == nullptr)
                {
                    ch = 'n';
                }
                else
                {
                    switch (tile->getType())
                    {
                        case ObjectType::Apple:
                            ch = 'a';
                            break;
                        case ObjectType::Empty:
                            ch = '-';
                            break;
                        case ObjectType::Snake:
                            ch = 's';
                            break;
                        case ObjectType::OutOfBorders:
                            ch = 'O';
                            break;
                        default:
                            assert(0 && "fallthrough");
                            break;
                    }
                }

                $M("%c ", ch);
            }

            $M("\n");
        }
    }
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
    Snake( Model* model);

    void clear();
    void addPosition( Vec2i pos);
    bool tryMove();
    void setDirection(Direction direction);
};

class Model final
{
private:
    std::vector<std::unique_ptr<Snake>> snakes_;
    std::vector<std::unique_ptr<Apple>> apples_;
    std::unique_ptr<Field> field_;

    std::vector<Apple*> inactive_apples_;
    std::vector<Snake*> inactive_snakes_;

public:
    Model()
        : snakes_{}
        , apples_{}
        , field_{ std::make_unique<Field>( Vec2i{20, 20})}
    {}

    Snake* createSnake()
    {
        snakes_.push_back( std::make_unique<Snake>( this));
        inactive_snakes_.push_back( snakes_.back().get());

        return snakes_.back().get();
    }

    void resetSnake( Snake* snake)
    {
        Vec2i sz = field_->getSize();
        snake->clear();

        Vec2i pos;
        do
        {
            pos = { uniform_distr( 0, sz.x), uniform_distr( 0, sz.y)};
        }
        while (field_->checkTile( pos)->getType() != ObjectType::Empty);

        snake->addPosition( pos);

        for (size_t i = 0; i < 5; i++)
        {
            Vec2i shift;
            Vec2i shifts[] = {{0, 1}, {0, -1}, {1, 0}, {-1, 0}};

            do
            {
                shift = shifts[uniform_distr( 0, 3)];
            }
            while (field_->checkTile( pos + shift)->getType() != ObjectType::Empty);

            pos += shift;
            snake->addPosition( pos);
        }
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

    void addInactive( Snake* snake)
    {
        inactive_snakes_.push_back( snake);
    }

    void init()
    {
        for (auto* apple : inactive_apples_)
            apple->reset();

        for (auto* snake : inactive_snakes_)
            resetSnake( snake);

        inactive_apples_.clear();
        inactive_snakes_.clear();
    }

    bool turn()
    {
        bool is_changed = false;
        for (auto& snake : snakes_)
        {
            snake->tryMove();
            is_changed = true;
        }

        for (auto* apple : inactive_apples_)
        {
            apple->reset();
            is_changed = true;
        }

        for (auto* snake : inactive_snakes_)
        {
            resetSnake( snake);
        }

        inactive_snakes_.clear();
        inactive_apples_.clear();

        return is_changed;
    }

    Field* getField() { return field_.get(); }
};

}

#endif // MODEL_H
