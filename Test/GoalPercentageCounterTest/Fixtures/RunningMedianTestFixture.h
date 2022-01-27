#pragma once

#include <gmock/gmock.h>

#include <Plugin/Data/RunningMedian.h>

class RunningMedianTestFixture : public ::testing::Test
{
public:
	RunningMedian runningMedian;

	void SetUp() override
	{
		runningMedian.reset();
	}

	void expectMedian(float value)
	{
		EXPECT_EQ(runningMedian.getMedian(), value);
	}

	void expectCount(size_t value)
	{
		EXPECT_EQ(runningMedian.getCount(), value);
	}
};
