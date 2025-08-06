#pragma once
namespace utils {


	template<typename T>
	class kvector
	{
	public:
		kvector() : m_data(nullptr), m_size(0), m_capacity(0) {}

		~kvector() {
			clear();
		}

		bool push_back(const T& value) {
			if (m_size >= m_capacity) {
				if (!grow()) return false;
			}
			m_data[m_size++] = value;
			return true;
		}

		T& operator[](size_t index) {
			return m_data[index];
		}

		const T& operator[](size_t index) const {
			return m_data[index];
		}

		size_t size() const {
			return m_size;
		}

		void clear() {
			if (m_data) {
				ExFreePool(m_data);
				m_data = nullptr;
				m_size = 0;
				m_capacity = 0;
			}
		}


		T* begin() {
			return m_data;
		}

		T* end() {
			return m_data + m_size;
		}

		const T* begin() const {
			return m_data;
		}

		const T* end() const {
			return m_data + m_size;
		}

		bool assign(const T* first, const T* last) {
			clear();
			size_t count = (size_t)(last - first);
			if (count == 0)
				return true;

			m_data = (T*)ExAllocatePool(NonPagedPool, count * sizeof(T));
			if (!m_data) return false;

			RtlCopyMemory(m_data, first, count * sizeof(T));
			m_size = count;
			m_capacity = count;
			return true;
		}

	private:
		bool grow() {
			size_t new_capacity = (m_capacity == 0) ? 4 : m_capacity * 2;
			T* new_data = (T*)ExAllocatePool(NonPagedPool, new_capacity * sizeof(T));
			if (!new_data) return false;

			if (m_data) {
				RtlCopyMemory(new_data, m_data, m_size * sizeof(T));
				ExFreePool(m_data);
			}

			m_data = new_data;
			m_capacity = new_capacity;
			return true;
		}

	private:
		T* m_data;
		size_t m_size;
		size_t m_capacity;
	};

}