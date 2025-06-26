#include "global_defs.h"
#include "thread_utils.h"
namespace utils
{
	namespace thread_utils
	{
		unsigned long long get_user_stack_ptr()
		{
			auto current_thread = reinterpret_cast<unsigned long long> (internal_functions::pfn_ke_get_current_thread());
			auto trap_frame = *(KTRAP_FRAME**)(current_thread + 0x90);
			return *(unsigned long long*)&trap_frame->Rsp;
		}
		void sleep(ULONG seconds)
		{
			LARGE_INTEGER larTime = {};
			larTime.QuadPart = -10 * 1000 * 1000;
			larTime.QuadPart *= seconds;
			KeDelayExecutionThread(KernelMode, FALSE, &larTime);
		}
	}
}