#include <gtest/gtest.h>
#include <random>
#include <vector>
#include <algorithm>
#include "../include/ancillary/container/flat_map.hpp"

using map_t = ancillary::flat_map<int, int>;
using pair_t = std::pair<int, int>;

struct CompareEqual {
	map_t::value_compare comp;
	CompareEqual(map_t::value_compare comp)
		: comp(comp) {}
	bool operator()(const map_t::value_type& lhs, const map_t::value_type& rhs) const {
		return !comp(lhs, rhs) && !comp(rhs, lhs);
	}
};

std::mt19937 gen{ std::random_device{}() };

const std::size_t N = 200;

TEST(FlatMapTests, ConstructorTests) {
	map_t m1;
	ASSERT_TRUE(m1.empty());
	std::vector<pair_t> pairs(N);
	std::generate(pairs.begin(), pairs.end(), [n = 0]() mutable {
		auto value = n++;
		return std::make_pair(value, value);
	});
	std::shuffle(pairs.begin(), pairs.end(), gen);

	map_t m2(pairs.begin(), pairs.end());
	ASSERT_EQ(pairs.size(), m2.size());
	ASSERT_TRUE(std::is_sorted(m2.begin(), m2.end(), m2.value_comp()));
	ASSERT_EQ(std::unique(m2.begin(), m2.end(), CompareEqual(m2.value_comp())), m2.end());

	size_t size = pairs.size();
	auto copy(pairs);
	for (auto it = copy.begin(); it != copy.end(); ++it)
		pairs.emplace_back(*it);
	std::shuffle(pairs.begin(), pairs.end(), gen);

	map_t m3(pairs.begin(), pairs.end());
	ASSERT_EQ(N, m3.size());
	ASSERT_TRUE(std::is_sorted(m3.begin(), m3.end(), m3.value_comp()));
	ASSERT_EQ(std::unique(m3.begin(), m3.end(), CompareEqual(m3.value_comp())), m3.end());

	map_t copier(m3);
	ASSERT_EQ(N, copier.size());
	ASSERT_TRUE(std::is_sorted(copier.begin(), copier.end(), copier.value_comp()));
	ASSERT_EQ(std::unique(copier.begin(), copier.end(), CompareEqual(copier.value_comp())), copier.end());

	map_t thief(std::move(copier));
	ASSERT_TRUE(copier.empty());
	ASSERT_TRUE(std::is_sorted(thief.begin(), thief.end(), thief.value_comp()));
	ASSERT_EQ(std::unique(thief.begin(), thief.end(), CompareEqual(thief.value_comp())), thief.end());
}

TEST(FlatMapTests, AssignmentTests) {
	std::vector<pair_t> pairs(N);
	std::generate(pairs.begin(), pairs.end(), [n = 0]() mutable {
		auto value = n++;
		return std::make_pair(value, value);
	});
	std::shuffle(pairs.begin(), pairs.end(), gen);

	map_t m1(pairs.begin(), pairs.end());
	map_t m2 = m1;
	ASSERT_EQ(N, m1.size());
	ASSERT_EQ(m1.size(), m2.size());
	ASSERT_TRUE(std::is_sorted(m2.begin(), m2.end(), m2.value_comp()));
	ASSERT_EQ(std::unique(m2.begin(), m2.end(), CompareEqual(m2.value_comp())), m2.end());

	map_t m3 = std::move(m2);
	ASSERT_TRUE(m2.empty());
	ASSERT_EQ(m1.size(), m3.size());
	ASSERT_TRUE(std::is_sorted(m3.begin(), m3.end(), m3.value_comp()));
	ASSERT_EQ(std::unique(m3.begin(), m3.end(), CompareEqual(m3.value_comp())), m3.end());

	std::initializer_list<pair_t> list{ {1, 1}, {2, 2}, {3, 3}, {4, 4} };
	m3 = list;
	ASSERT_EQ(list.size(), m3.size());
	ASSERT_TRUE(std::is_sorted(m3.begin(), m3.end(), m3.value_comp()));
	ASSERT_EQ(std::unique(m3.begin(), m3.end(), CompareEqual(m3.value_comp())), m3.end());
}

