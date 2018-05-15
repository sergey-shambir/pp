#include "stdafx.h"
#include "SemaphoreLock.h"
#include <algorithm>

SemaphoreLock::SemaphoreLock(const SemaphoreHandle& handle)
	: m_handle(handle)
{
	if (!m_handle)
	{
		throw std::logic_error("cannot lock null semaphore");
	}
	::WaitForSingleObject(m_handle, INFINITE);
}

SemaphoreLock::SemaphoreLock(const SemaphoreHandle& handle, AlreadyLocked) noexcept
	: m_handle(handle)
{
}

SemaphoreLock::~SemaphoreLock()
{
	if (m_handle)
	{
		::ReleaseSemaphore(m_handle, 1, nullptr);
	}
}

SemaphoreLock::operator bool() const noexcept
{
	return (m_handle != nullptr);
}

SemaphoreLock::SemaphoreLock(SemaphoreLock&& other) noexcept
	: m_handle(other.m_handle)
{
	other.m_handle = nullptr;
}

SemaphoreLock& SemaphoreLock::operator=(SemaphoreLock&& other) noexcept
{
	if (m_handle)
	{
		::ReleaseSemaphore(m_handle, 1, nullptr);
		m_handle = nullptr;
	}
	std::swap(m_handle, other.m_handle);
	return *this;
}

SemaphoreLock LockSemaphore(const SemaphoreHandle& handle)
{
	return SemaphoreLock(handle);
}

SemaphoreLock TryLockSemaphore(const SemaphoreHandle& handle)
{
	if (!handle)
	{
		throw std::logic_error("cannot try lock null semaphore");
	}
	DWORD result = ::WaitForSingleObject(handle.get(), 0);
	if (result == WAIT_TIMEOUT)
	{
		return SemaphoreLock(SemaphoreHandle(nullptr), AlreadyLocked{});
	}
	return SemaphoreLock(handle, AlreadyLocked{});
}
