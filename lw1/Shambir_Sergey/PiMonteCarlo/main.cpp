#include "stdafx.h"
#include "RunMonteCarlo.h"
#include "PiEstimation.h"
#include <stdint.h>
#include <iostream>

constexpr unsigned MAX_THREADS = 100;

struct CommandLineArgs
{
	unsigned iterationsCount = 0;
	unsigned threadCount = 0;
};

void PrintUsage(const std::string& selfName)
{
	std::cout
		<< "Calculates PI using Monte-Carlo heuristic method"
		<< "Usage: " << selfName << " <iterations_count> <thread_count>" << std::endl;
}

CommandLineArgs ParseCommandLineArgs(int argc, char* argv[])
{
	if (argc != 3)
	{
		throw std::logic_error("missing required arguments");
	}
	CommandLineArgs args;
	args.iterationsCount = std::stoul(argv[1]);
	args.threadCount = std::stoul(argv[2]);
	if (args.iterationsCount == 0 || args.threadCount == 0)
	{
		throw std::invalid_argument("arguments cannot be zero");
	}
	if (args.threadCount > MAX_THREADS)
	{
		throw std::invalid_argument("too much threads created");
	}

	return args;
}

int main(int argc, char* argv[])
{
	CommandLineArgs args;
	try
	{
		args = ParseCommandLineArgs(argc, argv);
	}
	catch (const std::exception& ex)
	{
		std::cout << ex.what() << std::endl;
		PrintUsage((argc > 0) ? argv[0] : "program");
		return 1;
	}

	try
	{
		CalculatePiEstimation calculate;
		ReducePiEstimation reduce;
		PiEstimation estimation = RunThreadedMonteCarlo(args.iterationsCount, args.threadCount, calculate, reduce);
		std::cout << "PI = " << GetPiFromEstimation(estimation) << std::endl;
	}
	catch (const std::exception& ex)
	{
		std::cerr << "FATAL ERROR: " << ex.what() << std::endl;
	}

    return 0;
}
