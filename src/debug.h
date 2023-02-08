#ifndef DEBUG_H
#define DEBUG_H

#include <iostream>

#define $$ std::printf("%s:%d %s\n", __FILE__, __LINE__, __PRETTY_FUNCTION__);

#define $M(...) std::printf(__VA_ARGS__)
#endif // DEBUG_H
