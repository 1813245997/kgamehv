#pragma once
#include "../ia32/ia32.hpp"
#include "../vtx/invalidators.h"


#define MASK_EPT_PML1_OFFSET(_VAR_) ((unsigned __int64)_VAR_ & 0xFFFULL)
#define MASK_EPT_PML1_INDEX(_VAR_) ((_VAR_ & 0x1FF000ULL) >> 12)
#define MASK_EPT_PML2_INDEX(_VAR_) ((_VAR_ & 0x3FE00000ULL) >> 21)
#define MASK_EPT_PML3_INDEX(_VAR_) ((_VAR_ & 0x7FC0000000ULL) >> 30)
#define MASK_EPT_PML4_INDEX(_VAR_) ((_VAR_ & 0xFF8000000000ULL) >> 39)
#define CPU_BASED_MONITOR_TRAP_FLAG 0x08000000

union __eptp
{
	unsigned __int64 all;
	struct
	{
		unsigned __int64 memory_type : 3; 
		unsigned __int64 page_walk_length : 3;
		unsigned __int64 dirty_and_aceess_enabled : 1;
		unsigned __int64 reserved1 : 5; 
		unsigned __int64 pml4_address : 36;
		unsigned __int64 reserved2 : 16;
	};
};


// See Table 28-1. 
union __ept_pml4e
{
	unsigned __int64 all;
	struct
	{
		unsigned __int64 read : 1; // bit 0
		unsigned __int64 write : 1; // bit 1
		unsigned __int64 execute : 1; // bit 2
		unsigned __int64 reserved1 : 5; // bit 7:3 (Must be Zero)
		unsigned __int64 accessed : 1; // bit 8
		unsigned __int64 ignored1 : 1; // bit 9
		unsigned __int64 execute_for_usermode : 1; // bit 10
		unsigned __int64 ignored2 : 1; // bit 11
		unsigned __int64 physical_address : 36; // bit (N-1):12 or Page-Frame-Number
		unsigned __int64 reserved2 : 4; // bit 51:N
		unsigned __int64 ignored3 : 12; // bit 63:52
	};
};

// See Table 28-3
union __ept_pdpte
{
	unsigned __int64 all;
	struct
	{
		unsigned __int64 read : 1; // bit 0
		unsigned __int64 write : 1; // bit 1
		unsigned __int64 execute : 1; // bit 2
		unsigned __int64 reserved1 : 5; // bit 7:3 (Must be Zero)
		unsigned __int64 accessed : 1; // bit 8
		unsigned __int64 ignored1 : 1; // bit 9
		unsigned __int64 execute_for_usermode : 1; // bit 10
		unsigned __int64 ignored2 : 1; // bit 11
		unsigned __int64 physical_address : 36; // bit (N-1):12 or Page-Frame-Number
		unsigned __int64 reserved2 : 4; // bit 51:N
		unsigned __int64 ignored3 : 12; // bit 63:52
	};
};

// See Table 28-5
union __ept_pde {
	unsigned __int64 all;
	struct
	{
		unsigned __int64 read : 1; // bit 0
		unsigned __int64 write : 1; // bit 1
		unsigned __int64 execute : 1; // bit 2
		unsigned __int64 reserved1 : 5; // bit 7:3 (Must be Zero)
		unsigned __int64 accessed : 1; // bit 8
		unsigned __int64 ignored1 : 1; // bit 9
		unsigned __int64 execute_for_usermode : 1; // bit 10
		unsigned __int64 ignored2 : 1; // bit 11
		unsigned __int64 physical_address : 36; // bit (N-1):12 or Page-Frame-Number
		unsigned __int64 reserved2 : 4; // bit 51:N
		unsigned __int64 ignored3 : 12; // bit 63:52
	}large_page;
	struct
	{
		unsigned __int64 read : 1;
		unsigned __int64 write : 1;
		unsigned __int64 execute : 1;
		unsigned __int64 memory_type : 3;
		unsigned __int64 ignore_pat : 1;
		unsigned __int64 large_page : 1;
		unsigned __int64 accessed : 1;
		unsigned __int64 dirty : 1;
		unsigned __int64 execute_for_usermode : 1;
		unsigned __int64 reserved1 : 10;
		unsigned __int64 physical_address : 27;
		unsigned __int64 reserved2 : 15;
		unsigned __int64 suppressve : 1;
	}page_directory_entry;
};

