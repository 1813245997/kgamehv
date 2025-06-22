
#include "global_defs.h"

#include "hook_functions.h"
#include "dwm_draw.h"
#include "strong_dx.h"
namespace hook_functions
{

 



 	BOOLEAN(__fastcall* original_ki_preprocess_fault)(
		IN OUT PEXCEPTION_RECORD ExceptionRecord,
		IN OUT PCONTEXT ContextRecord,
		IN KPROCESSOR_MODE PreviousMode);

	BOOLEAN __fastcall hook_ki_preprocess_fault(
		_Inout_ PEXCEPTION_RECORD ExceptionRecord,
		_Inout_ PCONTEXT ContextRecord,
		_In_ KPROCESSOR_MODE PreviousMode)
	{
		//不可以dprinftf 输出 因为是触发异常输出的
		hyper::EptHookInfo* matched_hook_info = nullptr;
		 
		if (ExceptionRecord->ExceptionCode != STATUS_BREAKPOINT)
		{
			return original_ki_preprocess_fault(ExceptionRecord, ContextRecord, PreviousMode);
		}

		if (PreviousMode == MODE::UserMode)
		{
			if (!find_hook_break_point_int3(ExceptionRecord->ExceptionAddress, &matched_hook_info))
			{
				return original_ki_preprocess_fault(ExceptionRecord, ContextRecord, PreviousMode);
			}

			using handler_fn_t =BOOLEAN(__fastcall*)(PEXCEPTION_RECORD, PCONTEXT, hyper::EptHookInfo*);
			auto handler = reinterpret_cast<handler_fn_t>(matched_hook_info->handler_va);

			if (handler(ExceptionRecord, ContextRecord, matched_hook_info))
			{
				return TRUE;  // 已处理
			}
		}
		return original_ki_preprocess_fault(ExceptionRecord, ContextRecord, PreviousMode);
	}


	 BOOLEAN(__fastcall* original_mm_is_address_valid)(
		_In_ PVOID VirtualAddress
		);

	BOOLEAN  __fastcall hook_mm_is_address_valid(
		_In_ PVOID VirtualAddress
	)
	{

		ULONG process_id = reinterpret_cast<ULONG>(utils::internal_functions::pfn_ps_get_current_process_id());

	   if (utils::hidden_modules::is_address_hidden(VirtualAddress))
	   {
		   return FALSE;
	   }

	   if (utils::hidden_user_memory::is_address_hidden_for_pid(process_id, reinterpret_cast<unsigned long long>(VirtualAddress)))
	   {
		   return FALSE;
	   }

		return utils::internal_functions::pfn_mm_is_address_valid_ex (VirtualAddress);
	}


	  NTSTATUS(__fastcall* original_mm_copy_memory)(
		  _In_ PVOID target_address,
		  _In_ MM_COPY_ADDRESS source_address,
		  _In_ SIZE_T number_of_bytes,
		  _In_ ULONG flags,
		  _Out_ PSIZE_T number_of_bytes_transferred
		);


	  NTSTATUS __fastcall hook_mm_copy_memory(
		  _In_ PVOID target_address,
		  _In_ MM_COPY_ADDRESS source_address,
		  _In_ SIZE_T number_of_bytes,
		  _In_ ULONG flags,
		  _Out_ PSIZE_T number_of_bytes_transferred
	  )
	  {
		  
		  if (target_address == nullptr )
		  {
			  return original_mm_copy_memory(
				  target_address,
				  source_address,
				  number_of_bytes,
				  flags,
				  number_of_bytes_transferred
			  );
		  }

		  if (number_of_bytes == 0)
		  {
			  return original_mm_copy_memory(
				  target_address,
				  source_address,
				  number_of_bytes,
				  flags,
				  number_of_bytes_transferred
			  );
		  }

		  PVOID64 virtual_address = nullptr;
		  ULONG process_id = reinterpret_cast<ULONG>(utils::internal_functions::pfn_ps_get_current_process_id());
		  if (flags == MM_COPY_MEMORY_PHYSICAL)
		  {
			  virtual_address = utils::internal_functions::pfn_mm_get_virtual_for_physical (source_address.PhysicalAddress);
		  }
		  else
		  {
			  virtual_address = source_address.VirtualAddress;
		  }

		  
		  if (utils::hidden_modules::is_address_hidden(virtual_address))
		  {
			  *number_of_bytes_transferred = 0;
			  return STATUS_CONFLICTING_ADDRESSES;
		  }

		  if (utils::hidden_user_memory::is_address_hidden_for_pid(process_id, reinterpret_cast<unsigned long long>(virtual_address)))
		  {
			  *number_of_bytes_transferred = 0;
			  return STATUS_CONFLICTING_ADDRESSES;
		  }
		 
		  return original_mm_copy_memory(
			  target_address,
			  source_address,
			  number_of_bytes,
			  flags,
			  number_of_bytes_transferred
		  );
	  }

	  

