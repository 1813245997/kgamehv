#include "global_defs.h"
#include "vfun_utils.h"

namespace utils
{
	namespace vfun_utils
	{

		PVOID get_vfunc(PVOID class_ptr, ULONG index)
		{
			return (*(PVOID**)class_ptr)[index];
		}

		void add_ref(PVOID class_ptr)
		{
			user_call::call(
				reinterpret_cast<unsigned long long> (get_vfunc(class_ptr, 1)),
				reinterpret_cast<unsigned long long> (class_ptr),
				0ull,
				0ull,
				0ull);
		}


		void release(PVOID class_ptr)
		{
			user_call::call  ( 
				reinterpret_cast<unsigned long long> (get_vfunc(class_ptr, 2)), 
				reinterpret_cast<unsigned long long> (class_ptr), 
				0ull,
				0ull,
				0ull);
		}
	}

}

