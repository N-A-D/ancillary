#pragma once

#include <vector>
#include <cassert>
#include <iterator>

namespace ancillary {
	namespace detail {

		template <
			class Compare,
			class Key,
			class = void
		> struct is_transparent
			: std::false_type {};
		
		template <
			class Compare,
			class Key
		> struct is_transparent<Compare, Key, std::void_t<typename Compare::is_transparent>>
			: std::true_type {};

		template <
			class Compare,
			class Key
		> constexpr bool is_transparent_v = is_transparent<Compare, Key>::value;

		template <
			class Value,
			class Compare,
			class ExtractKey
		> class FTValueCompare {
			Compare m_cmp;
			ExtractKey m_ext;
		public:
			FTValueCompare(Compare c)
				: m_cmp(c) {}
			bool operator()(const Value& lhs, const Value& rhs) const {
				return m_cmp(m_ext(lhs), m_ext(rhs));
			}
		};

		template <
			class Key, 
			class Compare
		> class FTCompareEqual {
			Compare m_cmp;
		public:
			FTCompareEqual(Compare c)
				: m_cmp(c) {}
			bool operator()(const Key& lhs, const Key& rhs) const {
				return !m_cmp(lhs, rhs) && !m_cmp(rhs, lhs);
			}
			template <class K, class = std::enable_if_t<is_transparent_v<Compare, K>>>
			bool operator()(const K& lhs, const Key& rhs) const {
				return !m_cmp(lhs, rhs) && !m_cmp(rhs, lhs);
			}
			template <class K, class = std::enable_if_t<is_transparent_v<Compare, K>>>
			bool operator()(const Key& lhs, const K& rhs) const {
				return !m_cmp(lhs, rhs) && !m_cmp(rhs, lhs);
			}
		};

