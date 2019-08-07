#include <gtest/gtest.h>
#include <bitset>
#include <random>
#include <limits>
#include <sstream>
#include <algorithm>
#include <set>
#include "../../include/ancillary/container/bitset.hpp"

template <size_t N>
bool is_equal(const ancillary::bitset<N>& bitset, const std::bitset<N>& invariant) {
	for (size_t i = N - 1; i != 0; --i)
		if (bitset[i] != invariant[i])
			return false;
	return bitset[0] == invariant[0];
}

std::mt19937 gen(std::random_device{}());

TEST(BitsetTests, ConstructorTests) {
	// Empty constructor
	ancillary::bitset<8> b1;
	std::bitset<8> b1_invariant;
	ASSERT_TRUE(is_equal(b1, b1_invariant));

	// unsigned long long constructor
	ancillary::bitset<8> b2(42);
	std::bitset<8> b2_invariant(42);
	ASSERT_TRUE(is_equal(b2, b2_invariant));
	
	ancillary::bitset<70> b3(ULLONG_MAX);
	std::bitset<70> b3_invariant(ULLONG_MAX);
	ASSERT_TRUE(is_equal(b3, b3_invariant));
	
	ancillary::bitset<8> b4(0xFFF0);
	std::bitset<8> b4_invariant(0xFFF0);
	ASSERT_TRUE(is_equal(b4, b4_invariant));

	// string constructor
	std::string bit_string = "110010";
	ancillary::bitset<8> b5(bit_string);
	std::bitset<8> b5_invariant(bit_string);
	ASSERT_TRUE(is_equal(b5, b5_invariant));
	
	ancillary::bitset<8> b6(bit_string, 2);
	std::bitset<8> b6_invariant(bit_string, 2);
	ASSERT_TRUE(is_equal(b6, b6_invariant));
	
	ancillary::bitset<8> b7(bit_string, 2, 3);
	std::bitset<8> b7_invariant(bit_string, 2, 3);
	ASSERT_TRUE(is_equal(b7, b7_invariant));
	
	// string constructor using zero/one digits other than '1' and '0'
	std::string custom_bit_string = "xyxxyyxy";
	ancillary::bitset<8> b8(custom_bit_string, 0, custom_bit_string.size(), 'x', 'y');
	std::bitset<8> b8_invariant(custom_bit_string, 0, custom_bit_string.size(), 'x', 'y');
	ASSERT_TRUE(is_equal(b8, b8_invariant));
	
	ancillary::bitset<16> b9("xxxyyxyy", 6, 'x', 'y');
	std::bitset<16> b9_invariant("xxxyyxyy", 6, 'x', 'y');
	ASSERT_TRUE(is_equal(b9, b9_invariant));

}

TEST(BitsetTests, ComparisonAndEqualityTests) {
	std::uniform_int_distribution<uint64_t> dis(0, 255);
	for (size_t i = 1; i != 256; ++i) {
		uint64_t left = dis(gen);
		uint64_t right = dis(gen);
		if (left < right)
			ASSERT_LT(ancillary::bitset<8>(left), ancillary::bitset<8>(right));
		else if (left > right)
			ASSERT_GT(ancillary::bitset<8>(left), ancillary::bitset<8>(right));
		else {
			ASSERT_EQ(ancillary::bitset<8>(left), ancillary::bitset<8>(right));
			ASSERT_LE(ancillary::bitset<8>(left), ancillary::bitset<8>(right));
			ASSERT_GE(ancillary::bitset<8>(left), ancillary::bitset<8>(right));
		}
	}
}

TEST(BitsetTests, ElementAccessTests) {
	for (size_t i = 0; i != 256; ++i) {
		ancillary::bitset<8> bitset(i);
		std::bitset<8> bitset_invariant(i);
		ASSERT_EQ(bitset.count(), bitset_invariant.count());
		ASSERT_TRUE(is_equal(bitset, bitset_invariant));
		if (bitset_invariant.all())
			ASSERT_TRUE(bitset.all());
		else if (bitset_invariant.any())
			ASSERT_TRUE(bitset.any());
		else
			ASSERT_TRUE(bitset.none());
	}
	ancillary::bitset<8> b1;
	std::bitset<8> b1_invariant;
	for (size_t i = 0; i != 8; ++i) {
		b1[i] = true;
		b1_invariant[i] = true;
		ASSERT_TRUE(is_equal(b1, b1_invariant));
	}
	for (size_t i = 0; i != 8; ++i) {
		b1[i].flip();
		b1_invariant[i].flip();
		ASSERT_TRUE(is_equal(b1, b1_invariant));
	}
}

