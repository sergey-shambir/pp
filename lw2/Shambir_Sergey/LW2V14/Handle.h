#pragma once

#include <algorithm>
#include <sdkddkver.h>
#include <windows.h>

namespace detail
{
using HandleCloseFn = void (*)(HANDLE);

#pragma pack(push, 1)
template <HandleCloseFn CloseFn, HANDLE NullValue>
class Handle final
{
public:
	Handle()
		: Handle(NullValue)
	{
	}

	explicit Handle(HANDLE handle)
		: m_handle(handle)
	{
	}

	Handle(const Handle&) = delete;
	Handle& operator=(const Handle&) = delete;

	Handle(Handle&& other)
		: m_handle(other.m_handle)
	{
		other.m_handle = NullValue;
	}

	Handle& operator=(Handle&& other)
	{
		Reset();
		std::swap(m_handle, other.m_handle);
		return *this;
	}

	~Handle()
	{
		Reset();
	}

	explicit operator bool() const
	{
		return (m_handle != NullValue);
	}

	operator HANDLE&()
	{
		return m_handle;
	}

	operator const HANDLE&() const
	{
		return m_handle;
	}

	HANDLE get() const
	{
		return m_handle;
	}

	void Reset()
	{
		if (m_handle != NullValue)
		{
			CloseFn(m_handle);
			m_handle = NullValue;
		}
	}

private:
	HANDLE m_handle{ NullValue };
};
#pragma pack(pop)

} // namespace detail
