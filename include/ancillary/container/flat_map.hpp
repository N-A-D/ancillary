#pragma once

#include <tuple>
#include <stdexcept>
#include "../detail/flat_tree.hpp"
#include "../detail/extract_key.hpp"

namespace ancillary {

	template <
		class Key,
		class T,
		class Compare = std::less<Key>,
		class Allocator = std::allocator<std::pair<Key, T>>
	> struct flat_map 
		: detail::flat_tree<std::pair<Key, T>, Compare, Allocator, detail::select1st<std::pair<Key, T>>, false>
	{
		using tree_type = detail::flat_tree<std::pair<Key, T>, Compare, Allocator, detail::select1st<std::pair<Key, T>>, false>;
		using typename tree_type::key_type;
		using mapped_type = T;
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

		flat_map() = default;

		explicit flat_map(const Compare& comp, const Allocator& alloc = Allocator())
			: tree_type(comp, alloc) {}

		explicit flat_map(const Allocator& alloc)
			: tree_type(alloc) {}

		template <class InIt>
		flat_map(InIt first, InIt last,
			const Compare& comp = Compare(),
			const Allocator& alloc = Allocator())
			: tree_type(first, last, comp, alloc) {}

		template <class InIt>
		flat_map(InIt first, InIt last,
			const Allocator& alloc)
			: tree_type(first, last, alloc) {}

		flat_map(const flat_map&) = default;
		flat_map(const flat_map& other, const Allocator& alloc)
			: tree_type(other, alloc) {}

		flat_map(flat_map&&) = default;
		flat_map(flat_map&& other, const Allocator& alloc)
			: tree_type(std::move(other), alloc) {}

		flat_map(std::initializer_list<value_type> list,
			const Compare& comp = Compare(),
			const Allocator& alloc = Allocator())
			: tree_type(list, comp, alloc) {}

		flat_map(std::initializer_list<value_type> list,
			const Allocator& alloc)
			: tree_type(list, alloc) {}

		~flat_map() = default;

		flat_map& operator=(const flat_map&) = default;
		flat_map& operator=(flat_map&&) = default;
		flat_map& operator=(std::initializer_list<value_type> list) {
			tree_type::operator=(list);
			return *this;
		}

		using tree_type::get_allocator;

		mapped_type& at(const key_type& key) {
			return const_cast<mapped_type&>(const_cast<const flat_map*>(this)->at(key));
		}

		const mapped_type& at(const key_type& key) const {
			auto it = find(key);
			if (it == end())
				throw std::out_of_range("No such element with the given key!");
			else
				return it->second;
		}

		mapped_type& operator[](const key_type& key) {
			auto it = find(key);
			if (it != end())
				return it->second;
			else
				return try_emplace(key).first->second;
		}

		mapped_type& operator[](key_type&& key) {
			auto it = find(key);
			if (it != end())
				return it->second;
			else
				return try_emplace(std::move(key)).first->second;
		}

		template <class M>
		std::pair<iterator, bool> insert_or_assign(const key_type& k, M&& obj) {
			auto it = find(k);
			if (it != end()) {
				it->second = std::forward<M>(obj);
				return { it, false };
			}
			else
				return emplace(k, std::forward<M>(obj));
		}

		template <class M>
		std::pair<iterator, bool> insert_or_assign(key_type&& k, M&& obj) {
			auto it = find(k);
			if (it != end()) {
				it->second = std::forward<M>(obj);
				return { it, false };
			}
			else
				return emplace(std::move(k), std::forward<M>(obj));
		}

		template <class M>
		iterator insert_or_assign(const_iterator hint, const key_type& k, M&& obj) {
			auto it = find(k);
			if (it != end()) {
				it->second = std::forward<M>(obj);
				return it;
			}
			else
				return emplace_hint(hint, k, std::forward<M>(obj));
		}

		template <class M>
		iterator insert_or_assign(const_iterator hint, key_type&& k, M&& obj) {
			auto it = find(k);
			if (it != end()) {
				it->second = std::forward<M>(obj);
				return it;
			}
			else
				return emplace_hint(hint, std::move(k), std::forward<M>(obj));
		}

		template <class... Args>
		std::pair<iterator, bool> try_emplace(const key_type& k, Args&&... args) {
			auto it = find(k);
			if (it != end())
				return { it, false };
			else
				return emplace(std::piecewise_construct,
					std::forward_as_tuple(k),
					std::forward_as_tuple(std::forward<Args>(args)...));
		}

		template <class... Args>
		std::pair<iterator, bool> try_emplace(key_type&& k, Args&&... args) {
			auto it = find(k);
			if (it != end())
				return { it, false };
			else
				return emplace(std::piecewise_construct,
					std::forward_as_tuple(std::move(k)),
					std::forward_as_tuple(std::forward<Args>(args)...));
		}

		template <class... Args>
		iterator try_emplace(const_iterator hint, const key_type& k, Args&&... args) {
			auto it = find(k);
			if (it != end())
				return it;
			else
				return emplace_hint(hint,
					std::piecewise_construct,
					std::forward_as_tuple(k),
					std::forward_as_tuple(std::forward<Args>(args)...));
		}

		template <class... Args>
		iterator try_emplace(const_iterator hint, key_type&& k, Args&&... args) {
			auto it = find(k);
			if (it != end())
				return it;
			else
				return emplace_hint(hint,
					std::piecewise_construct,
					std::forward_as_tuple(std::move(k)),
					std::forward_as_tuple(std::forward<Args>(args)...));
		}

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
		ancillary::flat_map<Key, Compare, Allocator>& lhs,
		ancillary::flat_map<Key, Compare, Allocator>& rhs)
	{
		return lhs.swap(rhs);
	}
}