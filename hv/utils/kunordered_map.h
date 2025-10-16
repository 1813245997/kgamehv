#pragma once
namespace utils {

	// Hash function template
	template<typename T>
	struct khash {
		size_t operator()(const T& key) const {
			// Simple hash implementation, can be optimized as needed
			return static_cast<size_t>(key);
		}
	};

	// Specialized version for strings
	template<>
	struct khash<const char*> {
		size_t operator()(const char* key) const {
			if (!key) return 0;
			size_t hash = 5381;
			int c;
			while ((c = *key++)) {
				hash = ((hash << 5) + hash) + c;
			}
			return hash;
		}
	};

	template<>
	struct khash<char*> {
		size_t operator()(char* key) const {
			return khash<const char*>()(key);
		}
	};

	// Key-value pair structure
	template<typename Key, typename Value>
	struct kpair {
		Key first;
		Value second;
		
		kpair() : first(), second() {}
		kpair(const Key& k, const Value& v) : first(k), second(v) {}
		kpair(const kpair& other) : first(other.first), second(other.second) {}
		
		kpair& operator=(const kpair& other) {
			if (this != &other) {
				first = other.first;
				second = other.second;
			}
			return *this;
		}
	};

	// Hash table node
	template<typename Key, typename Value>
	struct khash_node {
		kpair<Key, Value> data;
		khash_node* next;
		
		khash_node() : next(nullptr) {}
		khash_node(const Key& k, const Value& v) : data(k, v), next(nullptr) {}
		khash_node(const kpair<Key, Value>& p) : data(p), next(nullptr) {}
	};

	// Forward declaration for iterator
	template<typename Key, typename Value, typename Hash>
	class kunordered_map;

	// Iterator category tags
	struct forward_iterator_tag {};

	template<typename Key, typename Value, typename Hash>
	class khash_iterator {
	public:
		typedef kpair<Key, Value> value_type;
		typedef kpair<Key, Value>* pointer;
		typedef kpair<Key, Value>& reference;
		typedef ptrdiff_t difference_type;
		typedef forward_iterator_tag iterator_category;

		khash_iterator() : m_node(nullptr), m_bucket_index(0), m_map(nullptr) {}
		khash_iterator(khash_node<Key, Value>* node, size_t bucket_index, kunordered_map<Key, Value, Hash>* map)
			: m_node(node), m_bucket_index(bucket_index), m_map(map) {}

		reference operator*() const { return m_node->data; }
		pointer operator->() const { return &m_node->data; }

		khash_iterator& operator++() {
			if (m_node) {
				m_node = m_node->next;
				if (!m_node) {
					// Move to next non-empty bucket
					m_map->find_next_bucket(m_bucket_index, m_node);
				}
			}
			return *this;
		}

		khash_iterator operator++(int) {
			khash_iterator temp = *this;
			++(*this);
			return temp;
		}

		bool operator==(const khash_iterator& other) const {
			return m_node == other.m_node;
		}

		bool operator!=(const khash_iterator& other) const {
			return !(*this == other);
		}

	private:
		khash_node<Key, Value>* m_node;
		size_t m_bucket_index;
		kunordered_map<Key, Value, Hash>* m_map;
		
		friend class kunordered_map<Key, Value, Hash>;
	};

	// Kernel version of unordered_map
	template<typename Key, typename Value, typename Hash = khash<Key>>
	class kunordered_map {
	public:
		typedef Key key_type;
		typedef Value mapped_type;
		typedef kpair<Key, Value> value_type;
		typedef Hash hasher;
		typedef khash_iterator<Key, Value, Hash> iterator;
		typedef const khash_iterator<Key, Value, Hash> const_iterator;
		typedef size_t size_type;

		// Constructor
		kunordered_map() : m_buckets(nullptr), m_bucket_count(0), m_size(0), m_valid(false) {}

		~kunordered_map() {
			clear();
		}