// See Table 28-6																	 
union __ept_pte {
	unsigned __int64 all;
	struct
	{
		unsigned __int64 read : 1; // bit 0											 
		unsigned __int64 write : 1; // bit 1										 
		unsigned __int64 execute : 1; // bit 2
		unsigned __int64 ept_memory_type : 3; // bit 5:3 (EPT Memory type)
		unsigned __int64 ignore_pat : 1; // bit 6
		unsigned __int64 ignored1 : 1; // bit 7
		unsigned __int64 accessed_flag : 1; // bit 8	
		unsigned __int64 dirty_flag : 1; // bit 9
		unsigned __int64 execute_for_usermode : 1; // bit 10
		unsigned __int64 ignored2 : 1; // bit 11
		unsigned __int64 physical_address : 36; // bit (N-1):12 or Page-Frame-Number
		unsigned __int64 reserved : 4; // bit 51:N
		unsigned __int64 ignored3 : 11; // bit 62:52
		unsigned __int64 suppress_ve : 1; // bit 63
	};
};

struct __ept_dynamic_split
{
	DECLSPEC_ALIGN(PAGE_SIZE) __ept_pte pml1[512];

	__ept_pde* entry;

	LIST_ENTRY dynamic_split_list;
};

struct __vmm_ept_page_table
{
	DECLSPEC_ALIGN(PAGE_SIZE) __ept_pml4e pml4[512];

	DECLSPEC_ALIGN(PAGE_SIZE) __ept_pdpte pml3[512];

	DECLSPEC_ALIGN(PAGE_SIZE) __ept_pde pml2[512][512];
};


 





//typedef struct __ept_hooked_function_info 
//{
//	 
//	LIST_ENTRY hooked_function_list;
//	   
//	unsigned __int64 hook_size;
//
//	void* original_va;
//
//	void* fake_va;
//
//	void* new_handler_va;
//
//	unsigned long long original_pa;
//
//	unsigned long long fake_pa;
//
//	unsigned long long new_handler_pa;
//	 
//
//	unsigned __int8* trampoline_va;
//
//	unsigned __int8* fake_page_contents;
//
//	unsigned __int8* original_instructions_backup;
//
//	hook_type type;
//
//}ept_hooked_function_info;
//
//typedef struct __ept_hooked_page_info
//{ 
//
//	//
//	// Linked list entires for each page hook.
//	//
//	LIST_ENTRY hooked_page_list;
//
//	//
//	// Linked list entries for each function hook
//	//
//	LIST_ENTRY hooked_functions_list;
//
//
//	//
//// Page frame number of the hooked page. Used to find this structure in the list of page hooks
////
//	unsigned __int64 pfn_of_hooked_page;
//
//	//
//	// Page frame number of the page with fake contents. Used to swap page with fake contents
//	//
//	unsigned __int64 pfn_of_fake_page_contents;
//
//	//
//// Page with our hooked functions
////
//	unsigned __int8* fake_page_contents;
//	//
//	// The page entry in the page tables that this page is targetting.
//	//
//	__ept_pte* entry_address;
//
//	//
//	// The original page entry
//	// 
//	__ept_pte original_entry;
//
//	//
//	// The changed page entry
//	//
//	__ept_pte changed_entry;
//
//	HANDLE  process_id;
//
//	unsigned int  ref_count;
//
//	bool is_user_mode;
//
//
//} ept_hooked_page_info ;

