#include "stdafx.h"
#include "Thread.h"

class Thread::Impl
{
public:
	Impl(const std::function<void()>& proc)
		: m_proc(proc)
	{
		if (!proc)
		{
			throw std::logic_error("thread proc cannot be empty");
		}
		m_handle = CreateThreadSafely(&Impl::ThreadProc, reinterpret_cast<void*>(this));
	}

	~Impl()
	{
		Join();
	}

	void Join()
	{
		if (m_handle)
		{
			::WaitForSingleObject(m_handle.get(), INFINITE);
			m_handle.Reset();
		}
	}

private:
	static DWORD WINAPI ThreadProc(void* opaque)
	{
		auto pThis = reinterpret_cast<Impl*>(opaque);
		pThis->InstanceThreadProc();
		return 0;
	}

	void InstanceThreadProc()
	{
		try
		{
			m_proc();
		}
		catch (...)
		{
			std::terminate();
		}
	}

	ThreadHandle m_handle;
	std::function<void()> m_proc;
};

Thread::Thread(const std::function<void()>& proc)
	: m_impl(std::make_unique<Impl>(proc))
{
}

Thread::Thread(Thread&&) = default;

Thread& Thread::operator=(Thread&&) = default;

Thread::~Thread() = default;

void Thread::Join()
{
	m_impl->Join();
}
