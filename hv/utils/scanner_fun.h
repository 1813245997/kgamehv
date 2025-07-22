#pragma once
namespace utils
{
	namespace scanner_fun
	{
		unsigned long long find_module_export_by_name(void* module_base, const char* export_name);

		unsigned long long find_win32k_exprot_by_name(const char* export_name);


		unsigned long long find_ki_preprocess_fault();

		unsigned long long find_mm_copy_memory();

		unsigned long long find_mm_is_address_valid();

		unsigned long long find_mm_is_address_valid_ex();

		unsigned long long find_rtl_walk_frame_chain();

		unsigned long long find_rtl_lookup_function_entry();

		unsigned long long find_psp_exit_process();

		unsigned long long find_exp_lookup_handle_table_entry();

		unsigned long long find_mm_create_kernel_stack();

		unsigned long long find_mm_delete_kernel_stack();

		unsigned long long find_cocclusion_context_pre_sub_graph(unsigned long long image_base);

		unsigned long long find_cocclusion_context_post_sub_graph(unsigned long long image_base);

		unsigned long long find_cdxgi_swapchain_present_dwm(unsigned long long image_base);

		unsigned long long find_cdxgi_swapchain_present_multiplane_overlay(unsigned long long image_base);

		unsigned long long find_cdxgi_swapchain_dwm_legacy_present_dwm(unsigned long long image_base);

		unsigned long long find_mm_allocate_independent_pages();

		unsigned long long find_mm_free_independent_pages();

		unsigned long long find_mm_set_page_protection();
	

		
	}
}