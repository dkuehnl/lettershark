#ifndef RAMWATCHER_H
#define RAMWATCHER_H

#include <cstddef>

namespace RamWatcher {
    bool enoug_memory_available(size_t ram_limit);
}

#endif // RAMWATCHER_H
