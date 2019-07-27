#pragma once

#include <limits>
#include <memory>
#include <cassert>
#include <iterator>
#include <algorithm>
#include "../detail/is_iterator.hpp"

namespace ancillary {
	namespace detail {

		template <
			class Deque,
			bool IsConst
		> class deque_iterator {
		public:
			using iterator_category = std::random_access_iterator_tag;
			using value_type = typename Deque::value_type;
			using difference_type = typename Deque::difference_type;
			using reference = std::conditional_t
				<
					IsConst,
					typename Deque::const_reference,
				typename Deque::reference
				>;
			using pointer = std::conditional_t
				<
					IsConst,
					typename Deque::const_pointer,
					typename Deque::pointer
				>;

			friend class deque_iterator<Deque, !IsConst>;

			deque_iterator() noexcept = default;

			deque_iterator(const Deque* parent, pointer it)
				: m_parent(parent)
				, m_it(it) {}

			deque_iterator(const deque_iterator<Deque, false>& it)
				: m_parent(it.m_parent)
				, m_it(it.m_it) {}

			deque_iterator& operator=(const deque_iterator& it) {
				if (this != std::addressof(it)) {
					m_parent = it.m_parent;
					m_it = it.m_it;
				}
				return *this;
			}

			template <bool is_const>
			difference_type operator-(const deque_iterator<Deque, is_const>& other) const {
				return logical_address() - other.logical_address();
			}

			reference operator*() const {
				return *(operator->());
			}

			pointer operator->() const {
				assert(valid() && "Invalid iterator!");
				assert(m_it && "Bad iterator dereference!");
				return m_it;
			}

			reference operator[](difference_type n) const {
				return *(*this + n);
			}

			deque_iterator& operator++() {
				assert(valid() && "Invalid iterator!");
				assert(m_it && "Increment out of bounds!");
				m_it = m_parent->next(m_it);
				if (m_it == m_parent->m_tail)
					m_it = nullptr;
				return *this;
			}

			deque_iterator operator++(int) {
				deque_iterator tmp(*this);
				++*this;
				return tmp;
			}

			deque_iterator& operator--() {
				assert(valid() && "Invalid iterator!");
				assert(m_it != m_parent->m_head && "Decrement out of bounds!");
				if (m_it == nullptr)
					m_it = m_parent->m_tail;
				m_it = m_parent->prev(m_it);
				return *this;
			}

			deque_iterator operator--(int) {
				deque_iterator tmp(*this);
				--*this;
				return tmp;
			}

			deque_iterator& operator+=(difference_type n) {
				assert(valid());
				if (n > 0) {
					assert(m_parent->end() - *this && "Increment out of bounds!");
					m_it = m_parent->next(m_it, n);
					if (m_it == m_parent->m_tail)
						m_it = nullptr;
					return *this;
				}
				else if (n < 0) {
					return *this -= -n;
				}
				return *this;
			}

			deque_iterator operator+(difference_type n) {
				return deque_iterator(*this) += n;
			}

			deque_iterator& operator-=(difference_type n) {
				assert(valid());
				if (n > 0) {
					assert(*this - m_parent->begin() >= n && "Decrement out of bounds!");
					if (m_it == nullptr)
						m_it = m_parent->m_tail;
					m_it = m_parent->prev(m_it, n);
					return *this;
				}
				else if (n < 0) {
					return *this += -n;
				}
				return *this;
			}

			deque_iterator operator-(difference_type n) {
				return deque_iterator(*this) -= n;
			}

			template <bool is_const>
			bool operator==(const deque_iterator<Deque, is_const>& it) const {
				assert(valid() && "Invalid calling iterator!");
				assert(compatible(it) && "Incompatible iterators!");
				return m_it == it.m_it;
			}

			template <bool is_const>
			bool operator!=(const deque_iterator<Deque, is_const>& it) const {
				return !(*this == it);
			}

			template <bool is_const>
			bool operator<(const deque_iterator<Deque, is_const>& it) const {
				assert(valid() && "Invalid calling iterator!");
				assert(compatible(it) && "Incompatible iterator!");
				return logical_address() < it.logical_address();
			}

			template <bool is_const>
			bool operator<=(const deque_iterator<Deque, is_const>& it) const {
				return !(it < *this);
			}

			template <bool is_const>
			bool operator>(const deque_iterator<Deque, is_const>& it) const {
				return it < *this;
			}

			template <bool is_const>
			bool operator>=(const deque_iterator<Deque, is_const>& it) const {
				return !(*this < it);
			}

		private:

			const Deque* m_parent = nullptr;
			pointer m_it = nullptr;

			bool valid() const noexcept {
				return m_parent != nullptr;
			}

			template <bool is_const>
			bool compatible(const deque_iterator<Deque, is_const>& other) const noexcept {
				return m_parent == other.m_parent;
			}

			pointer logical_address() const noexcept {
				return m_parent->logical_address(m_it);
			}

		};

	}

