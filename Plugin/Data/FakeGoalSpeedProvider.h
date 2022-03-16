#pragma once

#include "IGoalSpeedProvider.h"

/** Allows faking goal speed statistics. This is required for storing separate "peak" values of max goal speed, min goal speed, median etc. */
class FakeGoalSpeedProvider : public IGoalSpeedProvider
{
public:

	inline void setFakeMax(float max) { _max = max; }
	inline void setFakeMin(float min) { _min = min; }
	inline void setFakeMedian(float median) { _median = median; }
	inline void setFakeMean(float mean) { _mean = mean; }

	inline void reset() override
	{
		*this = FakeGoalSpeedProvider();
	}
	inline void insert(float speed, bool isMetric = true) override { /* ignore */ }
	inline float getMostRecent(bool isMetric = true) const override { return .0f; } // Not supported
	inline float getMax(bool isMetric = true) const override { return _max; }
	inline float getMin(bool isMetric = true) const override { return _min; }
	inline float getMedian(bool isMetric = true) const override { return _median; }
	inline float getMean(bool isMetric = true) const override { return _mean; }
	inline float getStdDev(bool isMetric = true) const override { return .0f; } // Not supported
	inline size_t getCount(bool isMetric = true) const override { return 0; } // Not supported
	inline std::vector<float> getAllShotValues() const override { return std::vector<float>(); } // Not supported


private:
	float _max = .0f;
	float _min = .0f;
	float _median = .0f;
	float _mean = .0f;
};