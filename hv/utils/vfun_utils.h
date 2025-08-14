#pragma once
namespace utils
{
	namespace vfun_utils
	{
		PVOID get_vfunc(PVOID class_ptr, ULONG index);
		
		void add_ref(PVOID class_ptr);

		void release(PVOID class_ptr);
	}
}