	template <
		class T,
		class Allocator = std::allocator<T>
	> class deque {
		using alloc_traits = std::allocator_traits<Allocator>;
	public:

		using value_type = T;
		using allocator_type = Allocator;
		using size_type = std::size_t;
		using difference_type = std::ptrdiff_t;
		using reference = value_type & ;
		using const_reference = const value_type&;
		using pointer = typename alloc_traits::pointer;
		using const_pointer = typename alloc_traits::const_pointer;
		using iterator = detail::deque_iterator<deque<T, Allocator>, false>;
		using const_iterator = detail::deque_iterator<deque<T, Allocator>, true>;
		using reverse_iterator = std::reverse_iterator<iterator>;
		using const_reverse_iterator = std::reverse_iterator<const_iterator>;

		friend class iterator;
		friend class const_iterator;

		////////////////////////////////////////////////////////////////////////////////
		//                               Constructors                                 //
		////////////////////////////////////////////////////////////////////////////////

		deque() : deque(Allocator()) {}

		explicit deque(const Allocator& alloc)
			: m_alloc(alloc)
		{
			initialize();
		}

		explicit deque(size_type n, const Allocator& alloc = Allocator())
			: m_alloc(alloc)
		{
			if (n > 0)
				initialize_default_construct_n(n);
		}

		deque(size_type n, const value_type& v, const Allocator& alloc = Allocator())
			: m_alloc(alloc)
		{
			if (n > 0)
				initialize_fill_n(n, v);
		}

		template <class InIt, class = std::enable_if_t<detail::is_iterator_v<InIt>>>
		deque(InIt first, InIt last, const Allocator& alloc = Allocator())
			: m_alloc(alloc)
		{
			if (std::distance(first, last) > 0)
				initialize_copy(first, last);
		}

		deque(const deque& other)
			: m_alloc(alloc_traits::select_on_container_copy_construction(other.get_allocator()))
		{
			if (!other.empty())
				initialize_copy(other.begin(), other.end());
		}

		deque(const deque& other, const Allocator& alloc)
			: m_alloc(alloc)
		{
			if (!other.empty())
				initialize_copy(other.begin(), other.end());
		}

		deque(deque&& other)
		{
			steal_members(std::move(other));
		}

		deque(deque&& other, const Allocator& alloc) noexcept(alloc_traits::is_always_equal::value)
			: m_alloc(alloc)
		{
			if (alloc_traits::is_always_equal::value || m_alloc == other.get_allocator())
				steal_resources(std::move(other));
			else {
				if (!other.empty())
					initialize_move(other.begin(), other.end());
			}
		}

		deque(std::initializer_list<value_type> list, const Allocator& alloc = Allocator())
			: deque(list.begin(), list.end(), alloc) {}


		////////////////////////////////////////////////////////////////////////////////
		//                                Destructor                                  //
		////////////////////////////////////////////////////////////////////////////////

		~deque()
		{
			clear_and_deallocate();
		}

		////////////////////////////////////////////////////////////////////////////////
		//                                Assignment                                  //
		////////////////////////////////////////////////////////////////////////////////

		deque& operator=(const deque& other) {
			if (this != &other) {
				if (alloc_traits::propagate_on_container_copy_assignment::value) {
					if (m_alloc != other.m_alloc)
						clear_and_deallocate();
					m_alloc = other.m_alloc;
				}
				assign(other.begin(), other.end());
			}
			return *this;
		}

		deque& operator=(deque&& other)
			noexcept(alloc_traits::propagate_on_container_move_assignment::value || alloc_traits::is_always_equal::value)
		{
			if (this != &other) {
				if (alloc_traits::propagate_on_container_move_assignment::value) {
					clear_and_deallocate();
					steal_members(std::move(other));
				}
				else if (alloc_traits::is_always_equal::value || m_alloc == other.m_alloc) {
					clear_and_deallocate();
					steal_resources(std::move(other));
				}
				else
					assign(std::move_iterator(other.begin()), std::move_iterator(other.end()));
			}
			return *this;
		}

		deque& operator=(std::initializer_list<value_type> list) {
			assign(list);
			return *this;
		}

		void assign(size_type n, const value_type& value) {
			clear();
			for (size_type i = 0; i != n; ++i)
				push_back(value);
		}

		template <class InIt, class = std::enable_if_t<detail::is_iterator_v<InIt>>>
		void assign(InIt first, InIt last) {
			clear();
			for (auto it = first; it != last; ++it)
				push_back(*it);
		}

		void assign(std::initializer_list<value_type> list) {
			assign(list.begin(), list.end());
		}

		allocator_type get_allocator() const noexcept { return m_alloc; }

		////////////////////////////////////////////////////////////////////////////////
		//                              Element access                                //
		////////////////////////////////////////////////////////////////////////////////

		reference at(size_type index) {
			return const_cast<reference>(const_cast<const deque*>(this)->at(index));
		}

		const_reference at(size_type index) const {
			assert(!empty());
			if (!valid_index(index))
				throw std::out_of_range("Index out of range!");
			return (*this)[index];
		}

		reference operator[](size_type index) {
			return const_cast<reference>((*const_cast<const deque*>(this))[index]);
		}

		const_reference operator[](size_type index) const {
			assert(!empty());
			assert(valid_index(index));
			return *next(m_head, index);
		}

		reference front() {
			return const_cast<reference>(const_cast<const deque*>(this)->front());
		}

		const_reference front() const {
			assert(!empty());
			return *m_head;
		}

		reference back() {
			return const_cast<reference>(const_cast<const deque*>(this)->back());
		}

		const_reference back() const {
			assert(!empty());
			return *prev(m_tail);
		}

		////////////////////////////////////////////////////////////////////////////////
		//                                Iterators                                   //
		////////////////////////////////////////////////////////////////////////////////

		iterator begin() noexcept { return iterator(this, m_head); }
		const_iterator begin() const noexcept { return const_iterator(this, m_head); }
		const_iterator cbegin() const noexcept { return const_iterator(this, m_head); }

		iterator end() noexcept { return iterator(this, nullptr); }
		const_iterator end() const noexcept { return const_iterator(this, nullptr); }
		const_iterator cend() const noexcept { return const_iterator(this, nullptr); }

		reverse_iterator rbegin() noexcept { return reverse_iterator(end()); }
		const_reverse_iterator rbegin() const noexcept { return const_reverse_iterator(cend()); }
		const_reverse_iterator crbegin() const noexcept { return const_reverse_iterator(cend()); }

		reverse_iterator rend() noexcept { return reverse_iterator(begin()); }
		const_reverse_iterator rend() const noexcept { return const_reverse_iterator(cbegin()); }
		const_reverse_iterator crend() const noexcept { return const_reverse_iterator(cbegin()); }

		////////////////////////////////////////////////////////////////////////////////
		//                                 Capacity                                   //
		////////////////////////////////////////////////////////////////////////////////

		bool empty() const noexcept { return size() == 0; }
		size_type size() const noexcept { return m_size; }
		size_type max_size() const noexcept {
			return std::min<size_type>(alloc_traits::max_size(m_alloc), std::numeric_limits<difference_type>::max());
		}
		size_type capacity() const noexcept { return m_limit - m_data; }

		////////////////////////////////////////////////////////////////////////////////
		//                                Modifiers                                   //
		////////////////////////////////////////////////////////////////////////////////

		void clear() noexcept { while (!empty()) pop_back(); }

		iterator insert(const_iterator pos, const value_type& value) { return emplace(pos, value); }
		iterator insert(const_iterator pos, value_type&& value) { return emplace(pos, std::move(value)); }
		iterator insert(const_iterator pos, size_type n, const value_type& value) {
			assert(iterator_in_range(pos) && "Iterator out of range!");
			difference_type off = pos - cbegin();
			if (n == 0)
				return begin() + off;
			iterator it = begin() + off;
			for (size_type i = 0; i != n; ++i)
				it = emplace(it, value);
			return it;
		}

		template <class BidirIt, class = std::enable_if_t<detail::is_iterator_v<BidirIt>>>
		iterator insert(const_iterator pos, BidirIt first, BidirIt last) {
			assert(iterator_in_range(pos) && "Iterator out of range!");
			difference_type off = pos - cbegin();
			if (first == last)
				return begin() + off;
			iterator it = begin() + off;
			while (last != first)
				it = insert(it, *--last);
			return it;
		}

		iterator insert(const_iterator pos, std::initializer_list<value_type> list) {
			return insert(pos, list.begin(), list.end());
		}

		template <class... Args>
		iterator emplace(const_iterator pos, Args&&... args) {
			assert(iterator_in_range(pos) && "Iterator out of range!");
			if (pos == cbegin()) {
				append_front(std::forward<Args>(args)...);
				return begin();
			}
			if (pos == cend()) {
				append_back(std::forward<Args>(args)...);
				return std::prev(end());
			}
			difference_type from_front = pos - cbegin();
			difference_type from_back = cend() - pos;
			if (from_front < from_back) {
				append_front(std::forward<Args>(args)...);
				iterator it = end() - from_back;
				std::rotate(begin(), begin() + 1, it);
				return std::prev(it);
			}
			else {
				append_back(std::forward<Args>(args)...);
				iterator it = begin() + from_front;
				std::rotate(it, std::prev(end()), end());
				return it;
			}
		}

		iterator erase(const_iterator pos) {
			assert(iterator_in_range(pos) && "Iterator out of range!");
			assert(pos != cend() && "Iterator must be dereferenceable!");
			if (pos == cbegin()) {
				remove_front();
				return begin();
			}
			if (pos == std::prev(cend())) {
				remove_back();
				return end();
			}
			difference_type from_front = pos - cbegin();
			difference_type from_back = cend() - pos;
			if (from_front < from_back) {
				iterator it = end() - from_back;
				std::rotate(begin(), it, std::next(it));
				remove_front();
				return std::next(it);
			}
			else {
				iterator it = begin() + from_front;
				std::rotate(it, it + 1, end());
				remove_back();
				return it;
			}
		}

		iterator erase(const_iterator first, const_iterator last) {
			assert(iterator_in_range(first) && "Iterator out of range!");
			assert(iterator_in_range(last) && "Iterator out of range!");
			assert(first != cend() && "Iterator must be dereferenceable!");
			assert(first <= last && "Invalid range!");
			difference_type off_first = first - cbegin();
			size_type dist = last - first;
			if (first == last)
				return begin() + off_first;
			iterator it = begin() + off_first;
			for (size_type i = 0; i != dist; ++i)
				it = erase(it);
			return it;
		}

		void push_front(const T& value) { append_front(value); }
		void push_front(T&& value) { append_front(std::move(value)); }

		void push_back(const T& value) { append_back(value); }
		void push_back(T&& value) { append_back(std::move(value)); }

		void pop_front() { remove_front(); }
		void pop_back() { remove_back(); }

		template <class... Args>
		void emplace_front(Args&&... args) {
			append_front(std::forward<Args>(args)...);
		}

		template <class... Args>
		void emplace_back(Args&&... args) {
			append_back(std::forward<Args>(args)...);
		}

		void resize(size_type n) {
			if (m_size < n) {
				while (m_size != n)
					emplace_back();
			}
			else if (m_size > n) {
				while (m_size != n)
					pop_back();
			}
		}

		void resize(size_type n, const value_type value) {
			if (m_size < n) {
				while (m_size != n)
					emplace_back(value);
			}
			else if (m_size > n) {
				while (m_size != n)
					pop_back();
			}
		}

		void swap(deque& other)
			noexcept(alloc_traits::propagate_on_container_swap::value || alloc_traits::is_always_equal::value)
		{
			if (this != std::addressof(other)) {
				if (alloc_traits::propagate_on_container_swap::value) {
					swap_members(other);
				}
				else if (m_alloc == other.m_alloc) {
					swap_resources(other);
				}
			}
		}

	private:

		pointer m_data; // Beginning of storage
		pointer m_limit; // One past the end of allocated storage
		pointer m_head; // Logical beginning of the container
		pointer m_tail; // Logical ending of the container (one past)
		size_type m_size; // # of elements in the container
		allocator_type m_alloc; // Memory management handle

		////////////////////////////////////////////////////////////////////////////////
		//                              Memory management                             //
		////////////////////////////////////////////////////////////////////////////////

		pointer allocate(size_type cap) {
			return alloc_traits::allocate(m_alloc, cap);
		}

		void deallocate() {
			if (m_data != nullptr) {
				alloc_traits::deallocate(m_alloc, m_data, capacity());
				initialize(); // Reinitialize to empty container
			}
		}

		template <class... Args>
		void construct(pointer p, Args&&... args) {
			alloc_traits::construct(m_alloc, p, std::forward<Args>(args)...);
		}

		void destroy(pointer p) {
			alloc_traits::destroy(m_alloc, p);
		}

		void migrate(pointer dest) {
			if constexpr (std::is_nothrow_move_constructible_v<T>) {
				if (m_tail > m_head)
					std::uninitialized_move(m_head, m_tail, dest);
				else {
					pointer tmp = std::uninitialized_move(m_head, m_limit, dest);
					std::uninitialized_move(m_data, m_tail, tmp);
				}
			}
			else {
				if (m_tail > m_head)
					std::uninitialized_copy(m_head, m_tail, dest);
				else {
					pointer tmp = std::uninitialized_copy(m_head, m_limit, dest);
					std::uninitialized_copy(m_data, m_tail, tmp);
				}
			}
			clear_and_deallocate();
		}

		void expand() {
			static_assert(std::is_nothrow_move_constructible_v<T> || std::is_copy_constructible_v<T>);
			assert(full());
			size_type new_size = m_size;
			size_type new_cap = std::max(2 * capacity(), size_type(1));
			pointer new_data = allocate(new_cap);
			migrate(new_data);
			m_size = new_size;
			m_data = m_head = new_data;
			m_tail = m_head + m_size;
			m_limit = m_data + new_cap;
			assert(!full());
		}

		////////////////////////////////////////////////////////////////////////////////
		//                              Initialization                                //
		////////////////////////////////////////////////////////////////////////////////

		void initialize() {
			m_size = 0;
			m_data = m_limit = m_head = m_tail = nullptr;
		}

		void initialize_default_construct_n(size_type n) {
			static_assert(std::is_default_constructible_v<T>);
			m_size = n;
			size_type cap = generate_size_bigger_than_or_equal(m_size);
			if (m_size == cap) {
				m_data = allocate(m_size);
				m_limit = m_data + m_size;
				std::uninitialized_default_construct(m_data, m_limit);
				m_head = m_tail = m_data;
			}
			else {
				m_data = allocate(cap);
				m_limit = m_data + cap;
				std::uninitialized_default_construct_n(m_data, m_size);
				m_head = m_data;
				m_tail = m_data + m_size;
			}
		}

		void initialize_fill_n(size_type n, const value_type& value) {
			static_assert(std::is_copy_constructible_v<T>);
			m_size = n;
			size_type cap = generate_size_bigger_than_or_equal(m_size);
			if (m_size == cap) {
				m_data = allocate(m_size);
				m_limit = m_data + m_size;
				std::uninitialized_fill(m_data, m_limit, value);
				m_head = m_tail = m_data;
			}
			else {
				m_data = allocate(cap);
				m_limit = m_data + cap;
				std::uninitialized_fill_n(m_data, m_size, value);
				m_head = m_data;
				m_tail = m_data + m_size;
			}
		}

		template <class InIt, class = std::enable_if_t<detail::is_iterator_v<InIt>>>
		void initialize_copy(InIt first, InIt last) {
			static_assert(std::is_convertible_v<typename std::iterator_traits<InIt>::value_type, value_type>);
			static_assert(std::is_copy_constructible_v<T>);
			m_size = std::distance(first, last);
			size_type cap = generate_size_bigger_than_or_equal(m_size);
			if (m_size == cap) {
				m_data = allocate(m_size);
				m_limit = std::uninitialized_copy(first, last, m_data);
				m_head = m_tail = m_data;
			}
			else {
				m_data = allocate(cap);
				m_limit = m_data + cap;
				std::uninitialized_copy_n(first, m_size, m_data);
				m_head = m_data;
				m_tail = m_data + m_size;
			}
		}

		template <class InIt, class = std::enable_if_t<detail::is_iterator_v<InIt>>>
		void initialize_move(InIt first, InIt last) {
			static_assert(std::is_convertible_v<typename std::iterator_traits<InIt>::value_type, value_type>);
			static_assert(std::is_move_constructible_v<T>);
			m_size = std::distance(first, last);
			size_type cap = generate_size_bigger_than_or_equal(m_size);
			if (m_size == cap) {
				m_data = allocate(m_size);
				m_limit = std::uninitialized_move(first, last, m_data);
				m_head = m_tail = m_data;
			}
			else {
				m_data = allocate(cap);
				m_limit = m_data + cap;
				std::uninitialized_move_n(first, m_size, m_data);
				m_head = m_data;
				m_tail = m_data + m_size;
			}
		}

		////////////////////////////////////////////////////////////////////////////////
		//                           Element creation                                 //
		////////////////////////////////////////////////////////////////////////////////

		template <class... Args>
		void append_front(Args&&... args) {
			if (full())
				expand();
			m_head = prev(m_head);
			construct(m_head, std::forward<Args>(args)...);
			++m_size;
		}

		void remove_front() {
			assert(!empty());
			destroy(m_head);
			m_head = next(m_head);
			--m_size;
		}

		template <class... Args>
		void append_back(Args&&... args) {
			if (full())
				expand();
			construct(m_tail, std::forward<Args>(args)...);
			m_tail = next(m_tail);
			++m_size;
		}

		void remove_back() {
			assert(!empty());
			m_tail = prev(m_tail);
			destroy(m_tail);
			--m_size;
		}

		////////////////////////////////////////////////////////////////////////////////
		//                                Utilities                                   //
		////////////////////////////////////////////////////////////////////////////////

		bool valid_index(size_type index) const noexcept {
			return index < m_size;
		}

		bool iterator_in_range(const_iterator pos) const {
			return cbegin() <= pos && pos <= cend();
		}

		bool full() const noexcept { return capacity() == size(); }

		size_type generate_size_bigger_than_or_equal(size_type target) const noexcept {
			size_type sz = 1;
			while (sz <= target)
				sz <<= 2;
			return sz;
		}

		void clear_and_deallocate() {
			clear();
			deallocate();
		}

		void steal_resources(deque&& other) {
			m_data = other.m_data;
			m_limit = other.m_limit;
			m_head = other.m_head;
			m_tail = other.m_tail;
			m_size = other.m_size;
			other.initialize();
		}

		void steal_members(deque&& other) {
			m_alloc = std::move(other.m_alloc);
			steal_resources(std::move(other));
		}

		void swap_resources(deque& other) {
			std::swap(m_data, other.m_data);
			std::swap(m_limit, other.m_limit);
			std::swap(m_head, other.m_head);
			std::swap(m_tail, other.m_tail);
			std::swap(m_size, other.m_size);
		}

		void swap_members(deque& other) {
			std::swap(m_alloc, other.m_alloc);
			swap_resources(other);
		}

		template <class Pointer>
		Pointer next(Pointer p, size_type n = 1) const noexcept {
			size_type pos(p - m_data);
			assert(capacity() > 0);
			size_type mask = capacity() - 1;
			pos += n;
			pos &= mask;
			return m_data + pos;
		}

		template <class Pointer>
		Pointer prev(Pointer p, size_type n = 1) const noexcept {
			size_type pos(p - m_data);
			assert(capacity() > 0);
			size_type mask = capacity() - 1;
			pos -= n;
			pos &= mask;
			return m_data + pos;
		}

		template <class Pointer>
		Pointer logical_address(Pointer p) const noexcept {
			return p != nullptr ? prev(p, m_head - m_data) : m_data + m_size;
		}

	};

