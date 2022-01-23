#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <Plugin/Calculation/StatUpdater.h>

TEST(Dummy, SampleTest)
{
    EXPECT_TRUE(true);
    auto shotStats = std::make_shared<ShotStats>();
    auto pluginState = std::make_shared<PluginState>();
    auto statUpdater = std::make_unique<StatUpdater>(shotStats, pluginState);
}