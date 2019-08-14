#include <gtest/gtest.h>
#include <random>
#include <vector>
#include <algorithm>
#include "../include/constants.hpp"
#include <ancillary/algorithm/partition.hpp>

TEST(PartitionTests, UnstablePartitionTest) {
	std::srand(std::time(nullptr));
	for (size_t i = 1; i <= N; ++i) {
		std::vector<int> data;
		std::generate_n(std::back_inserter(data), i, []() { return std::rand() % 100 + 1; });
		std::vector<int> num1(data);
		std::vector<int> num2(data);
		std::partition(num1.begin(), num1.end(), [pivot = data.back()](auto value) { return pivot > value; });
		ancillary::partition(num2.begin(), num2.end(), [pivot = data.back()](auto value) { return pivot > value; });
		ASSERT_EQ(num1, num2);
	}
}

TEST(PartitionTests, StablePartitionTest) {
	std::srand(std::time(nullptr));
	for (size_t i = 1; i <= N; ++i) {
		std::vector<int> data;
		std::generate_n(std::back_inserter(data), i, []() { return std::rand() % 100 + 1; });
		std::vector<int> num1(data);
		std::vector<int> num2(data);
		std::stable_partition(num1.begin(), num1.end(), [pivot = data.back()](auto value) { return pivot > value; });
		ancillary::stable_partition(num2.begin(), num2.end(), [pivot = data.back()](auto value) { return pivot > value; });
		ASSERT_EQ(num1, num2);
	}
}