	 ULONG(NTAPI* original_rtl_walk_frame_chain)(
		 _Out_writes_(count - (flags >> RTL_STACK_WALKING_MODE_FRAMES_TO_SKIP_SHIFT)) PVOID* callers,
		 _In_ ULONG count,
		 _In_ ULONG flags
		 );

	 ULONG NTAPI hook_rtl_walk_frame_chain(
		 _Out_writes_(count - (flags >> RTL_STACK_WALKING_MODE_FRAMES_TO_SKIP_SHIFT)) PVOID* callers,
		 _In_ ULONG count,
		 _In_ ULONG flags
	 )
	 {
		 ULONG frames_captured = original_rtl_walk_frame_chain(callers, count, flags);

		 if (frames_captured == 0)
		 {
			 return frames_captured;
		 }

		 if (!callers)
		 {
			 return frames_captured;
		 }


		 if (!utils::internal_functions::pfn_mm_is_address_valid_ex(callers))
		 {
			 return frames_captured;
		 }

		 ULONG process_id = reinterpret_cast<ULONG>(utils::internal_functions::pfn_ps_get_current_process_id());

		 for (ULONG i = 0; i < frames_captured; ++i)
		 {
			 

			 if (!utils::internal_functions::pfn_mm_is_address_valid_ex(callers[i]))
				 continue;

			 PVOID addr = callers[i];
			 if (utils::hidden_modules::is_address_hidden(addr))
			 {
				 callers[i] = nullptr;
				 return i;
			 }

			 if (utils::hidden_user_memory::is_address_hidden_for_pid(process_id, reinterpret_cast<unsigned long long>(addr)))
			 {
				 callers[i] = nullptr;
				 return i;
			 }
		 }

		 return frames_captured;
	 }


	 PRUNTIME_FUNCTION(NTAPI* original_rtl_lookup_function_entry)(
		 _In_ DWORD64 control_pc,
		 _Out_ PDWORD64 image_base,
		 _Inout_opt_ PUNWIND_HISTORY_TABLE history_table
		 );

	 PRUNTIME_FUNCTION NTAPI hook_rtl_lookup_function_entry(
		 _In_ DWORD64 control_pc,
		 _Out_ PDWORD64 image_base,
		 _Inout_opt_ PUNWIND_HISTORY_TABLE history_table
	 )
	 {
		  

		 ULONG process_id = reinterpret_cast<ULONG>(utils::internal_functions::pfn_ps_get_current_process_id());

		 // 判断是否是隐藏模块地址
		 if (utils::hidden_modules::is_address_hidden(reinterpret_cast<PVOID>(control_pc)))
		 {
			 if (image_base)
			 {
				 *image_base = 0;
			 }
			 return nullptr;
		 }

		 // 判断是否是该进程隐藏的用户态内存地址
		 if (utils::hidden_user_memory::is_address_hidden_for_pid(process_id, control_pc))
		 {
			 if (image_base)
			 {
				 *image_base = 0;
			 }
			 return nullptr;
		 }

		 return original_rtl_lookup_function_entry(control_pc, image_base, history_table);
	 }


	   void(__fastcall* original_psp_exit_process)(
		 IN BOOLEAN trim_address_space,
		 IN PEPROCESS process
		 );

