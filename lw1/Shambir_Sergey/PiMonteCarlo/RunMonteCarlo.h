#pragma once

#include "RunMonteCarlo_detail.h"

template <typename CalculateFn, typename ReduceFn>
decltype(auto) RunThreadedMonteCarlo(
	unsigned iterationCount, unsigned threadCount,
	const CalculateFn& calculate, const ReduceFn& reduce)
{
	using Result = decltype(calculate());
	using ReduceResult = decltype(reduce(Result(), Result()));
	static_assert(std::is_same<Result, ReduceResult>::value, "reduce should not change value type");

	detail::ThreadedMonteCarloRunner<CalculateFn, ReduceFn> runner(iterationCount, threadCount, calculate, reduce);
	return runner();
}
