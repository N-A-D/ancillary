#pragma once

#include <utility>

namespace ancillary {
	namespace detail {

		template <
			class T
		> struct identity {
			using type = T;
			const T& operator()(const T& t) const {
				return t;
			}
		};

		template <
			class Pair
		> struct select1st;

		template <
			class F,
			class S
		> struct select1st<std::pair<F, S>>	{
			using type = F;
			const F& operator()(const std::pair<F, S>& p) const {
				return p.first;
			}
		};

	}
}