//typedef struct __ept_hooked_page_info
//{
//	// 链表结构，用于维护多个被 Hook 页
//	LIST_ENTRY hooked_page_list;
//
//	// 被 Hook 的真实物理页帧号（PFN）
//	unsigned __int64 pfn_of_hooked_page;
//
//	// 替代页的 PFN（即我们伪造的页面所映射的物理页）
//	unsigned __int64 pfn_of_fake_page;
//
//	// EPT 页表中该页对应的 PTE 地址
//	__ept_pte* entry_address;
//
//	// 原始的 PTE 内容（用于后续恢复）
//	__ept_pte original_entry;
//
//	// 替换后的 PTE 内容（伪造访问映射）
//	__ept_pte changed_entry;
//
//} ept_hooked_page_info;
 
//
//typedef struct _ept_breakpoint_info
//{
//	LIST_ENTRY breakpoint_list_entry;        // 用于挂入断点列表
//
//	unsigned __int64 instruction_size;       // 被覆盖的指令长度（跳板长度）
//
//	void* original_va;                       // 原始地址
//	void* fake_va;                           // 伪造页地址
//	void* breakpoint_handler_va;             // 中断处理器地址
//
//	unsigned __int8* trampoline_va;          // 跳板地址（跳回原始逻辑）
//	unsigned __int8* fake_page_contents;     // 模拟页内容
//	unsigned __int8* original_instruction_backup; // 原始指令备份（用于还原）
//
//	hook_type type;                          // 类型：INT3 / EPT HOOK / ShadowPage等
//
//} ept_breakpoint_info;
//
//typedef struct _ept_debugged_page_info
//{
//	LIST_ENTRY debugged_page_list_entry;     // 页级别断点列表
//	LIST_ENTRY breakpoints_list_head;        // 当前页上所有断点（ept_breakpoint_info）
//
//	HANDLE process_id;                       // 所属进程 ID
//
//	unsigned __int64 pfn_of_hooked_page;
//
//	unsigned __int8* fake_page_contents;     // 伪造页内容
//
//	unsigned int ref_count;                  // 引用计数
//
//} ept_debugged_page_info;


union __ept_violation
{
	unsigned __int64 all;
	struct
	{
		/**
		 * [Bit 0] Set if the access causing the EPT violation was a data read.
		 */
		unsigned __int64 read_access : 1;

		/**
		 * [Bit 1] Set if the access causing the EPT violation was a data write.
		 */
		unsigned __int64 write_access : 1;

		/**
		 * [Bit 2] Set if the access causing the EPT violation was an instruction fetch.
		 */
		unsigned __int64 execute_access : 1;

		/**
		 * [Bit 3] The logical-AND of bit 0 in the EPT paging-structure entries used to translate the guest-physical address of the
		 * access causing the EPT violation (indicates whether the guest-physical address was readable).
		 */
		unsigned __int64 ept_readable : 1;

		/**
		 * [Bit 4] The logical-AND of bit 1 in the EPT paging-structure entries used to translate the guest-physical address of the
		 * access causing the EPT violation (indicates whether the guest-physical address was writeable).
		 */
		unsigned __int64 ept_writeable : 1;

		/**
		 * [Bit 5] The logical-AND of bit 2 in the EPT paging-structure entries used to translate the guest-physical address of the
		 * access causing the EPT violation.
		 * If the "mode-based execute control for EPT" VM-execution control is 0, this indicates whether the guest-physical address
		 * was executable. If that control is 1, this indicates whether the guest-physical address was executable for
		 * supervisor-mode linear addresses.
		 */
		unsigned __int64 ept_executable : 1;

		/**
		 * [Bit 6] If the "mode-based execute control" VM-execution control is 0, the value of this bit is undefined. If that
		 * control is 1, this bit is the logical-AND of bit 10 in the EPT paging-structures entries used to translate the
		 * guest-physical address of the access causing the EPT violation. In this case, it indicates whether the guest-physical
		 * address was executable for user-mode linear addresses.
		 */
		unsigned __int64 ept_executable_for_usermode : 1;

