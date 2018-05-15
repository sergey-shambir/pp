#include "stdafx.h"
#include "winapi_raii.h"
#include <cassert>

void detail::CloseHandle(HANDLE handle)
{
	BOOL succeed = ::CloseHandle(handle);
	(void)succeed;
	assert(succeed);
}

ThreadHandle CreateThreadSafely(ThreadProc proc, void* opaque)
{
	HANDLE thread = ::CreateThread(nullptr, 0, proc, opaque, 0, nullptr);
	if (thread == nullptr)
	{
		throw std::system_error(::GetLastError(), std::system_category());
	}
	return ThreadHandle(thread);
}

SemaphoreHandle CreateAnonymousSemaphoreSafely(unsigned maxCount)
{
	HANDLE semaphore = ::CreateSemaphoreW(
		nullptr, // default security attributes
		LONG(maxCount),
		LONG(maxCount),
		nullptr // anonymous
	);
	if (semaphore == nullptr)
	{
		throw std::system_error(::GetLastError(), std::system_category());
	}
	return SemaphoreHandle(semaphore);
}
