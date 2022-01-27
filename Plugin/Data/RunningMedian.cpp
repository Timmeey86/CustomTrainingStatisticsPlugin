#include <pch.h>
#include "RunningMedian.h"

void RunningMedian::reset()
{
	_median = 0;
	while (!_left_max_heap.empty())
	{
		_left_max_heap.pop();
	}
	while (!_right_min_heap.empty())
	{
		_right_min_heap.pop();
	}
}

void RunningMedian::insert(float value)
{
	if (getCount() == 0)
	{
		_left_max_heap.push(value);
		_median = value;
		return;
	}
	else if (getCount() == 1)
	{
		if (value >= _median)
		{
			_right_min_heap.push(value);
		}
		else // value should go in the left heap and what was in left heap goes in right heap
		{
			_right_min_heap.push(_left_max_heap.top());
			_left_max_heap.pop();
			_left_max_heap.push(value);
		}
		_median = (_left_max_heap.top() + _right_min_heap.top()) / 2.0;
		return;
	}
	else // Count is >= 2, and there's a value in each heap
	{
		if (value < _median)
		{
			_left_max_heap.push(value);
		}
		else if (value > _median)
		{
			_right_min_heap.push(value);
		}
		else // Value == _median, value goes in whichever heap has the smaller size (left heap if they are the same size)
		{
			if (_left_max_heap.size() <= _right_min_heap.size())
			{
				_left_max_heap.push(value);
			}
			else
			{
				_right_min_heap.push(value);
			}
		}

		// If the size of the two heaps differ by more than 1, the root of the larger heap is removed and inserted in the smaller heap
		int sizeDiff = _left_max_heap.size() - _right_min_heap.size();
		while (sizeDiff > 1) // Left heap has 2 or more values than right heap
		{
			_right_min_heap.push(_left_max_heap.top());
			_left_max_heap.pop();
			sizeDiff = _left_max_heap.size() - _right_min_heap.size();
		}
		while (sizeDiff < -1) // Right heap has 2 or more values than left heap
		{
			_left_max_heap.push(_right_min_heap.top());
			_right_min_heap.pop();
			sizeDiff = _left_max_heap.size() - _right_min_heap.size();
		}

		// If the heaps differ in size, the current median is computed as the root of the larger heap
		// Or if they are the same size, the median is the average of the roots of the two heaps
		if (sizeDiff == 1)
		{
			_median = _left_max_heap.top();
		}
		else if (sizeDiff == 0)
		{
			_median = (_left_max_heap.top() + _right_min_heap.top()) / 2.0;
		}
		else if (sizeDiff == -1)
		{
			_median = _right_min_heap.top();
		}
		else 
		{
			// error
		}
	}
}

float RunningMedian::getMedian() const
{
	return _median;
}

size_t RunningMedian::getCount() const
{
	return _left_max_heap.size() + _right_min_heap.size();
}