#include <gtest/gtest.h>
#include <random>
#include <vector>
#include <cstdint>
#include <algorithm>
#include <unordered_set>
#include "constants.hpp"
#include "../include/ancillary/container/sparse_set.hpp"

using sparse_set_t = ancillary::sparse_set<std::uint64_t>;

TEST(SparseSetTests, ConstructorTests) {
	sparse_set_t ss1;
	ASSERT_TRUE(ss1.empty());

	auto list = { 1, 2, 3, 4, 5 };
	sparse_set_t ss2({1, 2, 3, 4, 5});
	ASSERT_EQ(ss2.size(), list.size());
	ASSERT_TRUE(std::equal(list.begin(), list.end(), ss2.begin(), ss2.end()));

	sparse_set_t ss3(ss2);
	ASSERT_EQ(ss3.size(), list.size());
	ASSERT_FALSE(ss2.empty());
	ASSERT_TRUE(std::equal(list.begin(), list.end(), ss3.begin(), ss3.end()));

	sparse_set_t ss4(std::move(ss3));
	ASSERT_EQ(ss4.size(), list.size());
	ASSERT_TRUE(ss3.empty());
	ASSERT_TRUE(std::equal(list.begin(), list.end(), ss4.begin(), ss4.end()));
}

TEST(SparseSetTests, AssignmentTests) {
	std::initializer_list<std::uint64_t> list1 = { 1, 2, 3, 4, 5 };
	std::initializer_list<std::uint64_t> list2 = { 5, 6, 7, 8, 9 };
	sparse_set_t ss1(list1);
	ASSERT_TRUE(std::equal(ss1.begin(), ss1.end(), list1.begin(), list1.end()));
	sparse_set_t ss2;
	ss2 = ss1;
	ASSERT_TRUE(std::equal(ss2.begin(), ss2.end(), list1.begin(), list1.end()));
	sparse_set_t ss3;
	ss3 = std::move(ss2);
	ASSERT_TRUE(ss2.empty());
	ASSERT_TRUE(std::equal(ss3.begin(), ss3.end(), list1.begin(), list1.end()));
	ss3 = list2;
	ASSERT_TRUE(std::equal(ss3.begin(), ss3.end(), list2.begin(), list2.end()));
}

TEST(SparseSetTests, IteratorTests) {
	std::initializer_list<std::uint64_t> list = { 1, 2, 3, 4, 5 };
	sparse_set_t ss1(list);
	auto it = ss1.begin();
	ASSERT_EQ(*it, 1);
	++it;
	ASSERT_EQ(*it, 2);

	auto it2 = ss1.begin();
	ASSERT_NE(it, it2);
	it2++;
	ASSERT_EQ(it, it2);
}

TEST(SparseSetTests, InsertionTests) {
	std::srand(std::time(nullptr));
	std::vector<std::uint64_t> integers;
	for (std::uint64_t i = 0; i < N; ++i)
		integers.emplace_back(i);
	std::shuffle(integers.begin(), integers.end(), std::mt19937{ std::random_device{}() });
	sparse_set_t set;
	set.insert(integers.begin(), integers.end());
	ASSERT_TRUE(std::equal(set.begin(), set.end(), integers.begin(), integers.end()));
}

TEST(SparseSetTests, ErasingTests) {
	std::srand(std::time(nullptr));
	std::vector<std::uint64_t> integers;
	for (std::uint64_t i = 0; i < N; ++i)
		integers.emplace_back(i);
	std::shuffle(integers.begin(), integers.end(), std::mt19937{ std::random_device{}() });
	sparse_set_t set;
	set.insert(integers.begin(), integers.end());
	ASSERT_TRUE(std::equal(set.begin(), set.end(), integers.begin(), integers.end()));
	std::shuffle(integers.begin(), integers.end(), std::mt19937{ std::random_device{}() });
	for (auto integer : integers) {
		ASSERT_TRUE(set.contains(integer));
		set.erase(integer);
		ASSERT_FALSE(set.contains(integer));
	}

}

TEST(SparseSetTests, SwapTests) {
	sparse_set_t ss1 = { 1, 2, 3, 4, 5 };
	sparse_set_t ss2 = { 5, 6, 7, 8, 9 };
	std::swap(ss1, ss2);
	std::initializer_list<std::uint64_t> list1 = { 1, 2, 3, 4, 5 };
	std::initializer_list<std::uint64_t> list2 = { 5, 6, 7, 8, 9 };
	ASSERT_TRUE(std::equal(ss1.begin(), ss1.end(), list2.begin(), list2.end()));
	ASSERT_TRUE(std::equal(ss2.begin(), ss2.end(), list1.begin(), list1.end()));
}
