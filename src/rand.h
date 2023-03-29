#ifndef RAND_H
#define RAND_H

#include <cctype>
#include <random>
#include <assert.h>

template <typename T>
int signum(T val) {
    int retval = (T(0) < val) - (val < T(0));
    assert (retval == 0 || retval == 1 || retval == -1);

    return retval;
}

int32_t uniform_distr(int32_t from, int32_t to);

#endif // RAND_H
