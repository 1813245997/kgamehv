#pragma once
//
// Success codes
//
#define _NO_CRT_STDIO_INLINE

#include <ntifs.h>
#include <ntimage.h>
#include <intrin.h>
#include <minwindef.h>


//stl

#include <functional>
#include <string>
#include <vector>
#include <list>
#include <deque>
#include <map>
#include <set>
#include <queue>
#include <stack>
#include <set>
#include <utility>
#include <algorithm>
#include <cctype>    // for std::tolower
#include <locale>    // for std::locale


#include "macros.h"
#include "types.h"
#include "ntos_struct_def.h"
#include "native_api.h"
#include "internal_function_defs.h"
#include "kvector.h"
#include "math_utils.h"
#include "string_utils.h"
#include "module_info.h"
#include "os_info.h"
#include "process_utils_struct.h"
#include "windows_version_enum.h"
#include "xor_crypt.h"
#include "import_encrypt.h"
#include "hook_manager.h"
#include "hook_functions.h"
#include "signature_scanner.h"
#include "scanner_fun.h"
#include "scanner_offset.h"
#include "hidden_module_list.h"
#include "hook_manager.h"
#include "process_manager_utils.h"
 
#include "hook_functions.h"
#include "internal_function_defs.h"
#include "feature_data_initializer.h"
#include "scanner_data.h"
#include "kernel_hide_utils.h"
#include "memory_utils.h"
#include "process_utils.h"
#include "feature_offset_initializer.h"
#include "dwm_draw.h"
 
#include "time_utils.h"
#include "cpu_registers.h"
#include "strong_dx.h"
#include "thread_utils.h"
#include "vfun_utils.h"
#include "user_call_utils.h"
#include "sstd_utils.h"
#include "hidden_user_memory_list.h"
#include "fault_uitls.h"
#include "window_utils.h"
#include "csgogame\offsets.h"
#include "csgogame\kcsgo2_utils.h"
#include "csgogame\kcsgo2_CPlayer.h"
#include "csgogame\KMenuConfig.h"
#include "user_comm\user_comm_struct.h"
#include "user_comm\user_comm.h"
#include "auth\auth.h"
#include "file_utils.h"
#include "log_utils.h"
#include "global_initializer.h"
#include "khyper_vt.h"
#include "driver_feature_init.h"
#include "hook_utils.h"
#include "memory_pool.h"
#include "../vtx/hypervisor_gateway.h"
#include "vmcall_comm_args.h"
#include "memory_pool.h"
#include "call_back_utils.h"

 #include "ImGui/imgui.h"

#include "VMProtectDDK.h"
