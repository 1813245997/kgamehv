#pragma warning( disable :  4201)
 
#include "../utils/global_defs.h"
#include "vmm.h"


bool init_vcpu(__vcpu*& vcpu);

bool init_vmxon(__vcpu* vcpu);

bool init_vmcs(__vcpu* vcpu);

bool allocate_vmm_context();

void dpc_broadcast_initialize_guest(KDPC* Dpc, PVOID DeferredContext, PVOID SystemArgument1, PVOID SystemArgument2);


bool vmm_init()
{
	if (allocate_vmm_context() == false)
	{
		return false;
	}

	if (init_system_data() == false)
	{
		return false;
	}


	prepare_host_page_tables();


	//
	// Initalize vcpu for each logical core
	for (unsigned int iter = 0; iter < g_vmm_context->processor_count; iter++)
	{
		if (init_vcpu(g_vmm_context->vcpu_table[iter]) == false)
			return false;

		if (init_vmxon(g_vmm_context->vcpu_table[iter]) == false)
			return false;

		if (init_vmcs(g_vmm_context->vcpu_table[iter]) == false)
			return false;
	}

	compatibility_check_perform_checks();

	/* g_invalid_msr_bitmap = allocate_invalid_msr_bitmap();
	if ( g_invalid_msr_bitmap == NULL)
	{
		return FALSE;
	}

	 g_invalid_synthetic_msr_bitmap = allocate_synthetic_msr_fault_bitmap();
	if ( g_invalid_msr_bitmap == NULL)
	{
		return FALSE;
	}*/

	//
	// Call derefered procedure call (DPC) to fill vmcs and launch vmm for every logical core
	KeGenericCallDpc(dpc_broadcast_initialize_guest, 0);
	return true;
}

void dpc_broadcast_initialize_guest(KDPC* Dpc, PVOID DeferredContext, PVOID SystemArgument1, PVOID SystemArgument2)
{
	UNREFERENCED_PARAMETER(DeferredContext);
	UNREFERENCED_PARAMETER(Dpc);
	vmx_save_state();

	// Wait for all DPCs to synchronize at this point
	KeSignalCallDpcSynchronize(SystemArgument2);

	// Mark the DPC as being complete
	KeSignalCallDpcDone(SystemArgument1);
}


/// <summary>
/// Deallocate all structures
/// </summary>
void free_vmm_context()
{
	if (g_vmm_context != nullptr)
	{
		// POOL MANAGER
		if (g_vmm_context->pool_manager != nullptr)
		{
			pool_manager::uninitialize();
			free_pool(g_vmm_context->pool_manager);
		}

		// VCPU TABLE
		if (g_vmm_context->vcpu_table != nullptr)
		{
			for (unsigned int i = 0; i < g_vmm_context->processor_count; i++)
			{
				// VCPU
				if (g_vmm_context->vcpu_table[i] != nullptr)
				{
					// VCPU VMXON REGION
					if (g_vmm_context->vcpu_table[i]->vmxon != nullptr)
					{
						free_contignous_memory(g_vmm_context->vcpu_table[i]->vmxon);
					}

					// VCPU VMCS REGION
					if (g_vmm_context->vcpu_table[i]->vmcs != nullptr)
					{
						free_contignous_memory(g_vmm_context->vcpu_table[i]->vmcs);
					}

					// VCPU VMM STACK
					if (g_vmm_context->vcpu_table[i]->vmm_stack != nullptr)
					{
						free_pool(g_vmm_context->vcpu_table[i]->vmm_stack);
					}

					// MSR BITMAP
					if (g_vmm_context->vcpu_table[i]->vcpu_bitmaps.msr_bitmap != nullptr)
					{
						free_pool(g_vmm_context->vcpu_table[i]->vcpu_bitmaps.msr_bitmap);
					}

					// IO BITMAP A
					if (g_vmm_context->vcpu_table[i]->vcpu_bitmaps.io_bitmap_a != nullptr)
					{
						free_pool(g_vmm_context->vcpu_table[i]->vcpu_bitmaps.io_bitmap_a);
					}

					// IO BITMAP B
					if (g_vmm_context->vcpu_table[i]->vcpu_bitmaps.io_bitmap_b != nullptr)
					{
						free_pool(g_vmm_context->vcpu_table[i]->vcpu_bitmaps.io_bitmap_b);
					}

					// EPT_STATE
					if (g_vmm_context->vcpu_table[i]->ept_state != nullptr)
					{
						// EPT POINTER
						if (g_vmm_context->vcpu_table[i]->ept_state->ept_pointer != nullptr)
						{
							free_pool(g_vmm_context->vcpu_table[i]->ept_state->ept_pointer);
						}
						// EPT PAGE TABLE
						if (g_vmm_context->vcpu_table[i]->ept_state->ept_page_table != nullptr)
						{
							free_contignous_memory(g_vmm_context->vcpu_table[i]->ept_state->ept_page_table);
						}

						free_pool(g_vmm_context->vcpu_table[i]->ept_state);
					}

					free_pool(g_vmm_context->vcpu_table[i]);
				}
			}
			free_pool(g_vmm_context->vcpu_table);
		}

		free_pool(g_vmm_context);
	}

	g_vmm_context = 0;
}

 
bool init_vmcs(__vcpu* vcpu)
{
	ia32_vmx_basic_register vmx_basic = { 0 };
	PHYSICAL_ADDRESS physical_max;

	vmx_basic.flags= __readmsr(IA32_VMX_BASIC);

	physical_max.QuadPart = ~0ULL;
	vcpu->vmcs = allocate_contignous_memory<__vmcs*>(PAGE_SIZE);
	if (vcpu->vmcs == NULL)
	{
		LogError("Vmcs structure could not be allocated");
		return false;
	}

	vcpu->vmcs_physical = MmGetPhysicalAddress(vcpu->vmcs).QuadPart;
	if (vcpu->vmcs_physical == NULL)
	{
		LogError("Could not get physical address of vmcs");
		return false;
	}

	RtlSecureZeroMemory(vcpu->vmcs, PAGE_SIZE);
	vcpu->vmcs->header.revision_identifier = vmx_basic.vmcs_revision_id;

	// Indicates if it's shadow vmcs or not
	vcpu->vmcs->header.shadow_vmcs_indicator = 0;

	return true;
}

