#pragma once

#include "RunningMedian.h"
#include "RunningMean.h"
#include "IGoalSpeedProvider.h"

#include <limits>

#include "../DLLImportExport.h"

class GOALPERCENTAGECOUNTER_IMPORT_EXPORT GoalSpeed : public IGoalSpeedProvider
{
public:
	GoalSpeed() = default;

	void reset() override;
	void insert(float speed, bool isMetric = true) override;
	float getMostRecent(bool isMetric = true) const override;
	float getMax(bool isMetric = true) const override;
	float getMin(bool isMetric = true) const override;
	float getMedian(bool isMetric = true) const override;
	float getMean(bool isMetric = true) const override;
	float getStdDev(bool isMetric = true) const override;
	size_t getCount(bool isMetric = true) const override;
	inline std::vector<float> getAllShotValues() const override { return _allSpeedValues; }
	
private:
	float convertSpeed(float metricSpeed, bool isMetric) const;

	static float constexpr _DEFAULT_VALUE{ 0.0 };			///< Default value for when the goal speed has not been set yet
	static float constexpr _KPH_TO_MPH{ 0.6213711922f };	///< Multiply KPH by this constant to convert to MPH

	float _mostRecentSpeed{ _DEFAULT_VALUE };	///< Most recent goal speed
	float _maxSpeed{ _DEFAULT_VALUE };			///< Maximum goal speed
	float _minSpeed{ _DEFAULT_VALUE };			///< Minimum goal speed
	RunningMedian _medianSpeed;					///< Median goal speed
	RunningMean _meanSpeed;						///< Mean goal speed
	std::vector<float> _allSpeedValues;			///< Stores all goal speeds which have been recorded
};