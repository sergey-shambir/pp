#pragma once
#include "Utility.h"
#include <cassert>
#include <iterator>
#include <stdexcept>
#include <utility>

template <class T>
class FastOddEvenMergeSorter
{
public:
	template <class Array>
	FastOddEvenMergeSorter(Array& container)
		: m_data(std::data(container))
		, m_size(std::size(container))
	{
		if (!IsPowerOf2OrZero(m_size))
		{
			throw std::logic_error("this sort algorithm can sort only when size is power of 2");
		}
	}

	void operator()()
	{
		for (ptrdiff_t p = m_size; p > 0; p >>= 1)
		{
			ptrdiff_t q = m_size;
			ptrdiff_t r = 0;
			ptrdiff_t d = p;
			bool canContinue = false;
			do
			{
				ptrdiff_t subRangeSize = m_size - d;

#pragma omp parallel for
				for (ptrdiff_t i = 0; i < subRangeSize; ++i)
				{
					if ((i & p) == r)
					{
						CompareExchange(i, i + d);
					}
				}
				canContinue = (q != p);
				if (canContinue)
				{
					d = q - p;
					q >>= 1;
					r = p;
				}
			} while (canContinue);
		}
	}

private:
	void CompareExchange(size_t li, size_t ri)
	{
		assert(li < m_size);
		assert(ri < m_size);
		if (m_data[li] > m_data[ri])
		{
			std::swap(m_data[li], m_data[ri]);
		}
	}

	T* m_data = nullptr;
	size_t m_size = 0;
};

// Sorts array using odd-even merge sort.
// Uses OpenMP to accelerate sorting operations.
template <class Array>
void FastOddEvenMergeSort(Array& array)
{
	using Element = range_element_type_t<Array>;
	FastOddEvenMergeSorter<Element> sorter(array);
	sorter();
}