		/**
		 * [Bit 7] Set if the guest linear-address field is valid. The guest linear-address field is valid for all EPT violations
		 * except those resulting from an attempt to load the guest PDPTEs as part of the execution of the MOV CR instruction.
		 */
		unsigned __int64 valid_guest_linear_address : 1;

		/**
		 * [Bit 8] If bit 7 is 1:
		 * - Set if the access causing the EPT violation is to a guest-physical address that is the translation of a linear
		 * address.
		 * - Clear if the access causing the EPT violation is to a paging-structure entry as part of a page walk or the update of
		 * an accessed or dirty bit.
		 * Reserved if bit 7 is 0 (cleared to 0).
		 */
		unsigned __int64 caused_by_translation : 1;

		/**
		 * [Bit 9] This bit is 0 if the linear address is a supervisor-mode linear address and 1 if it is a user-mode linear
		 * address. Otherwise, this bit is undefined.
		 *
		 * @remarks If bit 7 is 1, bit 8 is 1, and the processor supports advanced VM-exit information for EPT violations. (If
		 *          CR0.PG = 0, the translation of every linear address is a user-mode linear address and thus this bit will be 1.)
		 */
		unsigned __int64 usermode_linear_address : 1;

		/**
		 * [Bit 10] This bit is 0 if paging translates the linear address to a read-only page and 1 if it translates to a
		 * read/write page. Otherwise, this bit is undefined
		 *
		 * @remarks If bit 7 is 1, bit 8 is 1, and the processor supports advanced VM-exit information for EPT violations. (If
		 *          CR0.PG = 0, every linear address is read/write and thus this bit will be 1.)
		 */
		unsigned __int64 readable_writable_page : 1;

		/**
		 * [Bit 11] This bit is 0 if paging translates the linear address to an executable page and 1 if it translates to an
		 * execute-disable page. Otherwise, this bit is undefined.
		 *
		 * @remarks If bit 7 is 1, bit 8 is 1, and the processor supports advanced VM-exit information for EPT violations. (If
		 *          CR0.PG = 0, CR4.PAE = 0, or MSR_IA32_EFER.NXE = 0, every linear address is executable and thus this bit will be 0.)
		 */
		unsigned __int64 execute_disable_page : 1;

		/**
		 * [Bit 12] NMI unblocking due to IRET.
		 */
		unsigned __int64 nmi_unblocking : 1;
		unsigned __int64 reserved1 : 51;
	};
};


typedef struct __ept_hooked_page_info
{
	// 链表结构，用于维护多个被 Hook 页
	LIST_ENTRY hooked_page_list;

	// 被 Hook 的真实物理页帧号（PFN）
	unsigned __int64 pfn_of_hooked_page;

	// 替代页的 PFN（即我们伪造的页面所映射的物理页）
	unsigned __int64 pfn_of_fake_page;

	// EPT 页表中该页对应的 PTE 地址
	__ept_pte* entry_address;

	// 原始的 PTE 内容（用于后续恢复）
	__ept_pte original_entry;

	// 替换后的 PTE 内容（伪造访问映射）
	__ept_pte changed_entry;

} ept_hooked_page_info;
 

struct __ept_state
{
	LIST_ENTRY hooked_page_list;
	__eptp* ept_pointer;
	__vmm_ept_page_table* ept_page_table;
	__ept_hooked_page_info* page_to_change;
};

namespace ept
{
	/// <summary>
	/// Build mtrr map to track physical memory type
	/// </summary>
	void build_mtrr_map();

	/// <summary>
	/// Initialize ept structure
	/// </summary>
	/// <returns></returns>
	bool initialize(__ept_state& ept_state);

