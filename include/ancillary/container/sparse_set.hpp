#pragma once

#include <vector>
#include <cassert>
#include <type_traits>

namespace ancillary {
	namespace detail {

		template <
			class SparseSet
		> class sparse_set_iterator {
		public:
			
			using iterator_category = std::bidirectional_iterator_tag;
			using value_type        = typename SparseSet::value_type;
			using difference_type   = typename SparseSet::difference_type;
			using reference         = typename SparseSet::const_reference;
			using pointer           = typename SparseSet::const_pointer;

			sparse_set_iterator() = default;

			sparse_set_iterator(const SparseSet* set, pointer it)
				: m_set(set)
				, m_it(it) {}

			sparse_set_iterator& operator=(const sparse_set_iterator& other) {
				if (this != &other) {
					m_set = other.m_set;
					m_it = other.m_it;
				}
				return *this;
			}

			reference operator*() const {
				assert(valid() && "Invalid iterator!");
				return *m_it;
			}

			pointer operator->() const {
				return &(operator*());
			}

			sparse_set_iterator& operator++() {
				assert(valid() && "Invalid iterator!");
				++m_it;
				assert(m_it <= m_set->data() + m_set->m_size && "Increment out of bounds!");
				return *this;
			}

			sparse_set_iterator operator++(int) {
				sparse_set_iterator tmp(*this);
				++*this;
				return tmp;
			}

			sparse_set_iterator& operator--() {
				assert(valid() && "Invalid iterator!");
				--m_it;
				assert(m_it >= m_set->data() && "Decrement out of bounds!");
				return *this;
			}

			sparse_set_iterator operator--(int) {
				sparse_set_iterator tmp(*this);
				--*this;
				return tmp;
			}

			bool operator==(const sparse_set_iterator& other) const {
				assert(valid() && "Invalid iterator!");
				assert(compatible(other) && "Incompatible iterators!");
				return m_it == other.m_it;
			}

			bool operator!=(const sparse_set_iterator& other) const {
				return !(*this == other);
			}

		private:

			bool valid() const {
				return m_set != nullptr && m_it != nullptr;
			}

			bool compatible(const sparse_set_iterator& other) const {
				return m_set == other.m_set;
			}

			const SparseSet* m_set = nullptr;
			pointer m_it = nullptr;
		};

	}

	template <
		class T,
		class Allocator = std::allocator<T>
	> class sparse_set {
	public:
		static_assert(std::is_unsigned_v<T>);

		using container_type         = std::vector<T, Allocator>;
		using value_type             = typename container_type::value_type;
		using size_type              = typename container_type::size_type;
		using difference_type        = typename container_type::difference_type;
		using allocator_type         = typename container_type::allocator_type;
		using const_reference        = typename container_type::const_reference;
		using const_pointer          = typename container_type::const_pointer;
		using const_iterator         = detail::sparse_set_iterator<sparse_set<T>>;
		using const_reverse_iterator = std::reverse_iterator<const_iterator>;

		friend class const_iterator;

		sparse_set()
			: m_size(0)
			, m_dense()
			, m_sparse() {}

		sparse_set(const sparse_set&) = default;

		sparse_set(sparse_set&& other)
			: m_size(other.m_size)
			, m_dense(std::move(other.m_dense))
			, m_sparse(std::move(other.m_sparse))
		{
			other.m_size = 0;
		}

		template <class InIt>
		sparse_set(InIt first, InIt last)
			: sparse_set()
		{
			insert(first, last);
		}

		sparse_set(std::initializer_list<value_type> list)
			: sparse_set(list.begin(), list.end()) {}

		~sparse_set() = default;

		sparse_set& operator=(const sparse_set&) = default;

		sparse_set& operator=(sparse_set&& other) {
			if (this != &other) {
				m_size = other.m_size;
				m_dense = std::move(other.m_dense);
				m_sparse = std::move(other.m_dense);
				other.m_size = 0;
			}
			return *this;
		}
		
		sparse_set& operator=(std::initializer_list<value_type> list) {
			clear();
			insert(list);
			return *this;
		}

		const_pointer data() const { return m_dense.data(); }

		const_iterator begin() const noexcept { return const_iterator(this, data()); }
		const_iterator cbegin() const noexcept { return const_iterator(this, data()); }

		const_iterator end() const noexcept { return const_iterator(this, data() + m_size); }
		const_iterator cend() const noexcept { return const_iterator(this, data() + m_size); }

		const_reverse_iterator rbegin() const noexcept { return const_reverse_iterator(cend()); }
		const_reverse_iterator crbegin() const noexcept { return const_reverse_iterator(cend()); }

		const_reverse_iterator rend() const noexcept { return const_reverse_iterator(crbegin()); }
		const_reverse_iterator crend() const noexcept { return const_reverse_iterator(crbegin()); }

		bool empty() const noexcept { return size() == 0; }
		size_type size() const noexcept { return m_size; }

		void clear() noexcept { m_size = 0; }

		void insert(value_type v) {
			if (contains(v))
				return;
			if (m_sparse.size() <= v)
				m_sparse.resize(v + 1, 0);
			if (m_dense.size() <= m_size)
				m_dense.resize(m_size + 1, 0);
			m_sparse[v] = m_size;
			m_dense[m_size] = v;
			++m_size;
		}

		template <class InIt>
		void insert(InIt first, InIt last) {
			for (auto it = first; it != last; ++it)
				insert(*it);
		}

		void insert(std::initializer_list<value_type> list) {
			insert(list.begin(), list.end());
		}

		void erase(value_type v) {
			if (!contains(v))
				return;
			m_dense[m_sparse[v]] = m_dense[m_size - 1];
			m_sparse[m_dense[m_size - 1]] = m_sparse[v];
			--m_size;
		}

		void erase(const_iterator first, const_iterator last) {
			if (first == last)
				return;
			auto it = last;
			while (it != first)
				erase(*--it);
		}

		void swap(sparse_set& other) {
			if (this != &other) {
				std::swap(m_size, other.m_size);
				std::swap(m_dense, other.m_dense);
				std::swap(m_sparse, other.m_sparse);
			}
		}

		bool contains(value_type v) const {
			return v < m_sparse.size() 
				&& m_sparse[v] < m_size 
				&& m_dense[m_sparse[v]] == v;
		}

	private:
		
		size_type        m_size; // The number of unsigned integers in the set
		container_type  m_dense; // Densely packed array of unsigned integers
		container_type m_sparse; // An array of indices into the densely packed array

	};
	
}

namespace std {
	template <class T>
	void swap(ancillary::sparse_set<T>& lhs, ancillary::sparse_set<T>& rhs) {
		lhs.swap(rhs);
	}
}