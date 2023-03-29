#include "rand.h"

int32_t uniform_distr(int32_t from, int32_t to)
{
    static std::mt19937 generator(std::random_device{}());

    std::uniform_int_distribution<int32_t> distr(from, to);

    return distr(generator);
}
