#include "../utils/global_defs.h"
#include "vt_global.h"



unsigned  long long* g_invalid_msr_bitmap{};

unsigned  long long* g_invalid_synthetic_msr_bitmap{};

COMPATIBILITY_CHECKS_STATUS g_compatibility_check;

__vmm_context* g_vmm_context{};

LIST_ENTRY g_ept_breakpoint_hook_list{};