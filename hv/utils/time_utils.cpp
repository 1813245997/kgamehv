#include "global_defs.h"
#include "time_utils.h"

unsigned long long utils::time_utils::get_real_time()
{
	return  get_system_time() / (10 * 1000);
}

inline unsigned long long utils::time_utils::get_system_time()
{
	constexpr unsigned long long kuser_shared_data_base = 0xFFFFF78000000000ULL;
	constexpr unsigned long long system_time_offset = 0x14;

	return *(volatile unsigned long long*)(kuser_shared_data_base + system_time_offset);
}