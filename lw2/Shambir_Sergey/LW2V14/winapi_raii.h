#pragma once

#include <sdkddkver.h>
#include <windows.h>

#include "Handle.h"

namespace detail
{
void CloseHandle(HANDLE handle);
}

using SemaphoreHandle = detail::Handle<detail::CloseHandle, nullptr>;
using ThreadHandle = detail::Handle<detail::CloseHandle, nullptr>;
using ThreadProc = DWORD(WINAPI*)(void* opaque);

// Creates thread with given thread proc and opaque.
// Thread proc must not throw.
ThreadHandle CreateThreadSafely(ThreadProc proc, void* opaque);

// Creates anonymous semaphore with given slots count.
// Postcondition: (free slots count == slots count).
SemaphoreHandle CreateAnonymousSemaphoreSafely(unsigned maxCount);
