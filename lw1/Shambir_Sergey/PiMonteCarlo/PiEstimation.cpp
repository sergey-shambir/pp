#include "stdafx.h"
#include "PiEstimation.h"
#include <cmath>

CalculatePiEstimation::CalculatePiEstimation()
	: m_coordDistribution{-1.f, 1.f}
{
	std::random_device device;
	m_engine.seed(device());
}

CalculatePiEstimation::CalculatePiEstimation(const CalculatePiEstimation& other)
	: m_coordDistribution(other.m_coordDistribution)
{
	std::random_device device;
	m_engine.seed(device());
}

CalculatePiEstimation& CalculatePiEstimation::operator=(const CalculatePiEstimation& other)
{
	m_coordDistribution = other.m_coordDistribution;
	std::random_device device;
	m_engine.seed(device());
	return *this;
}

PiEstimation CalculatePiEstimation::operator()() const
{
	const float x = m_coordDistribution(m_engine);
	const float y = m_coordDistribution(m_engine);
	const float distance = std::sqrt(x * x + y * y);

	if (distance > 1.f)
	{
		// Point isn't in circle, PI estimation is 0
		return PiEstimation{ 0, 1 };
	}
	// Point is in circle, PI estimation is 4
	return PiEstimation{ 4, 1 };
}

PiEstimation ReducePiEstimation::operator()(PiEstimation a, PiEstimation b) const
{
	return{ a.first + b.first, a.second + b.second };
}

double GetPiFromEstimation(PiEstimation estimation)
{
	return double(estimation.first) / double(estimation.second);
}
