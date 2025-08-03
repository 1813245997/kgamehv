#include "global_defs.h"
#include "hook_utils.h"
namespace utils
{
	namespace hook_utils
	{
		bool hook_kernel_function(_In_ void* target_api, _In_ void* new_api, _Out_ void** origin_function)
		{
			if (!target_api || !new_api || !origin_function)
			{
				LogError("hook: Invalid parameters. target_api: %p, new_api: %p, origin_function: %p",
					target_api, new_api, origin_function);
				return false;
			}

			*origin_function = nullptr;

			const bool is_intel = utils::khyper_vt::is_intel_cpu();

			if (is_intel)
			{
				bool result = hvgt::hook_kernel_function(target_api, new_api, origin_function);
				if (result)
				{
					LogInfo("hook: [Intel] Successfully hooked. target_api: %p, new_api: %p, origin_function: %p",
						target_api, new_api, *origin_function);
				}
				else
				{
					LogError("hook: [Intel] Hook failed. target_api: %p, new_api: %p", target_api, new_api);
				}
				return result;
			}
			else
			{
				LogError("hook: [AMD] Unsupported CPU vendor. target_api: %p, new_api: %p", target_api, new_api);
				return false;
			}
		}


		bool hook_break_point_int3(_In_ PEPROCESS process, _In_ void* target_api, _In_ void* new_api, _Inout_   unsigned char* original_byte)
		{

			if (process == nullptr)
			{
				// 无效的进程指针
				return false;
			}

			if (target_api == nullptr)
			{
				// 目标函数地址无效
				return false;
			}

			if (new_api == nullptr)
			{
				// 新函数地址无效
				return false;
			}

		 

			const bool is_intel = utils::khyper_vt::is_intel_cpu();
			const bool is_user = utils::memory::is_user_address(target_api);

			const unsigned long long target_cr3 = utils::process_utils::get_process_cr3(process);
			if (target_cr3 == 0)
			{
				return false;
			}

			PMDL mdl = nullptr;

			// 如果是用户态地址，需要锁页
			if (is_user)
			{
				KAPC_STATE apc_state{};
				utils::internal_functions::pfn_ke_stack_attach_process(process, &apc_state);

				const unsigned long long original_page = reinterpret_cast<unsigned long long>(PAGE_ALIGN(target_api));

				NTSTATUS status = utils::memory::lock_memory(original_page, 0x1000, &mdl);
				utils::internal_functions::pfn_ke_unstack_detach_process(&apc_state);

				if (!NT_SUCCESS(status))
				{
					return false;
				}
			}

			// 调用 Intel 的 Hook 方法
			bool result = false;
			if (is_intel)
			{
				result = hvgt::hook_break_point_int3(
					target_cr3,
					target_api,
					new_api,
					original_byte 
					 
				);
			}

			// 如果加锁了，需要解锁
			if (is_user && mdl)
			{
				utils::memory::unlock_memory(mdl);
			}

			return result;
	

			

		}
		bool hook_user_exception_handler(
			_In_ PEPROCESS process,
			_In_ void* target_api,
			_In_ void* exception_handler,
			_In_ bool allocate_trampoline_page
		)
		{
			if (!process || !target_api || !exception_handler)
				return false;

			const bool is_intel = utils::khyper_vt::is_intel_cpu();
			const HANDLE process_id = utils::internal_functions::pfn_ps_get_process_id(process);
			const ULONGLONG target_cr3 = utils::process_utils::get_process_cr3(process);
			if (target_cr3 == 0)
				return false;

			unsigned char* trampoline_va = nullptr;
			PMDL mdl = nullptr;
			KAPC_STATE apc_state{};
			bool apc_attached = false;
			bool result = false;
			bool suspend_success = false;

			// Attach 到目标进程
			utils::internal_functions::pfn_ke_stack_attach_process(process, &apc_state);
			apc_attached = true;

			const ULONGLONG page_base = reinterpret_cast<ULONGLONG>(PAGE_ALIGN(target_api));

			// 先锁页，确保页面存在
			NTSTATUS lock_status = utils::memory::lock_memory(page_base, 0x1000, &mdl);
			if (!NT_SUCCESS(lock_status)) {
				utils::internal_functions::pfn_ke_unstack_detach_process(&apc_state);
				return false;
			}

			// 条件性分配 trampoline 页
			if (allocate_trampoline_page) {
				if (!NT_SUCCESS(utils::memory::allocate_user_hidden_exec_memory(process, reinterpret_cast<PVOID*>(&trampoline_va), 0x1000))) {
					utils::memory::unlock_memory(mdl);
					utils::internal_functions::pfn_ke_unstack_detach_process(&apc_state);
					return false;
				}
			}

			// detach 当前进程
			utils::internal_functions::pfn_ke_unstack_detach_process(&apc_state);
			apc_attached = false;

			// 挂起进程，保证 patch 安全
			NTSTATUS suspend_status = utils::internal_functions::pfn_ps_suspend_process(process);
			if (!NT_SUCCESS(suspend_status)) {
				LogError("hook_user_exception_handler: suspend failed: 0x%X", suspend_status);
				utils::memory::unlock_memory(mdl);
				if (trampoline_va)
					utils::memory::free_user_memory(process_id, trampoline_va, 0x1000);
				return false;
			}
			suspend_success = true;

			// 调用实际 hook 实现（Intel）
			if (is_intel) {
				result = hvgt::hook_user_exception_int3(
					process_id,
					target_cr3,
					target_api,
					exception_handler,
					trampoline_va  // 如果不分配，就是 nullptr
				);

				if (result) {
					LogInfo("hook_user_exception_handler: [Intel] Successfully hooked. target_api: %p, handler: %p, trampoline: %p",
						target_api, exception_handler, trampoline_va);
				}
				else {
					LogInfo("hook_user_exception_handler: [Intel] Failed to hook. target_api: %p, handler: %p", target_api, exception_handler);
				}
			}

			// 解锁页面
			if (mdl) {
				utils::memory::unlock_memory(mdl);
			}

			// 如果 hook 失败，需要释放 trampoline 页
			if (!result && trampoline_va) {
				utils::memory::free_user_memory(process_id, trampoline_va, 0x1000);
			}

			// 恢复进程执行
			if (suspend_success) {
				utils::internal_functions::pfn_ps_resume_process(process);
			}

			// 保底 detach
			if (apc_attached) {
				utils::internal_functions::pfn_ke_unstack_detach_process(&apc_state);
			}

			return result;
		}


		 


		bool unhook_user_all_exception_int3(_In_ PEPROCESS process)
		{

			if (!process)
			{
				return false;
			}

			// 获取 PID 和 CR3
			const HANDLE process_id = utils::internal_functions::pfn_ps_get_process_id(process);
			const ULONGLONG target_cr3 = utils::process_utils::get_process_cr3(process);

			if (target_cr3 == 0 || process_id == nullptr)
			{
				return false; // 获取 CR3 或 PID 失败
			}

			// 执行 Unhook 操作
			bool success = hvgt::unhook_user_all_exception_int3(process_id, target_cr3);
			if (success)
			{
				// 清理 INT3 断点记录
			   ept::remove_breakpoints_by_type_for_process(process_id, hook_type::hook_user_exception_break_point_int3);
			}

			return success;
		}
	 }
}