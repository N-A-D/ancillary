#pragma once

#include <iterator>
#include <type_traits>

namespace ancillary {
	namespace detail {

		template <
			class It,
			class = void
		> struct is_iterator 
			: std::false_type {};

		template <
			class It
		> class is_iterator<It, std::void_t<typename std::iterator_traits<It>::iterator_category>>
			: std::true_type {};

		template <
			class It
		> constexpr bool is_iterator_v = is_iterator<It>::value;

	}
}