#include "hv.h"
#include "processor_affinity.h"
 

namespace hv
{
	void ForEachLogicalProcessor(void (*callback_fn)(void*), void* context)
	{
		uint16_t group_count = KeQueryActiveGroupCount();

		for (uint16_t group_number = 0; group_number < group_count; ++group_number)
		{
			ULONG32 processor_count = KeQueryActiveProcessorCountEx(group_number);

			for (ULONG32 processor_number = 0; processor_number < processor_count; ++processor_number)
			{
				GROUP_AFFINITY group_affinity = { };
				group_affinity.Mask = (KAFFINITY)(1) << processor_number;
				group_affinity.Group = group_number;
				KeSetSystemGroupAffinityThread(&group_affinity, NULL);

				callback_fn(context);
			}
		}

		KeRevertToUserAffinityThread();
	}
}