		// Initialization
		void init() {
			m_buckets = nullptr;
			m_bucket_count = 0;
			m_size = 0;
			m_valid = false;
		}

		// Insert operations
		kpair<iterator, bool> insert(const value_type& value) {
			return insert(value.first, value.second);
		}

		kpair<iterator, bool> insert(const Key& key, const Value& value) {
			if (!ensure_capacity()) {
				return kpair<iterator, bool>(end(), false);
			}

			size_t bucket_index = get_bucket_index(key);
			khash_node<Key, Value>* node = find_in_bucket(bucket_index, key);
			
			if (node) {
				// Key already exists, update value
				node->data.second = value;
				return kpair<iterator, bool>(iterator(node, bucket_index, this), false);
			}

			// Create new node
			khash_node<Key, Value>* new_node = create_node(key, value);
			if (!new_node) {
				return kpair<iterator, bool>(end(), false);
			}

			// Insert into bucket
			new_node->next = m_buckets[bucket_index];
			m_buckets[bucket_index] = new_node;
			m_size++;

			return kpair<iterator, bool>(iterator(new_node, bucket_index, this), true);
		}

		// Access operations
		Value& operator[](const Key& key) {
			auto result = insert(key, Value());
			return result.first->second;
		}

		const Value& at(const Key& key) const {
			const_iterator it = find(key);
			if (it == end()) {
				// In real implementation should throw exception, but in kernel environment we return default value
				static Value default_value;
				return default_value;
			}
			return it->second;
		}

		Value& at(const Key& key) {
			iterator it = find(key);
			if (it == end()) {
				// In real implementation should throw exception, but in kernel environment we return default value
				static Value default_value;
				return default_value;
			}
			return it->second;
		}

		// Search operations
		iterator find(const Key& key) {
			if (!m_valid || m_bucket_count == 0) {
				return end();
			}

			size_t bucket_index = get_bucket_index(key);
			khash_node<Key, Value>* node = find_in_bucket(bucket_index, key);
			
			if (node) {
				return iterator(node, bucket_index, this);
			}
			return end();
		}

		const_iterator find(const Key& key) const {
			return const_cast<kunordered_map*>(this)->find(key);
		}

		// Delete operations
		size_type erase(const Key& key) {
			if (!m_valid || m_bucket_count == 0) {
				return 0;
			}

			size_t bucket_index = get_bucket_index(key);
			khash_node<Key, Value>* prev = nullptr;
			khash_node<Key, Value>* current = m_buckets[bucket_index];

			while (current) {
				if (current->data.first == key) {
					if (prev) {
						prev->next = current->next;
					} else {
						m_buckets[bucket_index] = current->next;
					}
					
					destroy_node(current);
					m_size--;
					return 1;
				}
				prev = current;
				current = current->next;
			}
			return 0;
		}

		// Capacity operations
		size_type size() const { return m_size; }
		bool empty() const { return m_size == 0; }
		size_type bucket_count() const { return m_bucket_count; }

		// Iterators
		iterator begin() {
			if (!m_valid || m_size == 0) {
				return end();
			}

			// Find first non-empty bucket
			for (size_t i = 0; i < m_bucket_count; ++i) {
				if (m_buckets[i]) {
					return iterator(m_buckets[i], i, this);
				}
			}
			return end();
		}

		iterator end() {
			return iterator(nullptr, 0, this);
		}

		const_iterator begin() const {
			return const_cast<kunordered_map*>(this)->begin();
		}

		const_iterator end() const {
			return const_cast<kunordered_map*>(this)->end();
		}

		// Cleanup
		void clear() {
			if (m_buckets && m_valid) {
				for (size_t i = 0; i < m_bucket_count; ++i) {
					khash_node<Key, Value>* node = m_buckets[i];
					while (node) {
						khash_node<Key, Value>* next = node->next;
						destroy_node(node);
						node = next;
					}
				}
				utils::memory::free_independent_pages(m_buckets, m_bucket_count * sizeof(khash_node<Key, Value>*));
				m_buckets = nullptr;
				m_bucket_count = 0;
				m_size = 0;
				m_valid = false;
			}
		}

