#include <gtest/gtest.h>
#include <random>
#include <vector>
#include <string>
#include <algorithm>
#include "../include/employee.hpp"
#include "../include/constants.hpp"
#include <ancillary/container/flat_set.hpp>

using set_t = ancillary::flat_set<int>;

std::mt19937 gen{ std::random_device{}() };


TEST(FlatSetTests, ConstructorTests) {
	set_t s1;
	ASSERT_TRUE(s1.empty());

	std::vector<int> integers(N);
	std::generate(integers.begin(), integers.end(), [n = 0]() mutable { return n++; });
	std::shuffle(integers.begin(), integers.end(), gen);

	set_t s2(integers.begin(), integers.end());
	ASSERT_EQ(N, s2.size());
	ASSERT_TRUE(std::is_sorted(s2.begin(), s2.end()));
	ASSERT_TRUE(std::unique(s2.begin(), s2.end()) == s2.end());

	size_t size = integers.size();
	for (size_t i = 0; i < size; ++i) {
		integers.emplace_back(static_cast<int>(i));
	}
	std::shuffle(integers.begin(), integers.end(), gen);
	set_t s3(integers.begin(), integers.end());
	ASSERT_EQ(N, s3.size());
	ASSERT_TRUE(std::is_sorted(s3.begin(), s3.end()));
	ASSERT_EQ(std::unique(s3.begin(), s3.end()), s3.end());

	set_t copier(s2);
	ASSERT_EQ(N, copier.size());
	ASSERT_TRUE(std::is_sorted(copier.begin(), copier.end()));
	ASSERT_EQ(std::unique(copier.begin(), copier.end()), copier.end());

	set_t thief(std::move(copier));
	ASSERT_TRUE(copier.empty());
	ASSERT_TRUE(std::is_sorted(thief.begin(), thief.end()));
	ASSERT_EQ(std::unique(thief.begin(), thief.end()), thief.end());

}

TEST(FlatSetTests, AssignmentTests) {
	std::vector<int> integers(N);
	std::generate(integers.begin(), integers.end(), [n = 0]() mutable { return n++; });
	std::shuffle(integers.begin(), integers.end(), gen);

	set_t s1(integers.begin(), integers.end());
	set_t s2 = s1;
	ASSERT_EQ(N, s1.size());
	ASSERT_EQ(s1.size(), s2.size());
	ASSERT_TRUE(std::is_sorted(s2.begin(), s2.end()));
	ASSERT_EQ(std::unique(s2.begin(), s2.end()), s2.end());

	set_t s3 = std::move(s2);
	ASSERT_TRUE(s2.empty());
	ASSERT_EQ(s1.size(), s3.size());
	ASSERT_TRUE(std::is_sorted(s3.begin(), s3.end()));
	ASSERT_EQ(std::unique(s3.begin(), s3.end()), s3.end());

	std::initializer_list<int> list{ 4,7,2,1,6,5,3 };
	s3 = list;
	ASSERT_EQ(list.size(), s3.size());
	ASSERT_TRUE(std::is_sorted(s3.begin(), s3.end()));
	ASSERT_EQ(std::unique(s3.begin(), s3.end()), s3.end());
}

