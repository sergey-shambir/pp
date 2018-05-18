/*
    Вариант 2 (20 баллов)
    Сортировка одномерного массива слиянием Бэтчера.
    Имя файла со входным массивом задается аргументом командной строки.
    После завершения приложение должно выдавать результат и затраченное время.
*/

#include "stdafx.h"
#include "FastOddEvenMergeSort.h"
#include "OddEvenMergeSort.h"
#include <cassert>
#include <chrono>
#include <iostream>
#include <numeric>
#include <random>

template <class Callback>
void TestSortFunction(Callback&& callback)
{
	for (size_t arraySize = 2; arraySize < 256 * 1024; arraySize *= 2)
	{
		for (unsigned seed : { 17, 1791, 2337, 5712 })
		{
			std::mt19937 engine{ seed };
			std::uniform_real_distribution<float> distribution{ 0, 100 };
			std::vector<float> numbers(arraySize);
			for (float& n : numbers)
			{
				n = distribution(engine);
			}
			callback(numbers);
		}
	}
}

template <class Callback>
void MeasureOperationTime(const std::string& name, Callback&& callback)
{
	const auto beginTime = std::chrono::steady_clock::now();
	callback();
	const auto endTime = std::chrono::steady_clock::now();
	const auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - beginTime);
	std::cout << name << " finished in " << duration.count() << " msec" << std::endl;
}

int main()
{
	volatile float preventOptimize = 0;

	std::cout << "checking correctness..." << std::endl;

	// Check sorting correctness: OddEvenMergeSort, FastOddEvenMergeSort and std::sort must have same results.
	TestSortFunction([](std::vector<float>& numbers) {
		std::vector<float> numbersCopy = numbers;
		std::vector<float> numbersControlCopy = numbers;

		OddEvenMergeSort(numbers);
		FastOddEvenMergeSort(numbersCopy);
		std::sort(std::begin(numbersControlCopy), std::end(numbersControlCopy));
		if (numbers != numbersControlCopy || numbersCopy != numbersControlCopy)
		{
			throw std::logic_error("sort produced incorrect results");
		}
	});

	std::cout << "benchmarking..." << std::endl;

	// Measure OddEvenMergeSort speed.
	MeasureOperationTime("odd-even merge sort (no OpenMP)", [&] {
		TestSortFunction([&](std::vector<float>& numbers) {
			std::vector<float> numbersCopy = numbers;
			OddEvenMergeSort(numbers);
			preventOptimize += std::accumulate(numbers.begin(), numbers.end(), 0.f);
		});
	});

	// Measure FastOddEvenMergeSort speed.
	MeasureOperationTime("odd-even merge sort (with OpenMP)", [&] {
		TestSortFunction([&](std::vector<float>& numbers) {
			std::vector<float> numbersCopy = numbers;
			FastOddEvenMergeSort(numbers);
			preventOptimize += std::accumulate(numbers.begin(), numbers.end(), 0.f);
		});
	});

	return 0;
}