	   void __fastcall hook_psp_exit_process(
		   IN BOOLEAN trim_address_space,
		   IN PEPROCESS process
	   )
	   {
		   //特殊处理待处理
	/*	   if (utils::dwm_draw::g_dwm_process == process )
		   {

		   }*/

		   /*  HANDLE  process_id  =  utils::internal_functions::pfn_ps_get_process_id(process) ;
			 hyper::unhook_all_ept_hooks_for_pid(process_id);
			 utils::hidden_user_memory::remove_hidden_addresses_for_pid(reinterpret_cast<ULONG> (process_id));*/
		   return original_psp_exit_process(trim_address_space, process);

	 }


	   NTSTATUS(NTAPI* original_nt_create_section)(
		   _Out_ PHANDLE section_handle,
		   _In_ ACCESS_MASK desired_access,
		   _In_opt_ POBJECT_ATTRIBUTES object_attributes,
		   _In_opt_ PLARGE_INTEGER maximum_size,
		   _In_ ULONG section_page_protection,
		   _In_ ULONG allocation_attributes,
		   _In_opt_ HANDLE file_handle
		   );


	   NTSTATUS NTAPI hook_nt_create_section(
		   _Out_ PHANDLE section_handle,
		   _In_ ACCESS_MASK desired_access,
		   _In_opt_ POBJECT_ATTRIBUTES object_attributes,
		   _In_opt_ PLARGE_INTEGER maximum_size,
		   _In_ ULONG section_page_protection,
		   _In_ ULONG allocation_attributes,
		   _In_opt_ HANDLE file_handle
	   )
	   {
		   return original_nt_create_section(
			   section_handle,
			   desired_access,
			   object_attributes,
			   maximum_size,
			   section_page_protection,
			   allocation_attributes,
			   file_handle
		   );
	   }


	   INT64(__fastcall* original_present_multiplane_overlay)(
		   void* thisptr,
		   PVOID dxgi_swap_chain,
		   unsigned int a3,
		   unsigned int a4,
		   int a5,
		   const void* a6,
		   PVOID64 a7,
		   unsigned int a8
		   ) = nullptr;

	   BOOLEAN  __fastcall new_present_multiplane_overlay(
		   _Inout_ PEXCEPTION_RECORD ExceptionRecord,
		   _Inout_ PCONTEXT ContextRecord,
		   _Inout_ hyper::EptHookInfo* matched_hook_info)
	   {
		   UNREFERENCED_PARAMETER(ExceptionRecord);

		
		    
		
		   if (!utils::dwm_draw::g_pswap_chain)
		   {
			   utils::dwm_draw::g_pswap_chain = ContextRecord->Rcx;
			   utils::dwm_draw::g_dwm_render_thread = utils::internal_functions::pfn_ps_get_current_thread();
			
			   
		   }

		   ContextRecord->Rip = reinterpret_cast<unsigned long long> (matched_hook_info->trampoline_va);

		   return TRUE;
	   }


	      INT64(__fastcall* original_cdxgi_swap_chain_dwm_legacy_present_dwm)(
		   void* pthis,
		   PVOID pDxgiSwapChain,
		   unsigned int a3,
		   unsigned int a4,
		   const PVOID a5, unsigned int a6, PVOID64 a7, unsigned int a8,
		   PVOID a9, unsigned int a10) = nullptr;


	   BOOLEAN  __fastcall new_cdxgi_swap_chain_dwm_legacy_present_dwm(
		   _Inout_ PEXCEPTION_RECORD ExceptionRecord,
		   _Inout_ PCONTEXT ContextRecord,
		   _Inout_ hyper::EptHookInfo* matched_hook_info)
	   {
		   UNREFERENCED_PARAMETER(ExceptionRecord);
		 

		   if (!utils::dwm_draw::g_pswap_chain)
		   {
			   utils::dwm_draw::g_pswap_chain = ContextRecord->Rcx;
			  
		   }
		   ContextRecord->Rip = reinterpret_cast<unsigned long long> (matched_hook_info->trampoline_va);

		   return TRUE;
	   }


	   HRESULT __fastcall  new_get_buffer(
		   _Inout_ PEXCEPTION_RECORD ExceptionRecord,
		   _Inout_ PCONTEXT ContextRecord,
		   _Inout_ hyper::EptHookInfo* matched_hook_info)
	   {
		   while (InterlockedCompareExchange(&utils::strong_dx::g_dwm_render_lock, 1, 0) != 0)
		   {
			   LARGE_INTEGER larTime = {  };
			   larTime.QuadPart = ((ULONG64)-10000) * 1000;
			   KeDelayExecutionThread(KernelMode, FALSE, &larTime);
		   }
		   

		   ContextRecord->Rip = reinterpret_cast<unsigned long long> (matched_hook_info->trampoline_va);

		   return TRUE;
	   }