TEST(FlatMapTests, InsertionTests) {
	map_t map;
	for (int i = 0; i < N; ++i) {
		auto ret = map.insert(map_t::value_type(i, i));
		ASSERT_EQ(i, ret.first->first);
		ASSERT_TRUE(ret.second);
	}
	ASSERT_EQ(N, map.size());
	ASSERT_TRUE(std::is_sorted(map.begin(), map.end(), map.value_comp()));
	ASSERT_EQ(std::unique(map.begin(), map.end(), CompareEqual(map.value_comp())), map.end());

	map.clear();
	auto it = map.begin();
	for (int i = 0; i < N; ++i) {
		auto value = std::make_pair(i, i);
		auto ret = map.insert(it, value);
		ASSERT_EQ(value.first, ret->first);
		it = map.end();
	}
	ASSERT_EQ(N, map.size());
	ASSERT_TRUE(std::is_sorted(map.begin(), map.end()));
	ASSERT_EQ(std::unique(map.begin(), map.end()), map.end());

	map.clear();
	it = map.begin();
	for (int i = N; i > 0; --i) {
		auto value = std::make_pair(i, i);
		auto ret = map.insert(it, value);
		ASSERT_EQ(value.first, ret->first);
		it = map.end();
	}
	ASSERT_EQ(N, map.size());
	ASSERT_TRUE(std::is_sorted(map.begin(), map.end()));
	ASSERT_EQ(std::unique(map.begin(), map.end()), map.end());

	map.clear();
	it = map.begin();
	for (int i = N; i > 0; --i) {
		auto value = std::make_pair(i, i);
		auto ret = map.insert(it, value);
		ASSERT_EQ(value.first, ret->first);
		it = map.begin();
	}
	ASSERT_EQ(N, map.size());
	ASSERT_TRUE(std::is_sorted(map.begin(), map.end()));
	ASSERT_EQ(std::unique(map.begin(), map.end()), map.end());

	map.clear();
	it = map.begin();
	for (int i = 0; i < N; ++i) {
		auto value = std::make_pair(i, i);
		it = map.insert(it, value);
		ASSERT_EQ(value.first, it->first);
	}
	ASSERT_EQ(N, map.size());
	ASSERT_TRUE(std::is_sorted(map.begin(), map.end()));
	ASSERT_EQ(std::unique(map.begin(), map.end()), map.end());

	map.clear();
	std::srand(std::time(nullptr));
	std::vector<int> integers(N);
	std::generate(integers.begin(), integers.end(), [n = 0]() mutable { return n++; });
	std::shuffle(integers.begin(), integers.end(), gen);

	for (auto integer : integers) {
		auto ret = map.insert(std::make_pair(integer, integer));
		ASSERT_EQ(integer, ret.first->first);
		ASSERT_TRUE(ret.second);
	}
	ASSERT_EQ(N, map.size());
	ASSERT_TRUE(std::is_sorted(map.begin(), map.end()));
	ASSERT_EQ(std::unique(map.begin(), map.end()), map.end());

	map.clear();
	it = map.begin();
	for (auto integer : integers) {
		auto value = std::make_pair(integer, integer);
		it = map.insert(it, value);
		ASSERT_EQ(value.first, it->first);
		it = map.begin() + std::rand() % map.size();
	}
	ASSERT_EQ(N, map.size());
	ASSERT_TRUE(std::is_sorted(map.begin(), map.end()));
	ASSERT_EQ(std::unique(map.begin(), map.end()), map.end());

	map.clear();
	auto copy = integers;
	std::copy(copy.begin(), copy.end(), std::back_inserter(integers));
	std::shuffle(integers.begin(), integers.end(), gen);

	for (auto integer : integers) {
		auto ret = map.insert(std::make_pair(integer, integer));
		ASSERT_EQ(integer, ret.first->first);
	}
	ASSERT_EQ(N, map.size());
	ASSERT_TRUE(std::is_sorted(map.begin(), map.end()));
	ASSERT_EQ(std::unique(map.begin(), map.end()), map.end());

	map.clear();
	it = map.begin();
	for (auto integer : integers) {
		auto value = std::make_pair(integer, integer);
		auto ret = map.insert(it, value);
		ASSERT_EQ(value.first, ret->first);
		it = map.begin() + std::rand() % map.size();
	}
	ASSERT_EQ(N, map.size());
	ASSERT_TRUE(std::is_sorted(map.begin(), map.end()));
	ASSERT_EQ(std::unique(map.begin(), map.end()), map.end());
}

TEST(FlatMapTests, ElementAccess) {
	std::vector<int> integers;
	ancillary::flat_map<int, bool> map;
	for (int i = 0; i < N; ++i) {
		map.emplace(std::make_pair(i, true));
		integers.emplace_back(i);
	}
	std::shuffle(integers.begin(), integers.end(), gen);
	for (auto integer : integers) {
		ASSERT_TRUE(map.at(integer));
		ASSERT_TRUE(map[integer]);
	}
}