		// Helper function for iterator
		void find_next_bucket(size_t& bucket_index, khash_node<Key, Value>*& node) const {
			bucket_index++;
			while (bucket_index < m_bucket_count && !m_buckets[bucket_index]) {
				bucket_index++;
			}
			if (bucket_index < m_bucket_count) {
				node = m_buckets[bucket_index];
			} else {
				node = nullptr;
			}
		}

	private:
		khash_node<Key, Value>** m_buckets;
		size_t m_bucket_count;
		size_t m_size;
		bool m_valid;
		Hash m_hasher;

		// Get bucket index
		size_t get_bucket_index(const Key& key) const {
			return m_hasher(key) % m_bucket_count;
		}

		// Find node in bucket
		khash_node<Key, Value>* find_in_bucket(size_t bucket_index, const Key& key) const {
			khash_node<Key, Value>* node = m_buckets[bucket_index];
			while (node) {
				if (node->data.first == key) {
					return node;
				}
				node = node->next;
			}
			return nullptr;
		}

		// Create node
		khash_node<Key, Value>* create_node(const Key& key, const Value& value) {
			khash_node<Key, Value>* node = (khash_node<Key, Value>*)utils::memory::allocate_independent_pages(
				sizeof(khash_node<Key, Value>), PAGE_READWRITE);
			if (!node) return nullptr;

			new (node) khash_node<Key, Value>(key, value);
			return node;
		}

		// Destroy node
		void destroy_node(khash_node<Key, Value>* node) {
			if (node) {
				node->~khash_node<Key, Value>();
				utils::memory::free_independent_pages(node, sizeof(khash_node<Key, Value>));
			}
		}

		// Ensure capacity is sufficient
		bool ensure_capacity() {
			if (!m_valid) {
				return initialize_buckets(8); // Initial bucket count
			}

			// If load factor is too high, rehash
			if (m_size >= m_bucket_count * 2) {
				return rehash(m_bucket_count * 2);
			}

			return true;
		}

		// Initialize buckets
		bool initialize_buckets(size_t bucket_count) {
			m_buckets = (khash_node<Key, Value>**)utils::memory::allocate_independent_pages(
				bucket_count * sizeof(khash_node<Key, Value>*), PAGE_READWRITE);
			if (!m_buckets) return false;

			RtlZeroMemory(m_buckets, bucket_count * sizeof(khash_node<Key, Value>*));
			m_bucket_count = bucket_count;
			m_valid = true;
			return true;
		}

		// Rehash
		bool rehash(size_t new_bucket_count) {
			khash_node<Key, Value>** old_buckets = m_buckets;
			size_t old_bucket_count = m_bucket_count;

			// Allocate new bucket array
			m_buckets = (khash_node<Key, Value>**)utils::memory::allocate_independent_pages(
				new_bucket_count * sizeof(khash_node<Key, Value>*), PAGE_READWRITE);
			if (!m_buckets) {
				m_buckets = old_buckets;
				return false;
			}

			RtlZeroMemory(m_buckets, new_bucket_count * sizeof(khash_node<Key, Value>*));
			m_bucket_count = new_bucket_count;

			// Re-insert all elements
			for (size_t i = 0; i < old_bucket_count; ++i) {
				khash_node<Key, Value>* node = old_buckets[i];
				while (node) {
					khash_node<Key, Value>* next = node->next;
					
					// Calculate new bucket index
					size_t new_bucket_index = get_bucket_index(node->data.first);
					
					// Insert into new bucket
					node->next = m_buckets[new_bucket_index];
					m_buckets[new_bucket_index] = node;
					
					node = next;
				}
			}

			// Free old bucket array
			utils::memory::free_independent_pages(old_buckets, old_bucket_count * sizeof(khash_node<Key, Value>*));
			return true;
		}
	};

}