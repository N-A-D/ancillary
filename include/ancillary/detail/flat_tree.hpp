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

	}
}