#include <pch.h>
#include "RunningMean.h"

void RunningMean::reset()
{
	_count = 0;
	_mean = 0;
}

void RunningMean::insert(float value)
{
	++_count;
	_mean += ((value - _mean) / _count); // Running average
}

float RunningMean::getMean() const
{
	return _mean;
}

size_t RunningMean::getCount() const
{
	return _count;
}