	   __int64(__fastcall* original_dxgk_get_device_state)(
		   _Inout_ PVOID unnamedParam1)
		   = nullptr;



	   __int64 __fastcall hook_dxgk_get_device_state(_Inout_ PVOID unnamedParam1)
	   {
		   static bool has_hooked_get_buffer = false;
			if (utils::internal_functions:: pfn_ke_get_current_irql() >= DISPATCH_LEVEL)
			{
				return  original_dxgk_get_device_state(unnamedParam1);
			}

			if (utils::internal_functions::pfn_ex_get_previous_mode() != MODE::UserMode)
			{
				return  original_dxgk_get_device_state(unnamedParam1);
			}
			
			if (!unnamedParam1)
			{
				return  original_dxgk_get_device_state(unnamedParam1);
			}

			if (utils::dwm_draw::g_dwm_process == nullptr)
			{
				return  original_dxgk_get_device_state(unnamedParam1);
			}

			if (utils::internal_functions::pfn_ps_get_current_process()!=utils :: dwm_draw::g_dwm_process)
			{
				return  original_dxgk_get_device_state(unnamedParam1);
			}

			if (!utils::strong_dx::is_user_stack_in_dxgi_range())
			{
				return  original_dxgk_get_device_state(unnamedParam1);
			}

			if (!utils::dwm_draw::g_pswap_chain)
			{
				return  original_dxgk_get_device_state(unnamedParam1);
			}

			if (!utils::internal_functions::pfn_mm_is_address_valid_ex(reinterpret_cast<PVOID> (utils::dwm_draw::g_pswap_chain)))
			{
				return  original_dxgk_get_device_state(unnamedParam1);
			}
			 
			  

			if (InterlockedCompareExchange(&utils::strong_dx::g_dwm_render_lock, 1, 0) != 0)
			{
				return  original_dxgk_get_device_state(unnamedParam1);
			}

			if (!NT_SUCCESS(utils::strong_dx::initialize(utils::dwm_draw::g_pswap_chain)))
			{
				return  original_dxgk_get_device_state(unnamedParam1);
			}

			if (!has_hooked_get_buffer)
			{
				 
				utils::dwm_draw::hook_get_buffer(utils::dwm_draw::g_dwm_process);
				has_hooked_get_buffer = true;
			}

			utils::strong_dx::render_overlay_frame(utils::strong_dx::draw_overlay_elements);

			InterlockedExchange(&utils::strong_dx::g_dwm_render_lock, 0);
		
		  
		   return  original_dxgk_get_device_state(unnamedParam1);
	   }

	   NTSTATUS(NTAPI* original_nt_query_virtual_memory) (
		   _In_ HANDLE ProcessHandle,
		   _In_opt_ PVOID BaseAddress,
		   _In_ MEMORY_INFORMATION_CLASS MemoryInformationClass,
		   _Out_writes_bytes_(MemoryInformationLength) PVOID MemoryInformation,
		   _In_ SIZE_T MemoryInformationLength,
		   _Out_opt_ PSIZE_T ReturnLength
		   ) = nullptr;

