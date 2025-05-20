#include "ramwatcher.h"

#include <windows.h>
#include <psapi.h>

bool RamWatcher::enoug_memory_available(size_t ram_limit) {
    MEMORYSTATUSEX statex;
    statex.dwLength = sizeof(statex);
    if (GlobalMemoryStatusEx(&statex)) {
        return statex.ullAvailPhys >= ram_limit;
    }
    return false;
}
