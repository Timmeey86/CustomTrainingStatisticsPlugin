#include <pch.h>
#include <cmath>
#include "RunningMean.h"

void RunningMean::reset()
{
	_count = 0;
	_mean = 0;
	_populationVariance = 0;
	_variance = 0;
	_stdDev = 0;
}

void RunningMean::insert(float value)
{
	++_count;
	float delta = value - _mean;
	_mean += (delta / _count); // Running average
	float delta2 = value - _mean;
	float M2 = (_populationVariance * (_count - 1)) + (delta * delta2);

	if (_count >= 2)
	{
		_populationVariance = M2 / _count;
		_variance = M2 / (_count - 1);
		_stdDev = std::sqrt(_variance);
	}
}

float RunningMean::getMean() const
{
	return _mean;
}

float RunningMean::getVariance() const
{
	return _variance;
}

float RunningMean::getStdDev() const
{
	return _stdDev;
}

size_t RunningMean::getCount() const
{
	return _count;
}