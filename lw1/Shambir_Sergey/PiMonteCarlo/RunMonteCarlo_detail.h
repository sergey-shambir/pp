#pragma once
#include "ThreadHandle.h"
#include <type_traits>
#include <algorithm>

namespace detail
{
template <typename CalculateFn, typename ReduceFn>
class ThreadedMonteCarloRunner
{
public:
	using Result = std::result_of_t<CalculateFn()>;

	struct Data
	{
		CalculateFn calculate;
		ReduceFn reduce;
		Result result;
		unsigned iterations;
		std::exception_ptr exception;
	};

	explicit ThreadedMonteCarloRunner(
		unsigned iterationCount, unsigned threadCount,
		const CalculateFn& calculate, const ReduceFn& reduce)
		: m_threadCount((std::min)(iterationCount, threadCount))
		, m_reduce(reduce)
	{
		if (m_threadCount == 0)
		{
			throw std::invalid_argument("thread and iteration counts cannot be zero");
		}

		unsigned iterationsPerThread = (iterationCount + threadCount - 1) / threadCount;
		for (unsigned i = 0; i < m_threadCount; ++i)
		{
			unsigned nextIterations = (std::min)(iterationCount, iterationsPerThread);
			iterationCount -= nextIterations;
			m_data.emplace_back(Data{ calculate, reduce, Result{}, nextIterations });
		}
	}

	ThreadedMonteCarloRunner(const ThreadedMonteCarloRunner&) = delete;
	ThreadedMonteCarloRunner& operator =(const ThreadedMonteCarloRunner&) = delete;

	Result operator()()
	{
		if (m_started)
		{
			throw std::logic_error("already running");
		}
		m_started = true;
		SpawnThreads();
		JoinThreads();

		return ReduceFinalData();
	}

private:
	static DWORD WINAPI CalculateThreadProc(void* opaque)
	{
		Data* data = reinterpret_cast<Data*>(opaque);
		try
		{
			for (unsigned i = 0; i < data->iterations; ++i)
			{
				data->result = data->reduce(data->result, data->calculate());
			}
		}
		catch (...)
		{
			data->exception = std::current_exception();
			return 1;
		}
		return 0;
	}

	void SpawnThreads()
	{
		for (unsigned i = 0; i < m_threadCount; ++i)
		{
			m_threads.emplace_back(CreateThread(CalculateThreadProc, &m_data[i]));
		}
	}

	void JoinThreads()
	{
		HANDLE& handle = m_threads[0];
		::WaitForMultipleObjects(DWORD(m_threads.size()), &handle, TRUE, INFINITE);
	}

	Result ReduceFinalData()
	{
		Result result{};
		for (Data& data : m_data)
		{
			if (data.exception)
			{
				std::rethrow_exception(data.exception);
			}
			result = m_reduce(result, data.result);
		}
		return result;
	}

	unsigned m_threadCount = 0;
	ReduceFn m_reduce{};
	bool m_started = false;

	std::vector<ThreadHandle> m_threads;
	std::vector<Data> m_data;
};
}
