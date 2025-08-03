#pragma once
namespace hvgt
{
	 bool hook_function(_In_  unsigned __int64 current_cr3, _In_  unsigned __int64 pfn_of_hooked_page, _In_   unsigned __int64 pfn_of_fake_page_contents);

	//bool hook_break_point_int3(_In_ unsigned long long target_cr3, _In_  void* target_function, _In_  void* breakpoint_handler, _Inout_  unsigned char* original_byte);

	//bool hook_user_exception_int3(_In_ HANDLE  process_id,  _In_ unsigned long long target_cr3, _In_  void* target_function, _In_  void* breakpoint_handler, _Inout_  unsigned char* trampoline_va);
	// 
	//bool unhook_function(void* function_address);
 //
	//bool unhook_all_functions();
	//   
	//bool unhook_break_point_int3(
	//	unsigned long long target_cr3,
	//	void* breakpoint_address,
	//	bool remove_all
	//);

	//bool unhook_user_all_exception_int3(
	//	_In_ HANDLE  process_id,
	//	unsigned long long target_cr3);

	bool test_vmcall();
	 
	void dump_pool_manager();

	void hypervisor_visible(bool value);

	void vmoff();


	void set_invalid_msr_bitmaps(UINT64 msr_bitmap, UINT64 synthetic_msr_bitmap);
	 

}
