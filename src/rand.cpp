#include "rand.h"

uint32_t uniform_distr(uint32_t from, uint32_t to)
{
    static std::mt19937 generator(std::random_device{}());

    std::uniform_int_distribution<uint32_t> distr(from, to);

    return distr(generator);
}
