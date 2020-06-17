#ifndef HARA_MACROS_H
#define HARA_MACROS_H

#include <stdexcept>

#define ASSERT(x, msg) \
    if (!(x)) throw std::runtime_error(msg)

#endif //HARA_MACROS_H