	/// <summary>
	/// Change page physcial frame number and invalidate tlb
	/// </summary>
	/// <param name="entry_address"> Pointer to page table entry which we want to change </param>
	/// <param name="entry_value"> Pointer to page table entry which we want use to change </param>
	/// <param name="invalidate"> If true invalidates tlb after changning pte value </param>
	/// <param name="invalidation_type"> Specifiy if we want to invalidate single context or all contexts  </param>
	void swap_pml1_and_invalidate_tlb(__ept_state& ept_state, __ept_pte* entry_address, __ept_pte entry_value, invept_type invalidation_type);


	bool hook_page_pfn_ept(
		__ept_state& ept_state,
		unsigned __int64 pfn_of_target_page,
		unsigned __int64 pfn_of_fake_page
	);



	/// <summary>
	/// Split pml2 into 512 pml1 entries (From one 2MB page to 512 4KB pages)
	/// </summary>
	/// <param name="pre_allocated_buffer"> Pre allocated buffer for split </param>
	/// <param name="physical_address"></param>
	/// <returns> status </returns>
	bool split_pml2(__ept_state& ept_state, void* pre_allocated_buffer, unsigned __int64 physical_address);



	/// <summary>
/// Unhook all functions and invalidate tlb
/// </summary>
	//void unhook_all_functions(__ept_state& ept_state);



	/// <summary>
	/// Perfrom a hook
	/// </summary>
	/// <param name="target_address" > Address of function which we want to hook </param>
	/// <param name="hook_function"> Address of hooked version of function which we are hooking </param>
	/// <param name="(Optional) trampoline"> Address of codecave which is located in 2gb range of target function (Use only if you need smaller trampoline)</param>
	/// <param name="origin_function"> Address used to call original function </param>
	/// <returns></returns>
	//bool hook_kernel_function(_In_  __ept_state& ept_state, _In_  void* target_function, _In_  void* new_function, _Out_   void** origin_function);

	//bool hook_user_exception_int3(_In_  __ept_state& ept_state, _In_ HANDLE process_id, _In_  void* target_function, _In_  void* breakpoint_handler, _In_ uint8_t* trampoline_va);

	//bool hook_break_ponint_int3(_In_  __ept_state& ept_state, _In_  void* target_function, _In_  void* breakpoint_handler, _Out_ unsigned char* original_byte);


	//bool hook_kernel_break_point_int3(_In_  __ept_state& ept_state, _In_  void* target_function, _In_  void* breakpoint_handler, _Out_ unsigned char* original_byte);

	//bool hook_user_break_point_int3(_In_  __ept_state& ept_state, _In_  void* target_function, _In_  void* breakpoint_handler, _Out_ unsigned char* original_byte);

	//bool find_break_point_info(

	//	_In_ HANDLE process_id,
	//	_In_ hook_type type,
	//	_In_ void* target_function,
	//	_Out_ ept_breakpoint_info** out_hook_info);



	///// <summary>
	///// Unhook single function
	///// </summary>
	///// <param name="virtual_address"></param>
	///// <returns></returns>
	//bool unhook_function(__ept_state& ept_state, unsigned __int64 virtual_address);

	///// <summary>
	///// Unhook all functions for a given process ID
	///// </summary>
	///// <param name="ept_state"></param>
	///// <param name="process_id"></param>
	///// <returns></returns>
	//bool unhook_by_pid(__ept_state& ept_state, HANDLE processId);


	//bool unhook_process_all_user_exception(
	//	__ept_state& ept_state,
	//	HANDLE process_id

	//);
   
	/*bool hook_instruction_memory_int1(__ept_hooked_function_info* hooked_function_info, void* target_function, unsigned __int64 page_offset);

	bool hook_instruction_memory_int3(__ept_hooked_function_info* hooked_function_info, void* target_function,  unsigned __int64 page_offset);

	void hook_write_absolute_jump(unsigned __int8* target_buffer, unsigned __int64 destination_address);

	void  hook_write_absolute_jump_int3(unsigned __int8* target_buffer, unsigned __int64 destination_address);


	bool remove_breakpoints_by_type_for_process(
		_In_ HANDLE process_id,
		_In_ hook_type type);*/
	

}