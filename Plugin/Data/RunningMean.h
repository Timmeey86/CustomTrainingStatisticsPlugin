#pragma once

class RunningMean
{
public:
	RunningMean() = default;

	/** Resets the mean and count to 0 */
	void reset();
	/** New value is added to update the mean, variance, and standard deviation.
	Algorithm details can be found at https://en.wikipedia.org/wiki/Algorithms_for_calculating_variance#Welford's_online_algorithm */
	void insert(float value);
	/** Returns the current mean */
	float getMean() const;
	/** Returns the current sample variance */
	float getVariance() const;
	/** Returns the current sample standard deviation */
	float getStdDev() const;
	/** Returns the current count */
	size_t getCount() const;

private:
	size_t _count{ 0 }; ///< The current count
	float _mean{ 0.0 }; ///< The current mean
	float _populationVariance{ 0.0 }; ///< The current population variance
	float _variance{ 0.0 }; ///< The current sample variance
	float _stdDev{ 0.0 }; ///< The current sample standard deviation
};