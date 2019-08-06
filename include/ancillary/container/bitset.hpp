#pragma once

#include <array>
#include <string>
#include <climits>
#include <cassert>
#include <istream>
#include <ostream>
#include <type_traits>

namespace ancillary {

	template <
		size_t N
	> class bitset {
		//friend class std::hash<bitset<N>>; // TODO

		using block_type = uint64_t;
		
		static constexpr size_t num_bits = N;
		static constexpr size_t block_size = CHAR_BIT * sizeof(block_type);
		static constexpr size_t num_blocks = (num_bits == 0 ? 0 : (num_bits - 1) / block_size) + 1;
		static constexpr size_t last_block = num_blocks - 1;
		static constexpr block_type zero_block = 0;
		static constexpr block_type ones_block = ~zero_block;

		std::array<block_type, num_blocks> m_blocks;

		// Returns the block that contains the bit located at pos
		static constexpr size_t bitblock(size_t pos) noexcept {
			assert(pos < num_bits);
			return pos / block_size;
		}

		// Returns the bitmask for the bit at position pos
		static constexpr block_type bitmask(size_t pos) noexcept {
			assert(pos < num_bits);
			return block_type(1) << (block_type(pos) % block_type(block_size));
		}

		// Returns the number of set bits in the integer x
		static constexpr size_t popcount(block_type x) noexcept {
			// https://en.wikipedia.org/wiki/Hamming_weight 
			const uint64_t m1 = 0x5555555555555555; //binary: 0101...
			const uint64_t m2 = 0x3333333333333333; //binary: 00110011..
			const uint64_t m4 = 0x0f0f0f0f0f0f0f0f; //binary:  4 zeros,  4 ones ...
			const uint64_t h01 = 0x0101010101010101; //the sum of 256 to the power of 0,1,2,3...

			x -= (x >> 1) & m1;             //put count of each 2 bits into those 2 bits
			x = (x & m2) + ((x >> 2) & m2); //put count of each 4 bits into those 4 bits 
			x = (x + (x >> 4)) & m4;        //put count of each 8 bits into those 8 bits 
			return (x * h01) >> 56;  //returns left 8 bits of x + (x<<8) + (x<<16) + (x<<24) + ... 
		}

		size_t popcount() const noexcept {
			size_t sum = 0;
			for (block_type block : m_blocks)
				sum += popcount(block);
			return sum;
		}

		void trim_last_block() noexcept {
			if constexpr (num_bits == 0 || num_bits % block_size != 0)
				m_blocks[last_block] &= ~(ones_block << (num_bits % block_size));
		}

		// Initializes the elements of *this according to the elements of the given string
		template <class CharT, class Traits, class Alloc>
		void initialize_from_string(const std::basic_string<CharT, Traits, Alloc>& str,
			typename std::basic_string<CharT, Traits, Alloc>::size_type
			pos = 0,
			typename std::basic_string<CharT, Traits, Alloc>::size_type
			n = std::basic_string<CharT, Traits, Alloc>::npos,
			CharT zero = CharT('0'),
			CharT one = CharT('1'))
		{
			// TODO
		}

		// Returns whether or not *this lexicographically compares less than rhs
		bool compare_to(const bitset<N>& rhs) const noexcept {
			for (size_t i = last_block; i > 0; --i)
				if (m_blocks[i] != rhs.m_blocks[i])
					return m_blocks[i] < rhs.m_blocks[i];
			return m_blocks[0] < rhs.m_blocks[0];
		}

		// Returns whether or not *this is equivalent to rhs
		bool equal_to(const bitset<N>& rhs) const noexcept {
			for (size_t i = last_block; i > 0; --i)
				if (m_blocks[i] != rhs.m_blocks[i])
					return false;
			return m_blocks[0] == rhs.m_blocks[0];
		}

		bool is_all_set() const noexcept {
			return count() == size();
		}

		bool is_any_set() const noexcept {
			return count() != 0;
		}

		bitset<N>& and_with(const bitset<N>& rhs) noexcept {
			for (size_t i = 0; i <= last_block; ++i)
				m_blocks[i] &= rhs.m_blocks[i];
			return *this;
		}

		bitset<N>& or_with(const bitset<N>& rhs) noexcept {
			for (size_t i = 0; i <= last_block; ++i)
				m_blocks[i] |= rhs.m_blocks[i];
			return *this;
		}

		bitset<N>& xor_with(const bitset<N>& rhs) noexcept {
			for (size_t i = 0; i <= last_block; ++i)
				m_blocks[i] ^= rhs.m_blocks[i];
			return *this;
		}

		bitset<N>& left_shift_by(size_t lshift) noexcept {
			if (lshift != 0) {
				if (lshift >= num_bits)
					return reset();

				size_t block_shift = lshift / block_size;
				if (block_shift != 0) {
					size_t i = last_block;
					for (; i >= block_shift; --i) // Shift blocks upward
						m_blocks[i] = m_blocks[i - block_shift];
					for (i = 0; i != block_shift; ++i) // Zero out lower order blocks
						m_blocks[i] = zero_block;
				}

				lshift %= block_size;
				if (lshift != 0) {
					size_t rshift = block_size - lshift;
					for (size_t i = last_block; i != 0; --i)
						m_blocks[i] = (m_blocks[i] << lshift) | (m_blocks[i - 1] >> rshift);
					m_blocks[0] <<= lshift;
				}
				trim_last_block();
			}
			return *this;
		}
		
		bitset<N>& right_shift_by(size_t rshift) noexcept {
			if (rshift != 0) {
				if (rshift >= num_bits)
					return reset();

				size_t block_shift = rshift / block_size;
				if (block_shift != 0) { 
					size_t i = 0;
					for (; i != num_blocks - block_shift; ++i) // Shift blocks downward
						m_blocks[i] = m_blocks[i + block_shift];
					for (; i != num_blocks; ++i) // Zero out the higher order blocks
						m_blocks[i] = zero_block;
				}

				rshift %= block_size;
				if (rshift != 0) {
					size_t lshift = block_size - rshift;
					for (size_t i = 0; i != last_block; ++i)
						m_blocks[i] = (m_blocks[i] >> rshift) | (m_blocks[i + 1] << lshift);
					m_blocks[last_block] >>= rshift;
				}
			}
			return *this;
		}

	public:
		
		class reference {
		public:

			~reference() = default;

			constexpr reference& operator=(bool x) noexcept { return set(x); }
			constexpr reference& operator=(const reference& x) noexcept { return set(x); }

			operator bool() const noexcept { return m_block & m_mask; }
			bool operator~() const noexcept { return !(operator bool()); }

			reference& set(bool x) noexcept {
				if (x)
					m_block |= m_mask;
				else
					m_block &= ~m_mask;
				return *this;
			}

			reference& flip() noexcept {
				m_block ^= m_mask;
				return *this;
			}

		private:
			friend class bitset<N>;

			reference(block_type mask, block_type& block) noexcept
				: m_mask(mask)
				, m_block(block) {}

			block_type m_mask;
			block_type& m_block;

		};

		constexpr bitset() noexcept
			: m_blocks{ zero_block } {}

		constexpr bitset(unsigned long long val) noexcept
			: bitset()
		{
			m_blocks[0] = val;
			trim_last_block();
		}

		template <class CharT, class Traits, class Alloc>
		explicit bitset(const std::basic_string<CharT, Traits, Alloc>& str,
			typename std::basic_string<CharT, Traits, Alloc>::size_type
			pos = 0,
			typename std::basic_string<CharT, Traits, Alloc>::size_type
			n = std::basic_string<CharT, Traits, Alloc>::npos,
			CharT zero = CharT('0'),
			CharT one = CharT('1'))
			: bitset()
		{
			initialize_from_string(str, pos, n, zero, one);
		}

		template <class CharT>
		explicit bitset(const CharT* str,
			typename std::basic_string<CharT>::size_type
			n = std::basic_string<CharT>::npos,
			CharT zero = CharT('0'),
			CharT one = CharT('1'))
			: bitset()
		{
			using string_t = std::basic_string<CharT>;
			string_t string = n == string_t::npos ? string_t(str) : string_t(str, n);
			initialize_from_string(string, 0, n, zero, one);
		}

		/* Lexicographical operators */

		bool operator<(const bitset<N>& rhs) const noexcept { return compare_to(rhs); }
		bool operator>(const bitset<N>& rhs) const noexcept { return rhs < *this; }
		bool operator<=(const bitset<N>& rhs) const noexcept { return *this < rhs || *this == rhs; }
		bool operator>=(const bitset<N>& rhs) const noexcept { return *this > rhs || *this == rhs; }

		/* Equality operations */

		bool operator==(const bitset<N>& rhs) const noexcept { return equal_to(rhs); }
		bool operator!=(const bitset<N>& rhs) const noexcept { return !(*this == rhs); }
		
		/* Element access */

		bool test(size_t pos) const noexcept { return m_blocks[bitblock(pos)] & bitmask(pos); }

		bool operator[](size_t pos) const noexcept { return test(pos); }
		reference operator[](size_t pos) noexcept { return reference(bitmask(pos), m_blocks[bitblock(pos)]); }

		bool all() const noexcept { return is_all_set(); }
		bool any() const noexcept { return is_any_set(); }
		bool none() const noexcept { return !any(); }

		size_t count() const noexcept { return popcount(); }

		/* Capacity */

		constexpr size_t size() const noexcept { return num_bits; }

		/* Modifiers */

		bitset<N>& operator&=(const bitset<N>& rhs) noexcept { return and_with(rhs); }
		bitset<N>& operator|=(const bitset<N>& rhs) noexcept { return or_with(rhs); }
		bitset<N>& operator^=(const bitset<N>& rhs) noexcept { return xor_with(rhs); }
		bitset<N> operator~() const noexcept { return bitset<N>(*this).flip(); }

		bitset<N> operator<<(size_t shift) const noexcept { return bitset<N>(*this) <<= shift; }
		bitset<N>& operator<<=(size_t shift) noexcept { return left_shift_by(shift); }
		bitset<N> operator>>(size_t shift) const noexcept { return bitset<N>(*this) >>= shift; }
		bitset<N>& operator>>=(size_t shift) noexcept { return right_shift_by(shift); }

		bitset<N>& set() noexcept {
			m_blocks.fill(ones_block);
			trim_last_block();
			return *this;
		}

		bitset<N>& set(size_t pos, bool value = true) noexcept {
			if (value)
				m_blocks[bitblock(pos)] |= bitmask(pos);
			else
				m_blocks[bitblock(pos)] &= ~bitmask(pos);
			return *this;
		}

		bitset<N>& reset() noexcept {
			m_blocks.fill(zero_block);
			return *this;
		}

		bitset<N>& reset(size_t pos) noexcept {
			return set(pos, false);
		}

		bitset<N>& flip() noexcept {
			for (size_t i = 0; i != num_blocks; ++i)
				m_blocks[i] = ~m_blocks[i];
			trim_last_block();
			return *this;
		}

		bitset<N>& flip(size_t pos) noexcept {
			m_blocks[bitblock(pos)] ^= bitmask(pos);
			return *this;
		}

		template<
			class CharT = char,
			class Traits = std::char_traits<CharT>,
			class Allocator = std::allocator<CharT>
		> std::basic_string<CharT, Traits, Allocator>
			to_string(CharT zero = CharT('0'), CharT one = CharT('1')) const 
		{
			std::basic_string<CharT, Traits, Allocator> ret(num_bits, zero);
			for (size_t i = 0; i != num_bits; ++i)
				if (test(i))
					ret[ret.size() - i - 1] = one;
			return ret;
		}

		unsigned long to_ulong() const {
			unsigned long long value = to_ullong();
			unsigned long ret = static_cast<unsigned long>(value);
			if (ret != value)
				throw std::overflow_error("bitset value cannot fit into unsigned long");
			return ret;
		}

		unsigned long long to_ullong() const {
			if constexpr (num_bits > block_size) {
				for (size_t i = 1; i != num_blocks; ++i)
					if (m_blocks[i] != zero_block)
						throw std::overflow_error("bitset value cannot fit into unsigned long long");
			}
			return m_blocks[0];
		}

	};

	template <size_t N>
	bitset<N> operator&(const bitset<N>& lhs, const bitset<N>& rhs) noexcept { return bitset<N>(lhs) &= rhs; }

	template <size_t N>
	bitset<N> operator|(const bitset<N>& lhs, const bitset<N>& rhs) noexcept { return bitset<N>(lhs) |= rhs; }

	template <size_t N>
	bitset<N> operator^(const bitset<N>& lhs, const bitset<N>& rhs) noexcept { return bitset<N>(lhs) ^= rhs; }

	template <class CharT, class Traits, size_t N>
	std::basic_ostream<CharT, Traits>& operator<<(std::basic_ostream<CharT, Traits>& os, const bitset<N>& x)
	{
		os << x.template to_string<CharT, Traits>();
		return os;
	}

	template <class CharT, class Traits, size_t N>
	std::basic_istream<CharT, Traits>& operator>>(std::basic_istream<CharT, Traits>& is, bitset<N>& x)
	{
		// TODO
		return is;
	}

}