#pragma once
#include "winapi_raii.h"

struct AlreadyLocked
{
};

class SemaphoreLock
{
public:
	explicit SemaphoreLock(const SemaphoreHandle& handle);
	explicit SemaphoreLock(const SemaphoreHandle& handle, AlreadyLocked) noexcept;
	~SemaphoreLock();

	explicit operator bool() const noexcept;

	SemaphoreLock(SemaphoreLock&&) noexcept;
	SemaphoreLock& operator=(SemaphoreLock&&) noexcept;

	SemaphoreLock(const SemaphoreLock&) = delete;
	SemaphoreLock& operator=(const SemaphoreLock&) = delete;

private:
	HANDLE m_handle = nullptr;
};

SemaphoreLock LockSemaphore(const SemaphoreHandle& handle);
SemaphoreLock TryLockSemaphore(const SemaphoreHandle& handle);
