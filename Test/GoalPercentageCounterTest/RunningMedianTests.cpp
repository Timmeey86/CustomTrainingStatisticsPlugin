#include "Fixtures/RunningMedianTestFixture.h"

TEST_F(RunningMedianTestFixture, increasing_insert)
{
	runningMedian.insert(1.0f);
	expectMedian(1.0f);
	expectCount(1);

	runningMedian.insert(2.0f);
	expectMedian(1.5f);
	expectCount(2);

	runningMedian.insert(3.0f);
	expectMedian(2.0f);
	expectCount(3);

	runningMedian.insert(4.0f);
	expectMedian(2.5f);
	expectCount(4);

	runningMedian.insert(5.0f);
	expectMedian(3.0f);
	expectCount(5);
}

TEST_F(RunningMedianTestFixture, decreasing_insert)
{
	runningMedian.insert(5.0f);
	expectMedian(5.0f);
	expectCount(1);

	runningMedian.insert(4.0f);
	expectMedian(4.5f);
	expectCount(2);

	runningMedian.insert(3.0f);
	expectMedian(4.0f);
	expectCount(3);

	runningMedian.insert(2.0f);
	expectMedian(3.5f);
	expectCount(4);

	runningMedian.insert(1.0f);
	expectMedian(3.0f);
	expectCount(5);
}

TEST_F(RunningMedianTestFixture, high_low_high)
{
	runningMedian.insert(5.0f);
	expectMedian(5.0f);
	expectCount(1);

	runningMedian.insert(1.0f);
	expectMedian(3.0f);
	expectCount(2);

	runningMedian.insert(4.0f);
	expectMedian(4.0f);
	expectCount(3);

	runningMedian.insert(2.0f);
	expectMedian(3.0f);
	expectCount(4);

	runningMedian.insert(3.0f);
	expectMedian(3.0f);
	expectCount(5);
}

TEST_F(RunningMedianTestFixture, low_high_low)
{
	runningMedian.insert(1.0f);
	expectMedian(1.0f);
	expectCount(1);

	runningMedian.insert(5.0f);
	expectMedian(3.0f);
	expectCount(2);

	runningMedian.insert(2.0f);
	expectMedian(2.0f);
	expectCount(3);

	runningMedian.insert(4.0f);
	expectMedian(3.0f);
	expectCount(4);

	runningMedian.insert(3.0f);
	expectMedian(3.0f);
	expectCount(5);
}

TEST_F(RunningMedianTestFixture, reset_test)
{
	runningMedian.insert(1.0f);
	expectMedian(1.0f);
	expectCount(1);

	runningMedian.insert(2.0f);
	expectMedian(1.5f);
	expectCount(2);

	runningMedian.insert(3.0f);
	expectMedian(2.0f);
	expectCount(3);

	runningMedian.reset();
	expectMedian(0.0f);
	expectCount(0);

	runningMedian.insert(4.0f);
	expectMedian(4.0f);
	expectCount(1);

	runningMedian.insert(5.0f);
	expectMedian(4.5f);
	expectCount(2);

	runningMedian.insert(6.0f);
	expectMedian(5.0f);
	expectCount(3);
}