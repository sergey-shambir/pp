#pragma once
#include <atomic>

// Spin mutex, a.k.a. spinlock:
//  - pros: while waiting, doesn't switch context to operating system
//  - cons: loads once CPU core while waiting resource
class SpinMutex
{
public:
	inline bool try_lock()
	{
		return !m_busy.test_and_set(std::memory_order_acquire);
	}

	inline void lock()
	{
		while (!try_lock())
		{
			/* do nothing */;
		}
	}

	inline void unlock()
	{
		m_busy.clear(std::memory_order_release);
	}

	SpinMutex() = default;
	SpinMutex(const SpinMutex&) = delete;
	SpinMutex& operator=(const SpinMutex&) = delete;
	SpinMutex(SpinMutex&&) = delete;
	SpinMutex& operator=(SpinMutex&&) = delete;

private:
	std::atomic_flag m_busy{ ATOMIC_FLAG_INIT };
};