/// <summary>
/// Allocate whole vmm context, build mtrr map, initalize pool manager and initialize ept structure 
/// </summary>
/// <returns> status </returns>
bool allocate_vmm_context()
{
	__cpuid_info cpuid_reg = { 0 };

	//
	// Allocate Nonpaged memory for vm global context structure
	//
	g_vmm_context = allocate_pool<__vmm_context>();
	if (g_vmm_context == nullptr) {
		LogError("g_vmm_context could not be allocated");
		return false;
	}
	RtlSecureZeroMemory(g_vmm_context, sizeof(__vmm_context));

	//
	// Allocate virtual cpu context for every logical core
	//
	 
	g_vmm_context->processor_count = utils::internal_functions::pfn_ke_query_active_processor_count_ex(ALL_PROCESSOR_GROUPS);
	g_vmm_context->vcpu_table = allocate_pool<__vcpu**>(sizeof(__vcpu*) * (g_vmm_context->processor_count));
	if (g_vmm_context->vcpu_table == nullptr)
	{
		LogError("vcpu_table could not be allocated");
		return false;
	}
	RtlSecureZeroMemory(g_vmm_context->vcpu_table, sizeof(__vcpu*) * (g_vmm_context->processor_count));




	//
	// Build mtrr map for physcial memory caching informations
	//
	ept::build_mtrr_map();

	if (pool_manager::initialize() == false)
	{
		return false;
	}



	g_vmm_context->hv_presence = true;

	__cpuid((int*)&cpuid_reg.eax, 0);
	g_vmm_context->highest_basic_leaf = cpuid_reg.eax;

	return true;
}

