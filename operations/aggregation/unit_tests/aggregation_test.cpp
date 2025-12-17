#include <gtest/gtest.h>
#include <utility>
#include <vector>
#include <map>
#include "../AvgWithPartitioning.h"

class AggregationTest : public ::testing::Test {
protected:
    void SetUp() override {
    }

    void TearDown() override {
    }
};

TEST_F(AggregationTest, Simple) {
    std::vector<std::pair<int, int>> tmp;
    auto res = AvgWithPartitioning<int, int>(tmp).size();
    EXPECT_EQ(res, 0);
}


// Common, unsorted test
TEST_F(AggregationTest, Usual) {
    std::vector<std::pair<int, int>> tmp = 
    {
        {  2,  20},
        {  3,  15},
        {  1,   7},
        { -3,  10},
        {  1,   3},
        {  2,   5},
        {100,   5},
        { 57,   2},
        { 13,   4},
        {100,  -5},
        {  2,   2}
    };
    auto res = AvgWithPartitioning<int, int>(tmp);
    EXPECT_EQ(res.size(), 7);
    
    // check what it contains
    std::map<int, int> perfect = 
    {
        { -3,  10},
        {  1,   5},
        {  2,   9},
        {  3,  15},
        { 13,   4},
        { 57,   2},
        {100,   0}
    };
    std::map<int, int> current(res.begin(), res.end());
    EXPECT_EQ(perfect, current);
}

// Check when all keys are unique + check for new type(double)
TEST_F(AggregationTest, Unique) {
    std::vector<std::pair<double, double>> tmp = 
    {
        {3.14, 22.8},
        {1.337, 14.88},
        {31.2025, 31.2026},
        {2222222, 33333333},
        {0.01, 0.01},
        {7.62, 5.45},
        {1e3, 1e2},
        {4.04, 50.5},
        {333, 707},
        {2.718281828, 1.618}
    };
    auto res = AvgWithPartitioning<double, double>(tmp);
    EXPECT_EQ(res.size(), 10);
}

// Check when all keys are the same
TEST_F(AggregationTest, Same) {
    std::vector<std::pair<char, int>> tmp = 
    {
        {'a', 1},
        {'a', 2},
        {'a', 3},
        {'a', 4},
        {'a', 5}
    };
    auto res = AvgWithPartitioning<char, int>(tmp);
    EXPECT_EQ(res.size(), 1);
    std::vector<std::pair<char, int>> perfect = {{'a', 3}};
    EXPECT_EQ(res, perfect);
}

// Check when only one element
TEST_F(AggregationTest, OneOnly) {
    std::vector<std::pair<int, int>> tmp = 
    {
        {4, 777}
    };
    auto res = AvgWithPartitioning<int, int>(tmp);
    EXPECT_EQ(res.size(), 1);
    std::vector<std::pair<int, int>> perfect = {{4, 777}};
    EXPECT_EQ(res, perfect);
}

// Check when there is a new block_size
TEST_F(AggregationTest, ChangedBlockSize) {
    std::vector<std::pair<int, int>> tmp = 
    {
        {3, 4}, {2, 5}, {77, 44}, {2, 3}, {2, 3}, {555, 123}, {3, 4}, {0, -2}, {22, 22}, {-1, 1}
    };

    auto res1 = AvgWithPartitioning<int, int>(tmp, 2);
    auto res2 = AvgWithPartitioning<int, int>(tmp, 1);
    auto res3 = AvgWithPartitioning<int, int>(tmp, 34);

    EXPECT_EQ(res2.size(), 7);
    EXPECT_EQ(res1.size(), 7);
    EXPECT_EQ(res3.size(), 7);

    std::map<int, int> perfect = 
    {
        { -1,   1},
        {  0,   -2},
        {  2,   3},
        {  3,   4},
        { 22,   22},
        { 77,   44},
        {555,   123}
    };

    std::map<int, int> current1(res1.begin(), res1.end());
    std::map<int, int> current2(res2.begin(), res2.end());
    std::map<int, int> current3(res3.begin(), res3.end());

    EXPECT_EQ(perfect, current1);
    EXPECT_EQ(perfect, current2);
    EXPECT_EQ(perfect, current3);
}

// Check on the big_data
#include <algorithm>
#include <random>
TEST_F(AggregationTest, BigData) {
    std::vector<std::pair<int, int>> tmp;
    for (int i = 0; i != 1'000'000; ++i) {
        tmp.push_back({i, i * 10});
    }
    std::random_device rd;
    std::mt19937 g(rd());
    std::shuffle(tmp.begin(), tmp.end(), g);
    auto res = AvgWithPartitioning<int, int>(tmp);
    EXPECT_EQ(res.size(), 1'000'000);
}