#include "stdafx.h"
#include "ThreadHandle.h"
#include <system_error>

ThreadHandle CreateThread(ThreadProc proc, void* opaque)
{
	HANDLE thread = ::CreateThread(nullptr, 0, proc, opaque, 0, nullptr);
	if (thread == nullptr)
	{
		throw std::system_error(::GetLastError(), std::system_category());
	}
	return ThreadHandle(thread);
}
