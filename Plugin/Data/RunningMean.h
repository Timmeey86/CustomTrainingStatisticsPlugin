#pragma once

class RunningMean
{
public:
	RunningMean() = default;

	/** Resets the mean and count to 0 */
	void reset();
	/** New value is added to update the mean */
	void insert(float value);
	/** Returns the current mean */
	float getMean() const;
	/** Returns the current count */
	size_t getCount() const;

private:
	size_t _count{ 0 }; ///< The current count
	float _mean{ 0.0 }; ///< The current mean
};