#pragma once

#include <queue>

/** Calculates a median efficiently in log(N) time using 2 different priority queues (heaps). The algorithm is explained here (https://stackoverflow.com/a/10931091) */
class RunningMedian
{
public:
	RunningMedian() = default;

	/** Resets the median and empties the heaps */
	void reset();
	/** New value is added to update the median */
	void insert(float value);
	/** Returns the current median */
	float getMedian() const;
	/** Returns the current count */
	size_t getCount() const;

private:
	float _median;								///< The current median
	std::priority_queue<float> _left_max_heap;	///< The numbers less than or equal to the median are stored in this heap
	std::priority_queue<float, std::vector<float>, std::greater<float>> _right_min_heap;	///< The numbers greater than or equal to the median are stored in this heap
};