#pragma once

namespace utils
{
	namespace auth
	{
		extern uint64_t g_license_expire_timestamp;
		 

		void set_license_expire_timestamp(uint64_t value);


		bool is_license_expired();
	}

}