/// <summary>
/// Allocate whole vcpu context and bitmaps
/// </summary>
/// <returns> Pointer to vcpu </returns>
bool init_vcpu(__vcpu*& vcpu)
{
	InitializeListHead(&g_ept_breakpoint_hook_list);
	vcpu = allocate_pool<__vcpu>();
	if (vcpu == nullptr)
	{
		LogError("vcpu structure could not be allocated");
		return false;
	}
	RtlSecureZeroMemory(vcpu, sizeof(__vcpu));

	vcpu->vmm_stack = allocate_pool<void*>(VMM_STACK_SIZE);
	if (vcpu->vmm_stack == nullptr)
	{
		LogError("vmm stack could not be allocated");
		return false;
	}
	RtlSecureZeroMemory(vcpu->vmm_stack, VMM_STACK_SIZE);

	vcpu->vcpu_bitmaps.msr_bitmap = allocate_pool<unsigned __int8*>(PAGE_SIZE);
	if (vcpu->vcpu_bitmaps.msr_bitmap == nullptr)
	{
		LogError("msr bitmap could not be allocated");
		return false;
	}
	RtlSecureZeroMemory(vcpu->vcpu_bitmaps.msr_bitmap, PAGE_SIZE);
	vcpu->vcpu_bitmaps.msr_bitmap_physical = MmGetPhysicalAddress(vcpu->vcpu_bitmaps.msr_bitmap).QuadPart;

	vcpu->vcpu_bitmaps.io_bitmap_a = allocate_pool<unsigned __int8*>(PAGE_SIZE);
	if (vcpu->vcpu_bitmaps.io_bitmap_a == nullptr)
	{
		LogError("io bitmap a could not be allocated");
		return false;
	}
	RtlSecureZeroMemory(vcpu->vcpu_bitmaps.io_bitmap_a, PAGE_SIZE);
	vcpu->vcpu_bitmaps.io_bitmap_a_physical = MmGetPhysicalAddress(vcpu->vcpu_bitmaps.io_bitmap_a).QuadPart;

	vcpu->vcpu_bitmaps.io_bitmap_b = allocate_pool<unsigned __int8*>(PAGE_SIZE);
	if (vcpu->vcpu_bitmaps.io_bitmap_b == nullptr)
	{
		LogError("io bitmap b could not be allocated");
		return false;
	}
	RtlSecureZeroMemory(vcpu->vcpu_bitmaps.io_bitmap_b, PAGE_SIZE);
	vcpu->vcpu_bitmaps.io_bitmap_b_physical = MmGetPhysicalAddress(vcpu->vcpu_bitmaps.io_bitmap_b).QuadPart;

	//
	// Allocate ept state structure
	//
	vcpu->ept_state = allocate_pool<__ept_state>();
	if (vcpu->ept_state == nullptr)
	{
		LogError("ept state could not be allocated");
		return false;
	}
	RtlSecureZeroMemory(vcpu->ept_state, sizeof(__ept_state));
	InitializeListHead(&vcpu->ept_state->hooked_page_list);

	//
	// Initialize ept structure
	//
	if (ept::initialize(*vcpu->ept_state) == false)
	{
		return false;
	}

	LogInfo("vcpu entry allocated successfully at %llX", vcpu);

	return true;
}

/// <summary>
/// Initialize vmxon structure
/// </summary>
/// <param name="vcpu"> Pointer to vcpu </param>
/// <returns> status </returns>
bool init_vmxon(__vcpu* vcpu)
{
	ia32_vmx_basic_register vmx_basic = { 0 };

	vmx_basic.flags = __readmsr(IA32_VMX_BASIC);

	if (vmx_basic.vmcs_size_in_bytes > PAGE_SIZE)
		vcpu->vmxon = allocate_contignous_memory<__vmcs*>(PAGE_SIZE);

	else
		vcpu->vmxon = allocate_contignous_memory<__vmcs*>(vmx_basic.vmcs_size_in_bytes);

	if (vcpu->vmxon == nullptr)
	{
		LogError("vmxon could not be allocated");
		return false;
	}

	vcpu->vmxon_physical = MmGetPhysicalAddress(vcpu->vmxon).QuadPart;
	if (vcpu->vmxon_physical == 0)
	{
		LogError("Could not get vmxon physical address");
		return false;
	}

	RtlSecureZeroMemory(vcpu->vmxon, PAGE_SIZE);
	vcpu->vmxon->header.all = vmx_basic.vmcs_revision_id;
	vcpu->vmxon->header.shadow_vmcs_indicator = 0;

	return true;
}
bool enable_vmx_operation(__vcpu  *  vcpu)
{
	  
	_disable();
	cr0 cr0{};
	cr4 cr4{};
	cr0.flags = __readcr0();
	cr4 .flags = __readcr4();

	// 3.23.7
	cr4.flags |= CR4_VMX_ENABLE_FLAG;

	// 3.23.8
	cr0.flags |= vcpu->cached.vmx_cr0_fixed0;
	cr0.flags &= vcpu->cached.vmx_cr0_fixed1;
	cr4.flags |= vcpu->cached.vmx_cr4_fixed0;
	cr4.flags &= vcpu->cached.vmx_cr4_fixed1;

	
	__writecr0(cr0.flags);
	__writecr4(cr4.flags);

	_enable();

	if (vcpu->cached.feature_control.lock_bit == 0)
	{
		//需要从vmexit 里面隐藏这个位
		vcpu->cached.feature_control.enable_vmx_outside_smx = 1;
		vcpu->cached.feature_control.lock_bit = 1;
		
		__writemsr(IA32_FEATURE_CONTROL, vcpu->cached.feature_control.flags);
	}

	return true;
}
 


