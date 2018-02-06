#include "stdafx.h"
#include "RunMonteCarlo.h"
#include "PiEstimation.h"
#include <stdint.h>
#include <iostream>

constexpr unsigned MAX_THREADS = 100;

#if defined(_WIN32)
constexpr char DIR_SEPARATOR = '\\';
#else
constexpr char DIR_SEPARATOR = '/';
#endif

struct CommandLineArgs
{
	unsigned iterationsCount = 0;
	unsigned threadCount = 0;
};

std::string GetApplicationName(int argc, char* argv[])
{
	if (argc < 1)
	{
		return "program";
	}
	const std::string path = argv[0];
	const size_t from = path.find_last_of(DIR_SEPARATOR);
	return path.substr((from == std::string::npos) ? 0 : from);
}

void PrintUsage(const std::string& selfName)
{
	std::cerr
		<< "Calculates PI using Monte-Carlo heuristic method\n"
		<< "Usage:\n"
		<< "  single-threaded: " << selfName << " <iterations_count>\n"
		<< "  multi-threaded: " << selfName << " <iterations_count> <thread_count>\n";
}

CommandLineArgs ParseCommandLineArgs(int argc, char* argv[])
{
	if (argc < 2 || argc > 3)
	{
		throw std::logic_error("wrong argument count");
	}
	CommandLineArgs args;
	args.iterationsCount = std::stoul(argv[1]);
	args.threadCount = (argc > 2) ? std::stoul(argv[2]) : 1u;
	if (args.iterationsCount == 0 || args.threadCount == 0)
	{
		throw std::invalid_argument("argument cannot be zero");
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
		std::cerr << "ERROR: " << ex.what() << "\n";
		PrintUsage(GetApplicationName(argc, argv));
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
		std::cerr << "FATAL ERROR: " << ex.what() << "\n";
	}

    return 0;
}
