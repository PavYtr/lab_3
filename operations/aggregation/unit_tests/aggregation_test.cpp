#include <gtest/gtest.h>
#include <utility>
#include <vector>
#include "../AvgWithPartitioning.h"

class AggregationTest : public ::testing::Test {
protected:
    void SetUp() override {
    }

    void TearDown() override {
    }
};

template <typename K, typename V>
void ExpectVectorsUnordered(std::vector<std::pair<K, V>> actual, 
                               std::vector<std::pair<K, V>> expected) {
    ASSERT_EQ(actual.size(), expected.size());

    auto comparator = [](const auto& a, const auto& b) {
        return a.first < b.first;
    };
    std::sort(actual.begin(), actual.end(), comparator);
    std::sort(expected.begin(), expected.end(), comparator);

    for (size_t i = 0; i < actual.size(); ++i) {
        EXPECT_EQ(actual[i].first, expected[i].first);
        
        if constexpr (std::is_floating_point_v<V>) {
            EXPECT_NEAR(actual[i].second, expected[i].second, 1e-5);
        } else {
            EXPECT_EQ(actual[i].second, expected[i].second);
        }
    }
}

TEST_F(AggregationTest, Simple) {
    std::vector<std::pair<int, int>> tmp;
    auto res = AvgWithPartitioning<int, int>(tmp).size();
    EXPECT_EQ(res, 0);
}

TEST_F(AggregationTest, SingleKeySingleValue){
    std::vector<std::pair<int, int>> tmp = {{1, 10}};
    auto res = AvgWithPartitioning<int, int>(tmp);
    ASSERT_EQ(res.size(), 1);
    EXPECT_EQ(res[0].first, 1);
    EXPECT_DOUBLE_EQ(res[0].second, 10.0);
}

TEST_F(AggregationTest, SingleKeyMultipleValues){
    std::vector<std::pair<int, int>> tmp = {
        {1, 10}, 
        {1, 20}, 
        {1, 30} 
    };
    auto res = AvgWithPartitioning<int, int>(tmp);
    ASSERT_EQ(res.size(), 1);
    EXPECT_EQ(res[0].first, 1);
    EXPECT_DOUBLE_EQ(res[0].second, 20);
}

TEST_F(AggregationTest, MultipleKeysMultipleValues){
    std::vector<std::pair<int, int>> tmp = { 
        {1, 10}, 
        {2, 20}, 
        {1, 30}, 
        {2, 40}, 
        {3, 50} 
    };
    auto res = AvgWithPartitioning<int, int>(tmp, 2);
    ASSERT_EQ(res.size(), 3);

    std::vector<std::pair<int, int>> expected = {
        {1, 20},
        {2, 30},
        {3, 50}
    };

    ExpectVectorsUnordered(res, expected);
}

TEST_F(AggregationTest, DifferentDataTypes){
    std::vector<std::pair<std::string, double>> tmp = {
        {"a", 1.0}, 
        {"a", 3.0}, 
        {"b", 2.0}, 
        {"b", 9.0}
    };
    auto res = AvgWithPartitioning<std::string, double>(tmp);
    ASSERT_EQ(res.size(), 2);
    std::vector<std::pair<std::string, double>> expected = {
        {"a", 2.0},
        {"b", 5.5}
    };
    ExpectVectorsUnordered(res, expected);
}

TEST_F(AggregationTest, SmallBlocksForcePartitioning){
    std::vector<std::pair<int, int>> tmp = {
        {1, 10}, {2, 10}, {3, 10}, 
        {1, 20}, {2, 20}, {3, 20}
    };

    auto res = AvgWithPartitioning<int, int>(tmp, 2);
    ASSERT_EQ(res.size(), 3);
    
    std::vector<std::pair<int, int>> expected = {
        {1, 15},
        {2, 15},
        {3, 15}
    };

    ExpectVectorsUnordered(res, expected);

}

TEST_F(AggregationTest, NegativeValues){
    std::vector<std::pair<int, int>> tmp = {
        {1, -10}, 
        {1, -20}, 
        {2, -30}, 
        {2, -44}
    };
    auto res = AvgWithPartitioning<int, int>(tmp);
    ASSERT_EQ(res.size(), 2);
    std::vector<std::pair<int, int>> expected = {
        {1, -15},
        {2, -37}
    };
    ExpectVectorsUnordered(res, expected);
}
/*TODO*/