TEST(BitsetTests, BinaryOperatorTests) {
	std::uniform_int_distribution<uint64_t> dis(1, std::numeric_limits<uint64_t>::max() - 1);
	for (size_t i = 0; i != 256; ++i) {
		uint64_t lhs_value = dis(gen);
		uint64_t rhs_value = dis(gen);
		std::bitset<64> lhs_bitset_invariant(lhs_value);
		std::bitset<64> rhs_bitset_invariant(rhs_value);
		ancillary::bitset<64> lhs_bitset(lhs_value);
		ancillary::bitset<64> rhs_bitset(rhs_value);
		lhs_bitset &= rhs_bitset;
		lhs_bitset_invariant &= rhs_bitset_invariant;
		ASSERT_TRUE(is_equal(lhs_bitset, lhs_bitset_invariant));
	}
	for (size_t i = 0; i != 256; ++i) {
		uint64_t lhs_value = dis(gen);
		uint64_t rhs_value = dis(gen);
		std::bitset<64> lhs_bitset_invariant(lhs_value);
		std::bitset<64> rhs_bitset_invariant(rhs_value);
		ancillary::bitset<64> lhs_bitset(lhs_value);
		ancillary::bitset<64> rhs_bitset(rhs_value);
		lhs_bitset |= rhs_bitset;
		lhs_bitset_invariant |= rhs_bitset_invariant;
		ASSERT_TRUE(is_equal(lhs_bitset, lhs_bitset_invariant));
	}
	for (size_t i = 0; i != 256; ++i) {
		uint64_t lhs_value = dis(gen);
		uint64_t rhs_value = dis(gen);
		std::bitset<64> lhs_bitset_invariant(lhs_value);
		std::bitset<64> rhs_bitset_invariant(rhs_value);
		ancillary::bitset<64> lhs_bitset(lhs_value);
		ancillary::bitset<64> rhs_bitset(rhs_value);
		lhs_bitset ^= rhs_bitset;
		lhs_bitset_invariant ^= rhs_bitset_invariant;
		ASSERT_TRUE(is_equal(lhs_bitset, lhs_bitset_invariant));
	}
}

TEST(BitsetTests, ShiftOperatorTests) {
	std::uniform_int_distribution<uint64_t> shift_dis(0, 512);
	std::uniform_int_distribution<uint64_t> value_dis(1, std::numeric_limits<uint64_t>::max() - 1);
	using bitset_t = ancillary::bitset<256>;
	using bitset_invariant_t = std::bitset<256>;
	for (size_t i = 0; i != 256; ++i) {
		uint64_t shift = shift_dis(gen);
		uint64_t value = value_dis(gen);
		bitset_t lshift_bitset(value);
		bitset_invariant_t lshift_bitset_invariant(value);
		ASSERT_TRUE(is_equal(lshift_bitset, lshift_bitset_invariant));
		lshift_bitset <<= shift;
		lshift_bitset_invariant <<= shift;
		ASSERT_TRUE(is_equal(lshift_bitset, lshift_bitset_invariant));

		bitset_t rshift_bitset(value);
		bitset_invariant_t rshift_bitset_invariant(value);
		ASSERT_TRUE(is_equal(rshift_bitset, rshift_bitset_invariant));
		rshift_bitset >>= shift;
		rshift_bitset_invariant >>= shift;
		ASSERT_TRUE(is_equal(rshift_bitset, rshift_bitset_invariant));
	}
}

