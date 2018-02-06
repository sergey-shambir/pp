#pragma once
#include "Handle.h"

using ThreadHandle = detail::Handle<detail::CloseHandle, nullptr>;
using ThreadProc = DWORD (WINAPI *)(void* opaque);

// Creates thread with given thread proc and opaque.
// Thread proc must not throw.
ThreadHandle CreateThread(ThreadProc proc, void* opaque);
