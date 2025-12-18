#include <gtest/gtest.h>
#include "../CacheImp.h"

class CacheTest : public ::testing::Test {
protected:
    void SetUp() override {
    }

    void TearDown() override {
    }
};

#include <iostream>
// Usual test: put, get different and same values;
TEST_F(CacheTest, PutAndGet) {
    SharedCache<int, int> cache;
    cache.set(1, 2);
    cache.set(2, 3);
    cache.set(3, 2);
    cache.set(4, 2);
    cache.set(1, 4);
    cache.set(5, 3);
    cache.set(6, 1);
    cache.set(10, 2);

    EXPECT_EQ(cache.size(), 5);

    // Try to find values that have already been dropped from the cache
    try {
        cache.get(2);
        throw std::exception();
    } catch(std::out_of_range& ex) {
        // successfully catched the exception
    }
}

// Try to insert the same value and the other type
TEST_F(CacheTest, SameValues) {
    SharedCache<char, short> cache;
    cache.set('a', 2);
    cache.set('a', 0);
    cache.set('a', -1);
    cache.set('a', 3);
    cache.set('a', 4);
    cache.set('a', 222);
    cache.set('a', 11);

    EXPECT_EQ(cache.size(), 1);

    // check if the value has changed
    EXPECT_EQ(cache.get('a'), 11);
}

// Check an uncommon buffer size
TEST_F(CacheTest, ChangedBufferSize) {
    // buffer_size = 1
    {
        SharedCache<int, double> cache(1);
        cache.set(1, 2.25);
        cache.set(7 * 1443 * 19, 1.41);
        cache.set(3, 3.3333333);
        EXPECT_EQ(cache.size(), 1);
        EXPECT_EQ(cache.get(3), 3.3333333);
    }

    // buffer_size = 2
    {
        SharedCache<std::byte, signed> cache(2);
        cache.set(std::byte(2), 2 / 1 + 3 - 5);
        cache.set(std::byte(3), 1 * 1 * 1 * 1);
        cache.set(std::byte(-22), 13);
        EXPECT_EQ(cache.size(), 2);
        // check if the first value has been dropped
        EXPECT_EQ(cache.contains(std::byte(2)), false);
    }
    
    // buffer_size = 777, all values are unique
    {
        SharedCache<int, int> cache(777);
        for (int i = 0; i != 1000; ++i) {
            cache.set(i, i - 10 * 2);
        }
        EXPECT_EQ(cache.size(), 777);
        EXPECT_EQ(cache.contains(999), true);
        EXPECT_EQ(cache.get(999), 979);
    }

    // buffer_size = 777, all values are the same
    {
        SharedCache<int, int> cache(777);
        for (int i = 0; i != 1000; ++i) {
            cache.set(0, i + 1);
        }
        EXPECT_EQ(cache.size(), 1);
        EXPECT_EQ(cache.get(0), 1000);
    }
}

// Check an empty cache
TEST_F(CacheTest, EmptyCache) {
    SharedCache<int, int> cache;

    EXPECT_EQ(cache.size(), 0);
    EXPECT_EQ(cache.contains(0), false);
}

// Check the method SharedCache<T, T>::clear()
TEST_F(CacheTest, ClearCache) {
    SharedCache<double, float> cache;
    cache.set(3.14, 3.14f);
    cache.set(14.14141414, 0.0f);
    cache.set(9.99, 3.04f);
    cache.set(3.14, 0.005f);
    cache.clear();
    EXPECT_EQ(cache.size(), 0);
    EXPECT_EQ(cache.contains(3.14), false);
}