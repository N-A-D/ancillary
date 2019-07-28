#pragma once

#include <vector>
#include <algorithm>
#include <iterator>

namespace ancillary {

	template <class BidirIt, class UnaryPredicate>
	constexpr BidirIt partition(BidirIt first, BidirIt last, UnaryPredicate p) {
		while (first != last) {
			while (p(*first) && ++first != last) {}
			while (last != first && !p(*--last)) {}
			if (first != last)
				std::iter_swap(first++, last);
		}
		return first;
	}

	template <class BidirIt, class UnaryPredicate>
	BidirIt stable_partition(BidirIt first, BidirIt last, UnaryPredicate p) {
		using Value = typename std::iterator_traits<BidirIt>::value_type;
		std::vector<Value> buffer(first, last);
		BidirIt last_true = std::copy_if(buffer.begin(), buffer.end(), first, p);
		auto not_p = [&p](auto v) { return !p(v); };
		std::copy_if(buffer.begin(), buffer.end(), last_true, not_p);
		return last_true;
	}

}