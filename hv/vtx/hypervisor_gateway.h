#pragma once
namespace hvgt
{
	bool hook_kernel_function(_In_ void* target_address, _In_ void* hook_function, _Inout_ void** origin_function);

	bool hook_break_point_int3(_In_ unsigned long long target_cr3, _In_  void* target_function, _In_  void* breakpoint_handler, _Inout_  unsigned char* original_byte);

	bool hook_user_exception_int3(_In_ HANDLE  process_id,  _In_ unsigned long long target_cr3, _In_  void* target_function, _In_  void* breakpoint_handler, _Inout_  unsigned char* trampoline_va);
	 
	bool unhook_function(void* function_address);
 
	bool unhook_all_functions();
	   

	bool unhook_break_point_int3(
		unsigned long long target_cr3,
		void* breakpoint_address,
		bool remove_all
	);
	bool test_vmcall();
	 
	void dump_pool_manager();

	void hypervisor_visible(bool value);

	void vmoff();


	void set_invalid_msr_bitmaps(UINT64 msr_bitmap, UINT64 synthetic_msr_bitmap);
	 

}
