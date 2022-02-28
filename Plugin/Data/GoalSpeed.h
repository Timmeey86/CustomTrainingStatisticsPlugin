#pragma once

#include "RunningMedian.h"
#include "RunningMean.h"

#include <limits>

#include "../DLLImportExport.h"


class GOALPERCENTAGECOUNTER_IMPORT_EXPORT GoalSpeed
{
public:
	GoalSpeed() = default;

	/** Resets all of the goal speed values */
	void reset();
	/** Updates the goal speed values. The speed should either be KPH (isMetric is true) or MPH (isMetric is false) */
	void insert(float speed, bool isMetric = true);
	/** Returns the most recent goal speed in KPH if isMetric is true and MPH otherwise */
	float getMostRecent(bool isMetric = true) const;
	/** Returns the maximum goal speed in KPH if isMetric is true and MPH otherwise */
	float getMax(bool isMetric = true) const;
	/** Returns the minimum goal speed in KPH if isMetric is true and MPH otherwise */
	float getMin(bool isMetric = true) const;
	/** Returns the median goal speed in KPH if isMetric is true and MPH otherwise */
	float getMedian(bool isMetric = true) const;
	/** Returns the mean goal speed in KPH if isMetric is true and MPH otherwise */
	float getMean(bool isMetric = true) const;
	/** Returns the total number of goals */
	size_t getCount(bool isMetric = true) const;
	/** Returns the recorded shot values as supplied to insert(). */
	inline std::vector<float> getAllShotValues() const { return _allSpeedValues; }
	
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