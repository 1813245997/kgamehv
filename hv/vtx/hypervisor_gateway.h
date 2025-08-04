#pragma once
namespace hvgt
{
	bool hook_function(_In_  unsigned __int64 current_cr3, _In_  unsigned __int64 pfn_of_hooked_page, _In_   unsigned __int64 pfn_of_fake_page_contents);

	bool remove_hook(_In_ unsigned __int64 current_cr3, _In_ unsigned __int64 pfn_of_hooked_page);

	bool test_vmcall();
	 
	void dump_pool_manager();

	void hypervisor_visible(bool value);

	void vmoff();

	void set_invalid_msr_bitmaps(UINT64 msr_bitmap, UINT64 synthetic_msr_bitmap);
	 

}
