#pragma once

#include <type_traits>

// Extracts element type of range type T.
template <typename T>
using range_element_type_t = std::remove_reference_t<decltype(*std::begin(std::declval<T&>()))>;

inline bool IsPowerOf2OrZero(size_t x)
{
	return (x & (x - 1)) == 0;
}