	template <class T, class Allocator>
	bool operator==(const deque<T, Allocator>& lhs, const deque<T, Allocator>& rhs) {
		return std::equal(lhs.begin(), lhs.end(), rhs.begin(), rhs.end());
	}

	template <class T, class Allocator>
	bool operator!=(const deque<T, Allocator>& lhs, const deque<T, Allocator>& rhs) {
		return !(lhs == rhs);
	}

	template <class T, class Allocator>
	bool operator<(const deque<T, Allocator>& lhs, const deque<T, Allocator>& rhs) {
		return std::lexicographical_compare(lhs.begin(), lhs.end(), rhs.begin(), rhs.end());
	}

	template <class T, class Allocator>
	bool operator<=(const deque<T, Allocator>& lhs, const deque<T, Allocator>& rhs) {
		return !(rhs < lhs);
	}

	template <class T, class Allocator>
	bool operator>(const deque<T, Allocator>& lhs, const deque<T, Allocator>& rhs) {
		return rhs < lhs;
	}

	template <class T, class Allocator>
	bool operator>=(const deque<T, Allocator>& lhs, const deque<T, Allocator>& rhs) {
		return !(lhs < rhs);
	}

}

namespace std {

	template <class T, class Allocator>
	void swap(ancillary::deque<T, Allocator>& lhs, ancillary::deque<T, Allocator>& rhs)
		noexcept(noexcept(lhs.swap(rhs)))
	{
		lhs.swap(rhs);
	}

}