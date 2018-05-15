#pragma once

#include "winapi_raii.h"
#include <functional>
#include <memory>

class Thread final
{
public:
	/// Creates thread with given procedure.
	/// @param proc - must never throw, otherwise terminate will be called.
	explicit Thread(const std::function<void()>& proc);

	Thread(Thread&&);
	Thread& operator=(Thread&&);

	Thread(const Thread&) = delete;
	Thread& operator=(const Thread&) = delete;

	~Thread();

	/// Joins thread if still exists.
	void Join();

private:
	class Impl;
	std::unique_ptr<Impl> m_impl;
};
