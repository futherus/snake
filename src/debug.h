#ifndef DEBUG_H
#define DEBUG_H

#include <iostream>

#ifndef NDEBUG
    #define $$ std::fprintf(stderr, "%s:%d %s\n", __FILE__, __LINE__, __PRETTY_FUNCTION__);
    #define $M(...) std::fprintf(stderr, __VA_ARGS__)
#else
    #define $$
    #define $M(...)
#endif

#endif // DEBUG_H
