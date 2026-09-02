#ifndef RAMWATCHER_H
#define RAMWATCHER_H

#include <cstddef>

/**
 * @brief Windows-only helper for checking available physical RAM before
 *        starting memory-heavy operations (parsing large files/folders,
 *        global search across many tabs).
 */
namespace RamWatcher {
    /**
     * @brief Checks whether at least ram_limit bytes of physical RAM are currently available.
     *
     * Wraps the Win32 GlobalMemoryStatusEx() API. Intended as a soft guard rail
     * before an operation known to be memory-intensive; callers typically warn
     * the user and let them opt out rather than block the operation outright.
     *
     * @param ram_limit Minimum required available physical memory, in bytes.
     * @return true if at least ram_limit bytes are available, false if less is
     *         available or the underlying system call fails.
     */
    bool enoug_memory_available(size_t ram_limit);
}

#endif // RAMWATCHER_H