/// <summary>
/// Initialize logical core and launch virtual machine managed by current vmcs
/// </summary>
/// <param name="guest_rsp"></param>
void init_logical_processor(void* guest_rsp)
{
	unsigned __int64 processor_number = KeGetCurrentProcessorNumberEx(NULL);

	__vcpu* vcpu = g_vmm_context->vcpu_table[processor_number];
	cache_cpu_data(vcpu->cached);

	////这个地方会被检测
	//adjust_control_registers();
	 

	if (!enable_vmx_operation(vcpu)) {
		LogDebug("Failed to enable VMX operation.\n");
		return;
	}
	if (__vmx_on(&vcpu->vmxon_physical)) 
	{
		LogError("Failed to put vcpu %d into VMX operation.\n", processor_number);
		return;
	}

	vcpu->vcpu_status.vmx_on = true;
	LogInfo("vcpu %d is now in VMX operation.\n", processor_number);
	
	 
	fill_vmcs(vcpu, guest_rsp);
	vcpu->vcpu_status.vmm_launched = true;

	__vmx_vmlaunch();

	// We should never get here
	
	LogError("Vmlaunch failed");
	ASSERT(FALSE);
	vcpu->vcpu_status.vmm_launched = false;
	vcpu->vcpu_status.vmx_on = false;
	__vmx_off();
}

unsigned long long* allocate_invalid_msr_bitmap()
{
	// 分配 4KB 大小的位图（共 0x1000 个 MSR，每位对应一个）
	UINT64* invalid_msr_bitmap = (UINT64*)ExAllocatePoolWithTag(NonPagedPool, 0x1000 / 8, POOL_TAG);
	if (invalid_msr_bitmap == nullptr)
	{
		return nullptr;
	}

	// 清零整个 bitmap
	RtlSecureZeroMemory(invalid_msr_bitmap, 0x1000 / 8);

	// 枚举所有 MSR，如果读失败（#GP），则标记为无效
	for (UINT32 i = 0; i < 0x1000; ++i)
	{
		__try
		{
			__readmsr(i);
		}
		__except (EXCEPTION_EXECUTE_HANDLER)
		{
			hv::bit_set(i, (unsigned long*)invalid_msr_bitmap);
		}
	}

	return invalid_msr_bitmap;
}

unsigned long long* allocate_synthetic_msr_fault_bitmap()
{
	constexpr UINT32 msr_start = 0x40000000;
	constexpr UINT32 msr_end = 0x400000FF;
	constexpr UINT32 msr_count = msr_end - msr_start + 1;
	constexpr SIZE_T bitmap_size_bytes = msr_count / 8; // 256 bits = 32 bytes

	UINT64* fault_bitmap = (UINT64*)ExAllocatePoolWithTag(NonPagedPool, bitmap_size_bytes, POOL_TAG);
	if (!fault_bitmap)
		return nullptr;

	RtlSecureZeroMemory(fault_bitmap, bitmap_size_bytes);

	for (UINT32 msr = msr_start; msr <= msr_end; ++msr)
	{
		__try
		{
			__readmsr(msr);
		}
		__except (EXCEPTION_EXECUTE_HANDLER)
		{
			const UINT32 index = msr - msr_start;
			hv::bit_set(index, (unsigned long*)fault_bitmap);
		}
	}

	return fault_bitmap;
}
 


bool init_system_data()
{
	g_vmm_context->system_eprocess =  PsInitialSystemProcess ;
	g_vmm_context->system_cr3.flags= utils::process_utils::get_process_cr3(PsInitialSystemProcess);
	return true;
}


