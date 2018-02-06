#pragma once
#include <stdint.h>
#include <random>

using PiEstimation = std::pair<uint64_t, uint64_t>;

class CalculatePiEstimation
{
public:
	CalculatePiEstimation();

	CalculatePiEstimation(const CalculatePiEstimation& other);
	CalculatePiEstimation& operator =(const CalculatePiEstimation& other);

	PiEstimation operator()() const;

private:
	mutable std::mt19937 m_engine;
	std::uniform_real_distribution<float> m_coordDistribution;
};

class ReducePiEstimation
{
public:
	PiEstimation operator()(PiEstimation a, PiEstimation b) const;
};

double GetPiFromEstimation(PiEstimation estimation);
