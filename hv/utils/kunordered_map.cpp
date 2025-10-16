#include "global_defs.h"
#include "kunordered_map.h"

namespace utils {

	// Here we can add some non-template helper function implementations
	// Since kunordered_map is a template class, main implementation is in header file
	
	// Example: some common hash function implementations
	size_t hash_combine(size_t seed, size_t value) {
		// Simple hash combination function
		return seed ^ (value + 0x9e3779b9 + (seed << 6) + (seed >> 2));
	}

	// Optimized version of string hash function
	size_t hash_string(const char* str, size_t length) {
		if (!str) return 0;
		
		size_t hash = 5381;
		for (size_t i = 0; i < length; ++i) {
			hash = ((hash << 5) + hash) + static_cast<unsigned char>(str[i]);
		}
		return hash;
	}

	// Integer hash function
	size_t hash_integer(uintptr_t value) {
		// Use multiplicative hashing
		const size_t multiplier = 0x9e3779b97f4a7c15ULL;
		return static_cast<size_t>((value * multiplier) >> 32);
	}

}