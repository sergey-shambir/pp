#include "stdafx.h"
#include "Handle.h"
#include <cassert>

void detail::CloseHandle(HANDLE handle)
{
	BOOL succeed = ::CloseHandle(handle);
	(void)succeed;
	assert(succeed);
}
