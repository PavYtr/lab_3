#include <gtest/gtest.h>
#include "../CacheImp.h"
#include <string>

class CacheTest : public ::testing::Test {
protected:
    void SetUp() override {
    }

    void TearDown() override {
    }
};

TEST_F(CacheTest, EmptyCacheSize) {
    LFUCache<int, int> cache(2);
    EXPECT_EQ(cache.size(), 0);
}

TEST_F(CacheTest, SetAndGetValue){
    LFUCache<int, int> cache(2);

    cache.set(1, 10);
    EXPECT_EQ(cache.get(1), 10);
    EXPECT_EQ(cache.size(), 1);

    cache.set(2, 20);
    EXPECT_EQ(cache.get(2), 20);
    EXPECT_EQ(cache.size(), 2);
}

TEST_F(CacheTest, MissingKeyException){
    LFUCache<std::string, int> cache(2);

    cache.set("a", 10);

    EXPECT_THROW(cache.get("d"), std::out_of_range);

}

TEST_F(CacheTest, EvictsLeastFrequentlyUsed){
    LFUCache <int, int> cache(2);

    cache.set(1, 10);
    cache.set(2, 20);

    cache.get(1);

    cache.set(3, 30);

    EXPECT_FALSE(cache.contains(2));
    EXPECT_TRUE(cache.contains(1));
    EXPECT_TRUE(cache.contains(3));
}

TEST_F(CacheTest, EvictsLRUWhenFrequencyTied){
    LFUCache<int, int> cache(3);
    for (int i = 1;i <= 4;i++){
        cache.set(i, i * 10);
    }

    EXPECT_FALSE(cache.contains(1));
    EXPECT_TRUE(cache.contains(2));
    EXPECT_TRUE(cache.contains(3));
    EXPECT_TRUE(cache.contains(4));
}

TEST_F(CacheTest, UpdateFrequencyOnExistingKey){
    LFUCache<int, int> cache(2);

    cache.set(1, 10);
    cache.set(2, 20);

    cache.set(1, 15);
    cache.set(3, 30);

    EXPECT_TRUE(cache.contains(1));
    EXPECT_TRUE(cache.contains(3));
    EXPECT_FALSE(cache.contains(2));
    EXPECT_EQ(cache.get(1), 15);
}

TEST_F(CacheTest, ClearCache){
    LFUCache<int, int> cache(2);

    cache.set(1, 10);
    cache.set(2, 20);

    EXPECT_EQ(cache.size(), 2);

    cache.clear();

    EXPECT_EQ(cache.size(), 0);
    EXPECT_FALSE(cache.contains(1));
    EXPECT_FALSE(cache.contains(2));

    cache.set(3, 30);
    EXPECT_EQ(cache.size(), 1);
    EXPECT_TRUE(cache.contains(3));
    EXPECT_EQ(cache.get(3), 30);
}

TEST_F(CacheTest, ZeroCapacityCache){
    LFUCache<int, int> cache(0);

    cache.set(1, 10);
    EXPECT_EQ(cache.size(), 0);
    EXPECT_FALSE(cache.contains(1));
    EXPECT_THROW(cache.get(1), std::out_of_range);
}

TEST_F(CacheTest, ComplexScenario){
    LFUCache<int, int> cache(3);

    cache.set(1, 10);
    cache.set(2, 20);
    cache.set(3, 39);

    for(int i = 0;i < 4;i++){
        cache.get(1);
    }

    cache.get(2);

    cache.set(4, 40);
    EXPECT_FALSE(cache.contains(3));
    EXPECT_TRUE(cache.contains(1));
    EXPECT_TRUE(cache.contains(2));
    EXPECT_TRUE(cache.contains(4));

    cache.set(5, 59);
    EXPECT_FALSE(cache.contains(4));

    cache.get(5);
    cache.get(5);

    cache.set(6, 60);
    EXPECT_FALSE(cache.contains(2));
    EXPECT_TRUE(cache.contains(1));
    EXPECT_TRUE(cache.contains(5));
    EXPECT_TRUE(cache.contains(6));
}