TEST(FlatSetTests, InsertionTests) {
	set_t set;

	for (int i = 0; i < N; ++i) {
		auto ret = set.insert(i);
		ASSERT_EQ(i, *ret.first);
		ASSERT_TRUE(ret.second);
	}
	ASSERT_EQ(N, set.size());
	ASSERT_TRUE(std::is_sorted(set.begin(), set.end()));
	ASSERT_EQ(std::unique(set.begin(), set.end()), set.end());

	set.clear();
	auto it = set.begin();
	for (int i = 0; i < N; ++i) {
		auto ret = set.insert(it, i);
		ASSERT_EQ(i, *ret);
		it = set.end();
	}
	ASSERT_EQ(N, set.size());
	ASSERT_TRUE(std::is_sorted(set.begin(), set.end()));
	ASSERT_EQ(std::unique(set.begin(), set.end()), set.end());

	set.clear();
	it = set.begin();
	for (int i = N; i > 0; --i) {
		auto ret = set.insert(it, i);
		ASSERT_EQ(i, *ret);
		it = set.end();
	}
	ASSERT_EQ(N, set.size());
	ASSERT_TRUE(std::is_sorted(set.begin(), set.end()));
	ASSERT_EQ(std::unique(set.begin(), set.end()), set.end());

	set.clear();
	it = set.begin();
	for (int i = N; i > 0; --i) {
		auto ret = set.insert(it, i);
		ASSERT_EQ(i, *ret);
		it = set.begin();
	}
	ASSERT_EQ(N, set.size());
	ASSERT_TRUE(std::is_sorted(set.begin(), set.end()));
	ASSERT_EQ(std::unique(set.begin(), set.end()), set.end());

	set.clear();
	it = set.begin();
	for (int i = 0; i < N; ++i) {
		it = set.insert(it, i);
		ASSERT_EQ(i, *it);
	}
	ASSERT_EQ(N, set.size());
	ASSERT_TRUE(std::is_sorted(set.begin(), set.end()));
	ASSERT_EQ(std::unique(set.begin(), set.end()), set.end());

	set.clear();
	std::srand(std::time(nullptr));
	std::vector<int> integers(N);
	std::generate(integers.begin(), integers.end(), [n = 0]() mutable { return n++; });
	std::shuffle(integers.begin(), integers.end(), gen);

	for (auto integer : integers) {
		auto ret = set.insert(integer);
		ASSERT_EQ(integer, *ret.first);
		ASSERT_TRUE(ret.second);
	}
	ASSERT_EQ(N, set.size());
	ASSERT_TRUE(std::is_sorted(set.begin(), set.end()));
	ASSERT_EQ(std::unique(set.begin(), set.end()), set.end());

	set.clear();
	it = set.begin();
	for (auto integer : integers) {
		auto ret = set.insert(it, integer);
		ASSERT_EQ(integer, *ret);
		it = set.begin() + std::rand() % set.size();
	}
	ASSERT_EQ(N, set.size());
	ASSERT_TRUE(std::is_sorted(set.begin(), set.end()));
	ASSERT_EQ(std::unique(set.begin(), set.end()), set.end());

	set.clear();
	auto copy = integers;
	std::copy(copy.begin(), copy.end(), std::back_inserter(integers));
	std::shuffle(integers.begin(), integers.end(), gen);

	for (auto integer : integers) {
		auto ret = set.insert(integer);
		ASSERT_EQ(integer, *ret.first);
	}
	ASSERT_EQ(N, set.size());
	ASSERT_TRUE(std::is_sorted(set.begin(), set.end()));
	ASSERT_EQ(std::unique(set.begin(), set.end()), set.end());

	set.clear();
	it = set.begin();
	for (auto integer : integers) {
		auto ret = set.insert(it, integer);
		ASSERT_EQ(integer, *ret);
		it = set.begin() + std::rand() % set.size();
	}
	ASSERT_EQ(N, set.size());
	ASSERT_TRUE(std::is_sorted(set.begin(), set.end()));
	ASSERT_EQ(std::unique(set.begin(), set.end()), set.end());
}

TEST(FlatSetTests, ErasureTests) {
	set_t set;
	std::vector<int> integers;
	for (int i = 0; i < N; ++i) {
		integers.emplace_back(i);
		set.insert(set.end(), i);
	}
	for (auto integer : integers) {
		ASSERT_EQ(1, set.erase(integer));
	}
	ASSERT_TRUE(set.empty());
	std::shuffle(integers.begin(), integers.end(), gen);
	set.insert(integers.begin(), integers.end());
	for (auto integer : integers) {
		auto it = set.find(integer);
		set.erase(it);
		ASSERT_FALSE(set.contains(integer));
	}
}

TEST(FlatSetTests, LookupTests) {
	std::vector<int> integers;
	set_t set;
	for (int i = 1; i <= N; ++i) {
		integers.emplace_back(i);
		set.insert(set.end(), i);
	}
	std::shuffle(integers.begin(), integers.end(), gen);
	for (auto integer : integers) {
		ASSERT_FALSE(set.contains(-integer));
		ASSERT_EQ(0, set.count(-integer));
		ASSERT_TRUE(set.contains(integer));
		ASSERT_EQ(1, set.count(integer));
	}
}

TEST(FlatSetTests, LexicographicalTests) {
	ASSERT_EQ(set_t({ 1, 2, 3, 4 }), set_t({ 1, 2, 3, 4 }));
	ASSERT_LE(set_t({ 1, 2, 3, 4 }), set_t({ 2, 3, 4, 5 }));
}

TEST(FlatSetTests, SwapTest) {
	set_t s1({ 1, 2, 3, 4 });
	set_t s2({ 5, 6, 7, 8 });
	s1.swap(s2);
	ASSERT_EQ(set_t({ 5, 6, 7, 8 }), s1);
	ASSERT_EQ(set_t({ 1, 2, 3, 4 }), s2);
}

TEST(FlatSetTests, TransparentCompareTests) {
	using set_t = ancillary::flat_set<Employee, EmployeeCompare>;
	std::vector<int> ids;
	std::generate_n(std::back_inserter(ids), N, [n = 0]() mutable { return n++; });
	std::vector<Employee> emps;
	std::generate_n(std::back_inserter(emps), N, []() { return Employee{}; });
	set_t set(emps.begin(), emps.end());
	std::shuffle(emps.begin(), emps.end(), gen);
	for (auto emp : emps)
		ASSERT_TRUE(set.contains(emp));
	std::shuffle(ids.begin(), ids.end(), gen);
	for (auto id : ids)
		ASSERT_TRUE(set.contains(id));
}