		template <
			class Value,
			class Compare,
			class Allocator,
			class ExtractKey,
			bool isMulti
		> class flat_tree {
		public:

			using container_type         = std::vector<Value, Allocator>;
			using key_type               = typename ExtractKey::type;
			using value_type             = typename container_type::value_type;
			using key_compare            = Compare;
			using value_compare          = FTValueCompare<Value, Compare, ExtractKey>;
			using key_equal              = FTCompareEqual<key_type, Compare>;
			using key_extract            = ExtractKey;
			using size_type              = typename container_type::size_type;
			using difference_type        = typename container_type::difference_type;
			using allocator_type         = typename container_type::allocator_type;
			using reference              = typename container_type::reference;
			using const_reference        = typename container_type::const_reference;
			using pointer                = typename container_type::pointer;
			using const_pointer          = typename container_type::const_pointer;
			using iterator               = typename container_type::iterator;
			using const_iterator         = typename container_type::const_iterator;
			using reverse_iterator       = typename container_type::reverse_iterator;
			using const_reverse_iterator = typename container_type::const_reverse_iterator;

			using emplace_ret_type = std::conditional_t
										<
											isMulti, 
											iterator, 
											std::pair<iterator, bool>
										>;

			////////////////////////////////////////////////////////////////////////////////////
			//                                  CONSTRUCTORS                                  //
			////////////////////////////////////////////////////////////////////////////////////

			flat_tree()
				: flat_tree(Compare(), Allocator()) {}

			explicit flat_tree(const Compare& comp, const Allocator& alloc = Allocator())
				: m_data(alloc)
				, m_kcmp(comp)
				, m_vcmp(comp)
				, m_keq(comp)
				, m_kext() {}

			explicit flat_tree(const Allocator& alloc)
				: flat_tree(Compare(), alloc) {}

			template <class InIt>
			flat_tree(InIt first, InIt last,
				const Compare& comp = Compare(),
				const Allocator& alloc = Allocator())
				: flat_tree(comp, alloc)
			{
				insert(first, last);
			}

			template <class InIt>
			flat_tree(InIt first, InIt last,
				const Allocator& alloc)
				: flat_tree(first, last, Compare, alloc) {}

			flat_tree(const flat_tree&) = default;
			flat_tree(const flat_tree& other, const Allocator& alloc)
				: m_data(other.m_data, alloc)
				, m_kcmp(other.m_kcmp)
				, m_vcmp(other.m_vcmp)
				, m_keq(other.m_keq)
				, m_kext(other.m_kext) {}

			flat_tree(flat_tree&&) = default;
			flat_tree(flat_tree&& other, const Allocator& alloc)
				: m_data(std::move(other.m_data), alloc)
				, m_kcmp(std::move(other.m_kcmp))
				, m_vcmp(std::move(other.m_vcmp))
				, m_keq(std::move(other.m_keq))
				, m_kext(std::move(other.m_kext)) {}

			flat_tree(std::initializer_list<value_type> list,
				const Compare& comp = Compare(),
				const Allocator& alloc = Allocator())
				: flat_tree(list.begin(), list.end(), comp, alloc) {}

			flat_tree(std::initializer_list<value_type> list,
				const Allocator& alloc)
				: flat_tree(list.begin(), list.end(), Compare(), alloc) {}

			////////////////////////////////////////////////////////////////////////////////////
			//                                   DESTRUCTOR                                   //
			////////////////////////////////////////////////////////////////////////////////////

			~flat_tree() = default;

			////////////////////////////////////////////////////////////////////////////////////
			//                                   ASSIGNMENT                                   //
			////////////////////////////////////////////////////////////////////////////////////

			flat_tree& operator=(const flat_tree&) = default;
			flat_tree& operator=(flat_tree&&) = default;
			flat_tree& operator=(std::initializer_list<value_type> list) {
				clear();
				insert(list);
				return *this;
			}

			allocator_type get_allocator() const noexcept { return m_data.get_allocator(); }

			////////////////////////////////////////////////////////////////////////////////////
			//                                    ITERATORS                                   //
			////////////////////////////////////////////////////////////////////////////////////

			iterator begin() noexcept { return m_data.begin(); }
			const_iterator begin() const noexcept { return m_data.begin(); }
			const_iterator cbegin() const noexcept { return m_data.cbegin(); }

			iterator end() noexcept { return m_data.end(); }
			const_iterator end() const noexcept { return m_data.end(); }
			const_iterator cend() const noexcept { return m_data.cend(); }

			reverse_iterator rbegin() noexcept { return m_data.rbegin(); }
			const_reverse_iterator rbegin() const noexcept { return m_data.rbegin(); }
			const_reverse_iterator crbegin() const noexcept { return m_data.crbegin(); }

			reverse_iterator rend() noexcept { return m_data.rend(); }
			const_reverse_iterator rend() const noexcept { return m_data.rend(); }
			const_reverse_iterator crend() const noexcept { return m_data.crend(); }

			////////////////////////////////////////////////////////////////////////////////////
			//                                    CAPACITY                                    //
			////////////////////////////////////////////////////////////////////////////////////

			bool empty() const noexcept { return m_data.empty(); }
			size_type size() const noexcept { return m_data.size(); }
			size_type max_size() const noexcept { return m_data.max_size(); }
			size_type capacity() const noexcept { return m_data.capacity(); }
			void reserve(size_type new_cap) { m_data.reserve(new_cap); }
			void shrink_to_fit() { m_data.shrink_to_fit(); }

			////////////////////////////////////////////////////////////////////////////////////
			//                                    MODIFIERS                                   //
			////////////////////////////////////////////////////////////////////////////////////

			void clear() noexcept { m_data.clear(); }

			emplace_ret_type insert(const value_type& x) { return emplace(x); }
			emplace_ret_type insert(value_type&& x) { return emplace(std::move(x)); }

			iterator insert(const_iterator hint, const value_type& x) { return emplace_hint(hint, x); }
			iterator insert(const_iterator hint, value_type&& x) { return emplace_hint(hint, std::move(x)); }

			template <class InIt>
			void insert(InIt first, InIt last) {
				for (auto it = first; it != last; ++it)
					insert(cend(), *it);
			}

			void insert(std::initializer_list<value_type> list) {
				insert(list.begin(), list.end());
			}

			template <class... Args>
			emplace_ret_type emplace(Args&&... args) {
				if constexpr (isMulti) {
					return emplace_multi(std::forward<Args>(args)...);
				}
				else {
					return emplace_unique(std::forward<Args>(args)...);
				}
			}

			template <class... Args>
			iterator emplace_hint(const_iterator hint, Args&&... args) {
				if constexpr (isMulti) {
					return emplace_hint_multi(hint, std::forward<Args>(args)...);
				}
				else {
					return emplace_hint_unique(hint, std::forward<Args>(args)...);
				}
			}

			iterator erase(iterator pos) { return m_data.erase(pos); }
			iterator erase(const_iterator pos) { return m_data.erase(pos); }
			iterator erase(const_iterator first, const_iterator last) {
				return m_data.erase(first, last);
			}
			size_type erase(const key_type& key) {
				auto range = equal_range(key);
				auto count = std::distance(range.first, range.second);
				if (count > 0)
					erase(range.first, range.second);
				return count;
			}

			void swap(flat_tree& other) {
				if (this != &other) {
					std::swap(m_kcmp, other.m_kcmp);
					std::swap(m_vcmp, other.m_vcmp);
					std::swap(m_keq, other.m_keq);
					std::swap(m_kext, other.m_kext);
					std::swap(m_data, other.m_data);
				}
			}

			////////////////////////////////////////////////////////////////////////////////////
			//                                     LOOKUP                                     //
			////////////////////////////////////////////////////////////////////////////////////

			size_type count(const key_type& key) const {
				auto range = equal_range(key);
				return std::distance(range.first, range.second);
			}

			template <class K, class = std::enable_if_t<is_transparent_v<Compare, K>>>
			size_type count(const K& key) const {
				auto range = equal_range(key);
				return std::distance(range.first, range.second);
			}

			iterator find(const key_type& key) {
				auto lower = lower_bound(key);
				return lower != end() && m_keq(m_kext(*lower), key) ? lower : end();
			}

			const_iterator find(const key_type& key) const {
				auto lower = lower_bound(key);
				return lower != end() && m_keq(m_kext(*lower), key) ? lower : end();
			}

			template <class K, class = std::enable_if_t<is_transparent_v<Compare, K>>>
			iterator find(const K& key) {
				auto lower = lower_bound(key);
				return lower != end() && m_keq(m_kext(*lower), key) ? lower : end();
			}

			template <class K, class = std::enable_if_t<is_transparent_v<Compare, K>>>
			const_iterator find(const K& key) const {
				auto lower = lower_bound(key);
				return lower != end() && m_keq(m_kext(*lower), key) ? lower : end();
			}

			bool contains(const key_type& key) const {
				return find(key) != end();
			}

			template <class K, class = std::enable_if_t<is_transparent_v<Compare, K>>>
			bool contains(const K& key) const {
				return find(key) != end();
			}

			std::pair<iterator, iterator> equal_range(const key_type& key) {
				return { lower_bound(key), upper_bound(key) };
			}

			std::pair<const_iterator, const_iterator> equal_range(const key_type& key) const {
				return { lower_bound(key), upper_bound(key) };
			}

			template <class K, class = std::enable_if_t<is_transparent_v<Compare, K>>>
			std::pair<iterator, iterator> equal_range(const K& key) {
				return { lower_bound(key), upper_bound(key) };
			}

			template <class K, class = std::enable_if_t<is_transparent_v<Compare, K>>>
			std::pair<const_iterator, const_iterator> equal_range(const K& key) const {
				return { lower_bound(key), upper_bound(key) };
			}

			iterator lower_bound(const key_type& key) { 
				return lower_bound_impl(begin(), end(), key); 
			}

			const_iterator lower_bound(const key_type& key) const { 
				return lower_bound_impl(begin(), end(), key); 
			}

			template <class K, class = std::enable_if_t<is_transparent_v<Compare, K>>>
			iterator lower_bound(const K& key) {
				return lower_bound_impl(begin(), end(), key);
			}

			template <class K, class = std::enable_if_t<is_transparent_v<Compare, K>>>
			const_iterator lower_bound(const K& key) const {
				return lower_bound_impl(begin(), end(), key);
			}

			iterator upper_bound(const key_type& key) {
				return upper_bound_impl(begin(), end(), key);
			}

			const_iterator upper_bound(const key_type& key) const {
				return upper_bound_impl(begin(), end(), key);
			}

			template <class K, class = std::enable_if_t<is_transparent_v<Compare, K>>>
			iterator upper_bound(const K& key) {
				return upper_bound_impl(begin(), end(), key);
			}

			template <class K, class = std::enable_if_t<is_transparent_v<Compare, K>>>
			const_iterator upper_bound(const K& key) const {
				return upper_bound_impl(begin(), end(), key);
			}

			////////////////////////////////////////////////////////////////////////////////////
			//                                    OBSERVERS                                   //
			////////////////////////////////////////////////////////////////////////////////////

			key_compare key_comp() const { return m_kcmp; }
			value_compare value_comp() const { return m_vcmp; }

		private:

			bool iterator_in_range(const_iterator it) const {
				return cbegin() <= it && it <= cend();
			}

			template <class RndIt, class Key>
			RndIt lower_bound_impl(RndIt first, RndIt last, const Key& key) const {
				typename std::iterator_traits<RndIt>::difference_type len, step;
				len = last - first;
				RndIt mid;
				while (len > 0) {
					mid = first;
					step = len / 2;
					mid += step;
					if (m_kcmp(m_kext(*mid), key)) {
						first = ++mid;
						len -= step + 1;
					}
					else
						len = step;
				}
				return first;
			}

			template <class RndIt, class Key>
			RndIt upper_bound_impl(RndIt first, RndIt last, const Key& key) const {
				typename std::iterator_traits<RndIt>::difference_type len, step;
				len = last - first;
				RndIt mid;
				while (len > 0) {
					mid = first;
					step = len / 2;
					mid += step;
					if (!m_kcmp(key, m_kext(*mid))) {
						first = ++mid;
						len -= step + 1;
					}
					else
						len = step;
				}
				return first;
			}

			template <class... Args>
			emplace_ret_type emplace_unique(Args&&... args) {
				m_data.emplace_back(std::forward<Args>(args)...);
				auto last = std::prev(end());
				auto lower = lower_bound_impl(begin(), last, m_kext(*last));
				if (lower == last)
					return { lower, true };
				else if (m_keq(m_kext(*lower), m_kext(*last))) {
					m_data.pop_back();
					return { lower, false };
				}
				else {
					std::rotate(lower, last, end());
					return { lower, true };
				}
			}

			template <class... Args>
			iterator emplace_hint_unique(const_iterator hint, Args&&... args) {
				assert(iterator_in_range(hint) && "Iterator out of range!");
				difference_type step = hint - cbegin();
				m_data.emplace_back(std::forward<Args>(args)...);
				iterator pos = begin() + step;
				iterator last = std::prev(end());
				if (pos == last || m_vcmp(*last, *pos)) {
					if (pos == begin() || m_vcmp(*std::prev(pos), *last)) {
						std::rotate(pos, last, end());
						return pos;
					}
					iterator prev = std::prev(pos);
					if (m_keq(m_kext(*prev), m_kext(*last))) {
						m_data.pop_back();
						return prev;
					}
					else {
						auto lower = lower_bound_impl(begin(), prev, m_kext(*last));
						if (m_keq(m_kext(*lower), m_kext(*last))) {
							m_data.pop_back();
							return lower;
						}
						else {
							std::rotate(lower, last, end());
							return lower;
						}
					}
				}
				else {
					auto lower = lower_bound_impl(pos, last, m_kext(*last));
					if (lower == last)
						return lower;
					else if (m_keq(m_kext(*lower), m_kext(*last))) {
						m_data.pop_back();
						return lower;
					}
					else {
						std::rotate(lower, last, end());
						return lower;
					}
				}
			}

			template <class... Args>
			iterator emplace_multi(Args&&... args) {
				m_data.emplace_back(std::forward<Args>(args)...);
				iterator last = std::prev(end());
				auto upper = upper_bound_impl(begin(), last, m_kext(*last));
				std::rotate(upper, last, end());
				return upper;
			}

			template <class... Args>
			iterator emplace_hint_multi(const_iterator hint, Args&&... args) {
				assert(iterator_in_range(hint) && "Iterator out of range!");
				difference_type step = hint - cbegin();
				m_data.emplace_back(std::forward<Args>(args)...);
				iterator pos = begin() + step;
				iterator last = std::prev(end());
				if (pos == last || m_vcmp(*last, *pos)) {
					if (pos == begin() ||
						m_vcmp(*std::prev(pos), *last) ||
						m_keq(m_kext(*std::prev(pos)), m_kext(*last)))
					{
						std::rotate(pos, last, end());
						return pos;
					}
					else {
						auto upper = upper_bound_impl(begin(), std::prev(pos), m_kext(*last));
						std::rotate(upper, last, end());
						return upper;
					}
				}
				else {
					auto upper = upper_bound_impl(pos, last, m_kext(*last));
					std::rotate(upper, last, end());
					return upper;
				}
			}

			key_compare    m_kcmp; // Key comparison
			value_compare  m_vcmp; // Value comparison
			key_extract    m_kext; // Key extraction
			key_equal      m_keq;  // Key equal
			container_type m_data; // Data store

		};

