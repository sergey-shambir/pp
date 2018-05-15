#pragma once
#include "SpinMutex.h"
#include <iostream>
#include <mutex>

class SyncOut
{
public:
	template <class... Args>
	static void print(Args&&... args)
	{
		std::lock_guard<SpinMutex> lock(GetOutMutex());
		printImpl(std::forward<Args>(args)...);
	}

	template <class... Args>
	static void println(Args&&... args)
	{
		std::lock_guard<SpinMutex> lock(GetOutMutex());
		printImpl(std::forward<Args>(args)...);
		std::cout << std::endl;
	}

private:
	template <class T>
	static void printImpl(T&& head)
	{
		std::cout << std::forward<T>(head);
	}

	template <class T, class... Args>
	static void printImpl(T&& head, Args&&... tail)
	{
		std::cout << std::forward<T>(head);
		printImpl(std::forward<Args>(tail)...);
	}

	static SpinMutex& GetOutMutex()
	{
		static SpinMutex s_printMutex;
		return s_printMutex;
	}
};
