#include "../utils/global_defs.h"
#include "processor_affinity.h"

namespace hv
{
	void for_each_logical_processor(void (*callback_fn)(void*), void* context)
	{
		uint16_t group_count = utils::internal_functions::pfn_ke_query_active_group_count();

		for (uint16_t group_number = 0; group_number < group_count; ++group_number)
		{
			ULONG32 processor_count = utils::internal_functions::pfn_ke_query_active_processor_count_ex(group_number);

			for (ULONG32 processor_number = 0; processor_number < processor_count; ++processor_number)
			{
				GROUP_AFFINITY group_affinity = { };
				group_affinity.Mask = (KAFFINITY)(1) << processor_number;
				group_affinity.Group = group_number;
				utils::internal_functions::pfn_ke_set_system_group_affinity_thread(&group_affinity, NULL);

				callback_fn(context);
			}
		}
		utils::internal_functions::pfn_ke_revert_to_user_affinity_thread();
	}

}