		template <class Val, class Comp, class Alloc, class ExtKey, bool isMulti>
		bool operator==(
			const flat_tree<Val, Comp, Alloc, ExtKey, isMulti>& lhs,
			const flat_tree<Val, Comp, Alloc, ExtKey, isMulti>& rhs)
		{
			auto equal = [comp = Comp(), ext_key = ExtKey()](const auto& lhs, const auto& rhs) {
				return !comp(ext_key(lhs), ext_key(rhs)) && !comp(ext_key(rhs), ext_key(lhs));
			};
			return std::equal(lhs.begin(), lhs.end(), rhs.begin(), rhs.end(), equal);
		}

		template <class Val, class Comp, class Alloc, class ExtKey, bool isMulti>
		bool operator!=(
			const flat_tree<Val, Comp, Alloc, ExtKey, isMulti>& lhs,
			const flat_tree<Val, Comp, Alloc, ExtKey, isMulti>& rhs)
		{
			return !(lhs == rhs);
		}

		template <class Val, class Comp, class Alloc, class ExtKey, bool isMulti>
		bool operator<(
			const flat_tree<Val, Comp, Alloc, ExtKey, isMulti>& lhs,
			const flat_tree<Val, Comp, Alloc, ExtKey, isMulti>& rhs)
		{
			auto comp = [comp = Comp(), ext_key = ExtKey()](const auto& lhs, const auto& rhs) {
				return comp(ext_key(lhs), ext_key(rhs));
			};
			return std::lexicographical_compare(lhs.begin(), lhs.end(), rhs.begin(), rhs.end(), comp);
		}

		template <class Val, class Comp, class Alloc, class ExtKey, bool isMulti>
		bool operator<=(
			const flat_tree<Val, Comp, Alloc, ExtKey, isMulti>& lhs,
			const flat_tree<Val, Comp, Alloc, ExtKey, isMulti>& rhs)
		{
			return !(rhs < lhs);
		}

		template <class Val, class Comp, class Alloc, class ExtKey, bool isMulti>
		bool operator>(
			const flat_tree<Val, Comp, Alloc, ExtKey, isMulti>& lhs,
			const flat_tree<Val, Comp, Alloc, ExtKey, isMulti>& rhs)
		{
			return rhs < lhs;
		}

		template <class Val, class Comp, class Alloc, class ExtKey, bool isMulti>
		bool operator>=(
			const flat_tree<Val, Comp, Alloc, ExtKey, isMulti>& lhs,
			const flat_tree<Val, Comp, Alloc, ExtKey, isMulti>& rhs)
		{
			return !(lhs < rhs);
		}
	}
}