	   NTSTATUS NTAPI  new_nt_query_virtual_memory(
		   _In_ HANDLE ProcessHandle,
		   _In_opt_ PVOID BaseAddress,
		   _In_ MEMORY_INFORMATION_CLASS MemoryInformationClass,
		   _Out_writes_bytes_(MemoryInformationLength) PVOID MemoryInformation,
		   _In_ SIZE_T MemoryInformationLength,
		   _Out_opt_ PSIZE_T ReturnLength
	   )
	   {

		   if (MemoryInformationClass != MemoryBasicInformation &&
			   MemoryInformationClass != 2)
		   {
			   return original_nt_query_virtual_memory(ProcessHandle, BaseAddress, MemoryInformationClass, MemoryInformation, MemoryInformationLength, ReturnLength);
		   }

		   PROCESS_BASIC_INFORMATION ProcessBasicInfo;
		   NTSTATUS QueryStatus = utils::internal_functions::pfn_zw_query_information_process(
			   ProcessHandle,
			   ProcessBasicInformation,
			   (PVOID)&ProcessBasicInfo,
			   sizeof(ProcessBasicInfo),
			   NULL
		   );

		   if (!NT_SUCCESS(QueryStatus)) {

			   return original_nt_query_virtual_memory(ProcessHandle, BaseAddress, MemoryInformationClass, MemoryInformation, MemoryInformationLength, ReturnLength);
		   }


		   if (!utils::hidden_user_memory::is_address_hidden_for_pid( ProcessBasicInfo.UniqueProcessId ,reinterpret_cast<unsigned long long > ( BaseAddress))) {
			   return original_nt_query_virtual_memory(ProcessHandle, BaseAddress, MemoryInformationClass, MemoryInformation, MemoryInformationLength, ReturnLength);
		   }



		   NTSTATUS  Status = original_nt_query_virtual_memory(ProcessHandle, BaseAddress, MemoryInformationClass, MemoryInformation, MemoryInformationLength, ReturnLength);
		   if (!NT_SUCCESS(Status))
		   {
			   return Status;
		   }
		   PMEMORY_BASIC_INFORMATION pMBI = static_cast<PMEMORY_BASIC_INFORMATION>(
			   MemoryInformation);

		   PVOID NextBase = reinterpret_cast<PUCHAR>(pMBI->BaseAddress) +
			   pMBI->RegionSize;

		   MEMORY_BASIC_INFORMATION NextBlock;
		   SIZE_T NextLength = 0;
		   Status = original_nt_query_virtual_memory(ProcessHandle, NextBase,
			   MemoryInformationClass, &NextBlock, sizeof(NextBlock), &NextLength);


		   pMBI->AllocationBase = 0;
		   pMBI->AllocationProtect = 0;
		   pMBI->State = MEM_FREE;
		   pMBI->Protect = PAGE_NOACCESS;
		   pMBI->Type = 0;

		   // If next block is free too, add it to the region size
		   if (NextBlock.State == MEM_FREE)
			   pMBI->RegionSize += NextBlock.RegionSize;


		   // Clear the output buffer if the address is hidden


		   return Status;


	   }
	    
	   extern NTSTATUS(NTAPI* original_nt_read_virtual_memory)(
		   _In_ HANDLE ProcessHandle,
		   _In_opt_ PVOID BaseAddress,
		   _Out_writes_bytes_(NumberOfBytesToRead) PVOID Buffer,
		   _In_ SIZE_T NumberOfBytesToRead,
		   _Out_opt_ PSIZE_T NumberOfBytesRead
		   )=nullptr;


	   NTSTATUS NTAPI new_nt_read_virtual_memory(
		   _In_ HANDLE ProcessHandle,
		   _In_opt_ PVOID BaseAddress,
		   _Out_writes_bytes_(NumberOfBytesToRead) PVOID Buffer,
		   _In_ SIZE_T NumberOfBytesToRead,
		   _Out_opt_ PSIZE_T NumberOfBytesRead
	   )
	   {
		   PROCESS_BASIC_INFORMATION ProcessBasicInfo;
		   NTSTATUS QueryStatus = utils::internal_functions::pfn_zw_query_information_process(
			   ProcessHandle,
			   ProcessBasicInformation,
			   (PVOID)&ProcessBasicInfo,
			   sizeof(ProcessBasicInfo),
			   NULL
		   );

		   if (!NT_SUCCESS(QueryStatus)) {

			   return original_nt_read_virtual_memory(ProcessHandle, BaseAddress, Buffer, NumberOfBytesToRead, NumberOfBytesRead);
		   }

		   if (utils::hidden_user_memory::is_address_hidden_for_pid(ProcessBasicInfo.UniqueProcessId, reinterpret_cast<unsigned long long> (BaseAddress))) {

			   
			   return STATUS_ACCESS_VIOLATION;
		   }


		   return original_nt_read_virtual_memory(ProcessHandle, BaseAddress, Buffer, NumberOfBytesToRead, NumberOfBytesRead);
	   }
}
