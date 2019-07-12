#pragma once

#include <cassert>
#include <iterator>

namespace ancillary {
	
	template <class RndIt, class Compare, class DiffType = typename std::iterator_traits<RndIt>::difference_type>
	constexpr DiffType sift_up_heap(RndIt data, DiffType idx, Compare comp)
	{
		while (idx > 0 && comp(data[(idx - 1) / 2], data[idx])) {
			std::swap(data[(idx - 1) / 2], data[idx]);
			idx = (idx - 1) / 2;
		}
		return idx;
	}

	template <class RndIt, class Compare, class DiffType = typename std::iterator_traits<RndIt>::difference_type>
	constexpr DiffType sift_down_heap(RndIt data, DiffType parent, DiffType len, Compare comp)
	{
		while (2 * parent <= len) {
			auto child = 2 * parent;
			if (child < len && comp(data[child - 1], data[child]))
				++child;
			if (comp(data[parent - 1], data[child - 1])) {
				std::swap(data[parent - 1], data[child - 1]);
				parent = child;
			}
			else
				break;
		}
		return parent;
	}

	template <class RndIt>
	constexpr void make_heap(RndIt first, RndIt last)
	{
		ancillary::make_heap(first, last, std::less<>{});
	}

	template <class RndIt, class Compare>
	constexpr void make_heap(RndIt first, RndIt last, Compare comp)
	{
		assert(first <= last);
		auto len = std::distance(first, last);
		auto mid = len >> 1;
		for (auto i = mid; i != 0; --i)
			ancillary::sift_down_heap(first, i, len, comp);
	}

	template <class RndIt>
	constexpr void push_heap(RndIt first, RndIt last)
	{
		ancillary::push_heap(first, last, std::less<>{});
	}

	template <class RndIt, class Compare>
	constexpr void push_heap(RndIt first, RndIt last, Compare comp)
	{
		assert(first < last);
		auto len = std::distance(first, last);
		ancillary::sift_up_heap(first, len - 1, comp);
	}

	template <class RndIt>
	constexpr void pop_heap(RndIt first, RndIt last)
	{
		ancillary::pop_heap(first, last, std::less<>{});
	}

	template <class RndIt, class Compare>
	constexpr void pop_heap(RndIt first, RndIt last, Compare comp)
	{
		assert(first < last);
		if (first == std::prev(last))
			return;
		std::iter_swap(first, std::prev(last));
		ancillary::make_heap(first, std::prev(last), comp);
	}

	template <class RndIt>
	constexpr void sort_heap(RndIt first, RndIt last)
	{
		ancillary::sort_heap(first, last, std::less<>{});
	}

	template <class RndIt, class Compare>
	constexpr void sort_heap(RndIt first, RndIt last, Compare comp)
	{
		while (first != last)
			ancillary::pop_heap(first, last--, comp);
	}

}