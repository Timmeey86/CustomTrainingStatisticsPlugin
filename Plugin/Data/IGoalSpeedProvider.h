#pragma once

#include "../DLLImportExport.h"

#include <vector>

// TODO: No longer compare standard deviation: A low standard deviation for slow shots might be worse than a higher one for fast shots (as long as being fast is even the point)
/** Defines the interface for a class which provides goal speed statistics. It is an interface since some code paths need to fake these values. */
class GOALPERCENTAGECOUNTER_IMPORT_EXPORT IGoalSpeedProvider
{
protected:
	IGoalSpeedProvider() = default;

public:
	virtual ~IGoalSpeedProvider() = default;

	/** Resets all of the goal speed values */
	virtual void reset() = 0;
	/** Updates the goal speed values. The speed should either be KPH (isMetric is true) or MPH (isMetric is false) */
	virtual void insert(float speed, bool isMetric = true) = 0;
	/** Returns the most recent goal speed in KPH if isMetric is true and MPH otherwise */
	virtual float getMostRecent(bool isMetric = true) const = 0;
	/** Returns the maximum goal speed in KPH if isMetric is true and MPH otherwise */
	virtual float getMax(bool isMetric = true) const = 0;
	/** Returns the minimum goal speed in KPH if isMetric is true and MPH otherwise */
	virtual float getMin(bool isMetric = true) const = 0;
	/** Returns the median goal speed in KPH if isMetric is true and MPH otherwise */
	virtual float getMedian(bool isMetric = true) const = 0;
	/** Returns the mean goal speed in KPH if isMetric is true and MPH otherwise */
	virtual float getMean(bool isMetric = true) const = 0;
	/** Returns the mean goal speed in KPH if isMetric is true and MPH otherwise */
	virtual float getStdDev(bool isMetric = true) const = 0;
	/** Returns the total number of goals */
	virtual size_t getCount(bool isMetric = true) const = 0;
	/** Returns the recorded shot values as supplied to insert(). */
	virtual std::vector<float> getAllShotValues() const = 0;
};