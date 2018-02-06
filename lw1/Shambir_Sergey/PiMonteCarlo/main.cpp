#include "stdafx.h"
#include "RunMonteCarlo.h"
#include "PiEstimation.h"
#include <stdint.h>
#include <iostream>

int main()
{
	try
	{
		CalculatePiEstimation calculate;
		ReducePiEstimation reduce;
		PiEstimation estimation = RunThreadedMonteCarlo(100'000'000, 8, calculate, reduce);
		double pi = GetPiFromEstimation(estimation);

		std::cout << "PI = " << pi << std::endl;
	}
	catch (const std::exception& ex)
	{
		std::cerr << "FATAL ERROR: " << ex.what() << std::endl;
	}

    return 0;
}
