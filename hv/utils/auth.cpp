#include "global_defs.h"
#include "auth.h"

namespace utils
{
	namespace auth
	{
		uint64_t g_license_expire_timestamp{};//Unix timestamp
		 
 

	 
		void set_license_expire_timestamp(uint64_t value)
		{
			 

			g_license_expire_timestamp = value;
		 
			 
		}


		bool is_license_expired()
		{
		  
			if (g_license_expire_timestamp == 0)
			{
				return true;
			}

			LARGE_INTEGER system_time_utc = {};
			KeQuerySystemTime(&system_time_utc);

			uint64_t unix_time = (system_time_utc.QuadPart - 116444736000000000ULL) / 10000000;

			return unix_time > g_license_expire_timestamp;
		}
	}
}