TEST(FlatMapTests, TryEmplace) {
	std::vector<int> integers(N);
	std::generate(integers.begin(), integers.end(), [n = 0]() mutable { return n++; });
	map_t map;
	for (auto integer : integers) {
		auto ret = map.try_emplace(integer, false);
		ASSERT_EQ(integer, ret.first->first);
		ASSERT_TRUE(ret.second);
		ASSERT_FALSE(map.at(integer));
		ASSERT_FALSE(map[integer]);
	}
	for (auto integer : integers) {
		auto ret = map.try_emplace(integer, true);
		ASSERT_EQ(integer, ret.first->first);
		ASSERT_FALSE(ret.second);
		ASSERT_FALSE(map.at(integer));
		ASSERT_FALSE(map[integer]);
	}

	map.clear();
	std::shuffle(integers.begin(), integers.end(), gen);
	std::srand(std::time(nullptr));
	auto it = map.begin();
	for (auto integer : integers) {
		auto ret = map.try_emplace(it, integer, true);
		ASSERT_EQ(integer, ret->first);
		ASSERT_TRUE(map.at(integer));
		ASSERT_TRUE(map[integer]);
		it = map.begin() + std::rand() % map.size();
	}

	it = map.begin();
	std::shuffle(integers.begin(), integers.end(), gen);
	for (auto integer : integers) {
		auto ret = map.try_emplace(it, integer, false);
		ASSERT_EQ(integer, ret->first);
		ASSERT_TRUE(map.at(integer));
		ASSERT_TRUE(map[integer]);
		it = map.begin() + std::rand() % map.size();
	}
}

TEST(FlatMapTests, InsertOrAssignTests) {
	std::vector<int> integers(N);
	std::generate(integers.begin(), integers.end(), [n = 0]() mutable { return n++; });
	map_t map;
	for (auto integer : integers) {
		auto ret = map.insert_or_assign(integer, false);
		ASSERT_EQ(integer, ret.first->first);
		ASSERT_TRUE(ret.second);
		ASSERT_FALSE(map.at(integer));
		ASSERT_FALSE(map[integer]);
	}
	for (auto integer : integers) {
		auto ret = map.insert_or_assign(integer, true);
		ASSERT_EQ(integer, ret.first->first);
		ASSERT_FALSE(ret.second);
		ASSERT_TRUE(map.at(integer));
		ASSERT_TRUE(map[integer]);
	}
	map.clear();
	std::shuffle(integers.begin(), integers.end(), gen);
	std::srand(std::time(nullptr));

	auto it = map.begin();
	for (auto integer : integers) {
		auto ret = map.insert_or_assign(it, integer, true);
		ASSERT_EQ(integer, ret->first);
		ASSERT_TRUE(map.at(integer));
		ASSERT_TRUE(map[integer]);
		it = map.begin() + std::rand() % map.size();
	}

	it = map.begin();
	std::shuffle(integers.begin(), integers.end(), gen);
	for (auto integer : integers) {
		auto ret = map.insert_or_assign(it, integer, false);
		ASSERT_EQ(integer, ret->first);
		ASSERT_FALSE(map.at(integer));
		ASSERT_FALSE(map[integer]);
		it = map.begin() + std::rand() % map.size();
	}
}

TEST(FlatMapTests, ErasureTests) {
	map_t map;
	std::vector<int> integers;
	for (int i = 0; i < N; ++i) {
		integers.emplace_back(i);
		auto value = map_t::value_type(i, i);
		map.insert(map.end(), value);
	}
	for (auto integer : integers) {
		ASSERT_EQ(1, map.erase(integer));
	}
	ASSERT_TRUE(map.empty());
	for (auto integer : integers) {
		map.insert(map_t::value_type(integer, integer));
	}
	for (auto integer : integers) {
		auto it = map.find(integer);
		map.erase(it);
		ASSERT_FALSE(map.contains(integer));
	}
}

TEST(FlatMapTests, LookupTests) {
	map_t map;
	std::vector<int> integers;
	for (int i = 1; i <= N; ++i) {
		integers.emplace_back(i);
		auto value = map_t::value_type(i, i);
		map.insert(map.end(), value);
	}
	std::shuffle(integers.begin(), integers.end(), gen);
	for (auto integer : integers) {
		ASSERT_FALSE(map.contains(-integer));
		ASSERT_EQ(0, map.count(-integer));
		ASSERT_TRUE(map.contains(integer));
		ASSERT_EQ(1, map.count(integer));
	}
}

TEST(FlatMapTests, LexicographicalTests) {
	ASSERT_EQ(map_t({ {1, 1}, {2, 2}, {3, 3} }), map_t({ {1, 1}, {2, 2}, {3, 3} }));
	ASSERT_LE(map_t({ {1, 1}, {2, 2}, {3, 3} }), map_t({ {2, 3}, {3, 8}, {4, 3} }));
}

TEST(FlatMapTests, SwapTest) {
	map_t m1({ {1, 1}, {2, 2}, {3, 3} });
	map_t m2({ {2, 3}, {3, 8}, {4, 3} });
	m1.swap(m2);
	ASSERT_EQ(m1, map_t({ {2, 3}, {3, 8}, {4, 3} }));
	ASSERT_EQ(m2, map_t({ {1, 1}, {2, 2}, {3, 3} }));
}