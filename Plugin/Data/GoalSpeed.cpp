#include <pch.h>
#include "GoalSpeed.h"

#include <algorithm>

void GoalSpeed::reset()
{
	_mostRecentSpeed = _DEFAULT_VALUE;
	_maxSpeed = _DEFAULT_VALUE;
	_minSpeed = _DEFAULT_VALUE;
	_medianSpeed.reset();
	_meanSpeed.reset();
	_allSpeedValues.clear();
}

void GoalSpeed::insert(float speed, bool isMetric)
{
	_allSpeedValues.emplace_back(speed);
	float metricSpeed = convertSpeed(speed, isMetric);
	_mostRecentSpeed = metricSpeed;
	_medianSpeed.insert(metricSpeed);
	_meanSpeed.insert(metricSpeed);

	// Should only be default value when there are 0 goals
	if (_minSpeed == _DEFAULT_VALUE)
	{
		_maxSpeed = metricSpeed;
		_minSpeed = metricSpeed;
	}
	else
	{
		_maxSpeed = std::max<float>(_maxSpeed, metricSpeed); // std::max(a, b) doens't work because of a dumb max macro
		_minSpeed = std::min<float>(_minSpeed, metricSpeed); // std::min(a, b) doens't work because of a dumb min macro
	}
}

float GoalSpeed::getMostRecent(bool isMetric) const
{
	if (_mostRecentSpeed == _DEFAULT_VALUE)
	{
		return _DEFAULT_VALUE;
	}
	return convertSpeed(_mostRecentSpeed, isMetric);
}

float GoalSpeed::getMax(bool isMetric) const
{
	if (_maxSpeed == _DEFAULT_VALUE)
	{
		return _DEFAULT_VALUE;
	}
	return convertSpeed(_maxSpeed, isMetric);
}

float GoalSpeed::getMin(bool isMetric) const
{
	if (_minSpeed == _DEFAULT_VALUE)
	{
		return _DEFAULT_VALUE;
	}
	return convertSpeed(_minSpeed, isMetric);
}

float GoalSpeed::getMedian(bool isMetric) const
{
	return convertSpeed(_medianSpeed.getMedian(), isMetric);
}

float GoalSpeed::getMean(bool isMetric) const
{
	return convertSpeed(_meanSpeed.getMean(), isMetric);
}

float GoalSpeed::getStdDev(bool isMetric) const
{
	return convertSpeed(_meanSpeed.getStdDev(), isMetric);
}

size_t GoalSpeed::getCount(bool isMetric) const
{
	return convertSpeed(_meanSpeed.getCount(), isMetric);
}

float GoalSpeed::convertSpeed(float metricSpeed, bool isMetric) const
{
	if (isMetric)
	{
		return metricSpeed;
	}
	else
	{
		return metricSpeed * _KPH_TO_MPH;
	}
}