TEST(BitsetTests, IndividualBitManipulationTests) {
	ancillary::bitset<8> b1;
	std::bitset<8> b1_invariant;
	b1.set();
	b1_invariant.set();
	ASSERT_EQ(b1.count(), b1_invariant.count());
	ASSERT_TRUE(is_equal(b1, b1_invariant));
	b1.reset();
	b1_invariant.reset();
	ASSERT_EQ(b1.count(), b1_invariant.count());
	ASSERT_TRUE(is_equal(b1, b1_invariant));

	b1.set();
	b1_invariant.set();
	for (size_t i = 0; i != 8; ++i) {
		b1.reset(i);
		b1_invariant.reset(i);
		ASSERT_EQ(b1.count(), b1_invariant.count());
		ASSERT_TRUE(is_equal(b1, b1_invariant));
	}
	ASSERT_EQ(b1.count(), 0);

	for (size_t i = 0; i != 8; ++i) {
		b1.set(i);
		b1_invariant.set(i);
		ASSERT_EQ(b1.count(), b1_invariant.count());
		ASSERT_TRUE(is_equal(b1, b1_invariant));
	}
	ASSERT_EQ(b1.count(), 8);

	std::uniform_int_distribution<uint64_t> dis(0, std::numeric_limits<uint64_t>::max() - 1);
	for (size_t i = 0; i != 256; ++i) {
		uint64_t value = dis(gen);
		ancillary::bitset<64> b2(value);
		std::bitset<64> b2_invariant(value);
		ASSERT_EQ(b2.count(), b2_invariant.count());
		ASSERT_TRUE(is_equal(b2, b2_invariant));
		b2.flip();
		b2_invariant.flip();
		ASSERT_EQ(b2.count(), b2_invariant.count());
		ASSERT_TRUE(is_equal(b2, b2_invariant));
	}
	for (size_t i = 0; i != 256; ++i) {
		uint64_t value = dis(gen);
		ancillary::bitset<64> b3(value);
		std::bitset<64> b3_invariant(value);

		for (size_t i = 0; i != 64; ++i) {
			b3.flip(i);
			b3_invariant.flip(i);
			ASSERT_EQ(b3.count(), b3_invariant.count());
			ASSERT_TRUE(is_equal(b3, b3_invariant));
		}
	}
}

TEST(BitsetTests, ToStringTests) {
	std::uniform_int_distribution<uint64_t> dis(1, std::numeric_limits<uint64_t>::max() - 1);
	for (size_t i = 0; i != 256; ++i) {
		uint64_t value = dis(gen);
		ancillary::bitset<64> b1(value);
		std::bitset<64> b1_invariant(value);
		ASSERT_EQ(b1.to_string(), b1_invariant.to_string());
	}
}

TEST(BitsetTests, ToUlongTests) {
	std::uniform_int_distribution<uint32_t> dis(1, std::numeric_limits<uint32_t>::max() - 1);
	for (size_t i = 0; i != 256; ++i) {
		uint32_t value = dis(gen);
		ancillary::bitset<32> b1(value);
		std::bitset<32> b1_invariant(value);
		ASSERT_EQ(b1.to_ulong(), b1_invariant.to_ulong());
	}
}

TEST(BitsetTests, ToUllongTests) {
	std::uniform_int_distribution<uint64_t> dis(1, std::numeric_limits<uint64_t>::max() - 1);
	for (size_t i = 0; i != 256; ++i) {
		uint64_t value = dis(gen);
		ancillary::bitset<64> b1(value);
		std::bitset<64> b1_invariant(value);
		ASSERT_EQ(b1.to_ullong(), b1_invariant.to_ullong());
	}
}

TEST(BitsetTests, OutputOperatorTests) {
	std::uniform_int_distribution<uint64_t> dis(1, std::numeric_limits<uint64_t>::max() - 1);
	for (size_t i = 0; i != 256; ++i) {
		uint64_t value = dis(gen);
		ancillary::bitset<64> b1(value);
		std::bitset<64> b1_invariant(value);
		ASSERT_EQ(b1.count(), b1_invariant.count());
		ASSERT_TRUE(is_equal(b1, b1_invariant));
		std::stringstream b1_ss;
		std::stringstream b1_invariant_ss;
		b1_ss << b1;
		b1_invariant_ss << b1_invariant;
		ASSERT_EQ(b1_ss.str(), b1_invariant_ss.str());
	}
}

TEST(BitsetTests, InputStreamOperatorTests) {
	std::string bit_string = "00001111";
	std::shuffle(bit_string.begin(), bit_string.end(), gen);
	for (size_t i = 0; i != 256; ++i) {
		std::istringstream b1_stream(bit_string);
		std::istringstream b1_invariant_stream(bit_string);
		ancillary::bitset<8> b1;
		b1_stream >> b1;
		std::bitset<8> b1_invariant;
		b1_invariant_stream >> b1_invariant;
		ASSERT_EQ(b1.count(), b1_invariant.count());
		ASSERT_TRUE(is_equal(b1, b1_invariant));
		std::shuffle(bit_string.begin(), bit_string.end(), gen);
	}
}

TEST(BitsetTests, OrderedCollectionTests) {
	std::uniform_int_distribution<uint64_t> dis(1, std::numeric_limits<uint64_t>::max() - 1);
	using bitset_t = ancillary::bitset<64>;
	std::set<bitset_t> set;
	for (size_t i = 0; i != 256; ++i) {
		set.emplace_hint(set.end(), bitset_t{ dis(gen) });
	}
	ASSERT_TRUE(std::is_sorted(set.begin(), set.end()));
}