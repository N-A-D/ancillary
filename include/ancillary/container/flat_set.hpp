#pragma once

#include "../detail/flat_tree.hpp"
#include "../detail/extract_key.hpp"

namespace ancillary {

	template <
		class Key,
		class Compare = std::less<Key>,
		class Allocator = std::allocator<Key>
	> struct flat_set : detail::flat_tree<Key, Compare, Allocator, detail::identity<Key>, false>
	{
		using tree_type = detail::flat_tree<Key, Compare, Allocator, detail::identity<Key>, false>;
		using typename tree_type::key_type;
		using typename tree_type::value_type;
		using typename tree_type::key_compare;
		using typename tree_type::value_compare;
		using typename tree_type::size_type;
		using typename tree_type::difference_type;
		using typename tree_type::allocator_type;
		using typename tree_type::reference;
		using typename tree_type::const_reference;
		using typename tree_type::pointer;
		using typename tree_type::const_pointer;
		using typename tree_type::iterator;
		using typename tree_type::const_iterator;
		using typename tree_type::reverse_iterator;
		using typename tree_type::const_reverse_iterator;

		flat_set() = default;

		explicit flat_set(const Compare& comp, const Allocator& alloc = Allocator())
			: tree_type(comp, alloc) {}

		explicit flat_set(const Allocator& alloc)
			: tree_type(alloc) {}

		template <class InIt>
		flat_set(InIt first, InIt last,
			const Compare& comp = Compare(),
			const Allocator& alloc = Allocator())
			: tree_type(first, last, comp, alloc) {}

		template <class InIt>
		flat_set(InIt first, InIt last,
			const Allocator& alloc)
			: tree_type(first, last, alloc) {}

		flat_set(const flat_set&) = default;
		flat_set(const flat_set& other, const Allocator& alloc)
			: tree_type(other, alloc) {}

		flat_set(flat_set&&) = default;
		flat_set(flat_set&& other, const Allocator& alloc)
			: tree_type(std::move(other), alloc) {}

		flat_set(std::initializer_list<value_type> list,
			const Compare& comp = Compare(),
			const Allocator& alloc = Allocator())
			: tree_type(list, comp, alloc) {}

		flat_set(std::initializer_list<value_type> list,
			const Allocator& alloc)
			: tree_type(list, alloc) {}

		~flat_set() = default;

		flat_set& operator=(const flat_set&) = default;
		flat_set& operator=(flat_set&&) = default;
		flat_set& operator=(std::initializer_list<value_type> list) {
			tree_type::operator=(list);
			return *this;
		}

		using tree_type::get_allocator;

		using tree_type::begin;
		using tree_type::cbegin;
		using tree_type::end;
		using tree_type::cend;

		using tree_type::rbegin;
		using tree_type::crbegin;
		using tree_type::rend;
		using tree_type::crend;

		using tree_type::empty;
		using tree_type::size;
		using tree_type::max_size;
		using tree_type::capacity;
		using tree_type::reserve;
		using tree_type::shrink_to_fit;

		using tree_type::clear;
		using tree_type::insert;
		using tree_type::emplace;
		using tree_type::emplace_hint;
		using tree_type::erase;
		using tree_type::swap;

		using tree_type::count;
		using tree_type::find;
		using tree_type::contains;
		using tree_type::equal_range;
		using tree_type::lower_bound;
		using tree_type::upper_bound;

		using tree_type::key_comp;
		using tree_type::value_comp;

	};

}

namespace std {
	template <class Key, class Compare, class Allocator>
	void swap(
		ancillary::flat_set<Key, Compare, Allocator>& lhs,
		ancillary::flat_set<Key, Compare, Allocator>& rhs)
	{
		return lhs.swap(rhs);
	}
}