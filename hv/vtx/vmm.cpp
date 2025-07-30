#pragma warning( disable :  4201)
 
#include "../utils/global_defs.h"
#include <ntddk.h>
#include <intrin.h>
#include "common.h"
#include "cpuid.h"
#include "../asm\vm_context.h"
#include "cr.h"
#include "msr.h"
#include "vmcs.h"
#include "hypervisor_routines.h"
 
#include "vmcs_encodings.h"
#include "allocators.h"
#include "trap-frame.h"
#include "exception-routines.h"
#include "bit_wise.h"
#include "CompatibilityChecks.h"
#include "vt_global.h"
LIST_ENTRY g_ept_breakpoint_hook_list{};
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


/// <summary>
/// Allocates contiguous memory for vmcs
/// </summary>
/// <param name="vcpu"> Pointer to vcpu </param>
/// <returns> status </returns>
bool init_vmcs(__vcpu* vcpu)
{
	__vmx_basic_msr vmx_basic = { 0 };
	PHYSICAL_ADDRESS physical_max;

	vmx_basic.all = __readmsr(IA32_VMX_BASIC);

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
	vcpu->vmcs->header.revision_identifier = vmx_basic.vmcs_revision_identifier;

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
	__vmx_basic_msr vmx_basic = { 0 };

	vmx_basic.all = __readmsr(IA32_VMX_BASIC);

	if (vmx_basic.vmxon_region_size > PAGE_SIZE)
		vcpu->vmxon = allocate_contignous_memory<__vmcs*>(PAGE_SIZE);

	else
		vcpu->vmxon = allocate_contignous_memory<__vmcs*>(vmx_basic.vmxon_region_size);

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
	vcpu->vmxon->header.all = vmx_basic.vmcs_revision_identifier;
	vcpu->vmxon->header.shadow_vmcs_indicator = 0;

	return true;
}
bool enable_vmx_operation(__vcpu  *  vcpu)
{
	 
	 
	 
	_disable();
	__cr0 cr0{};
	__cr4 cr4{};
	cr0.all = __readcr0();
	cr4 .all = __readcr4();

	// 3.23.7
	cr4.all |= CR4_VMX_ENABLE_FLAG;

	// 3.23.8
	cr0.all |= vcpu->cached.vmx_cr0_fixed0;
	cr0.all &= vcpu->cached.vmx_cr0_fixed1;
	cr4.all |= vcpu->cached.vmx_cr4_fixed0;
	cr4.all &= vcpu->cached.vmx_cr4_fixed1;

	
	__writecr0(cr0.all);
	__writecr4(cr4.all);

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
/// Adjust cr4 and cr0 for turning on vmx
/// </summary>
void adjust_control_registers()
{
	__cr4 cr4;
	__cr0 cr0;
	__cr_fixed cr_fixed;

	cr_fixed.all = __readmsr(IA32_VMX_CR0_FIXED0);
	cr0.all = __readcr0();
	cr0.all |= cr_fixed.split.low;
	cr_fixed.all = __readmsr(IA32_VMX_CR0_FIXED1);
	cr0.all &= cr_fixed.split.low;
	__writecr0(cr0.all);
	cr_fixed.all = __readmsr(IA32_VMX_CR4_FIXED0);
	cr4.all = __readcr4();
	cr4.all |= cr_fixed.split.low;
	cr_fixed.all = __readmsr(IA32_VMX_CR4_FIXED1);
	cr4.all &= cr_fixed.split.low;
	__writecr4(cr4.all);

	__ia32_feature_control_msr feature_msr = { 0 };
	feature_msr.all = __readmsr(IA32_FEATURE_CONTROL);

	if (feature_msr.lock == 0) 
	{
		feature_msr.vmxon_outside_smx = 1;
		feature_msr.lock = 1;

		__writemsr(IA32_FEATURE_CONTROL, feature_msr.all);
	}
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
	
	prepare_external_structures(vcpu);
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
			SetBit(i, (unsigned long*)invalid_msr_bitmap);
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
			SetBit(index, (unsigned long*)fault_bitmap);
		}
	}

	return fault_bitmap;
}
/// <summary>
/// Initialize and launch vmm
/// </summary>
/// <returns> status </returns>
bool vmm_init()
{
	if (allocate_vmm_context() == false)
		return false;

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

	 g_invalid_msr_bitmap = allocate_invalid_msr_bitmap();
	if ( g_invalid_msr_bitmap == NULL)
	{
		return FALSE;
	}

	 g_invalid_synthetic_msr_bitmap = allocate_synthetic_msr_fault_bitmap();
	if ( g_invalid_msr_bitmap == NULL)
	{
		return FALSE;
	}
	 
	//
	// Call derefered procedure call (DPC) to fill vmcs and launch vmm for every logical core
	KeGenericCallDpc(dpc_broadcast_initialize_guest, 0);
	return true;
}


// get the value of CR0 that the guest believes is active.
// this is a mixture of the guest CR0 and the CR0 read shadow.
  cr0 read_effective_guest_cr0() {
	// TODO: cache this value
	auto const mask = hv::vmread (VMCS_CTRL_CR0_GUEST_HOST_MASK);

	// bits set to 1 in the mask are read from CR0, otherwise from the shadow
	cr0 cr0;
	cr0.flags = (hv::vmread(VMCS_CTRL_CR0_READ_SHADOW) & mask)
		| (hv::vmread(VMCS_GUEST_CR0) & ~mask);

	return cr0;
}

// get the value of CR4 that the guest believes is active.
// this is a mixture of the guest CR4 and the CR4 read shadow.
  cr4 read_effective_guest_cr4() {
	// TODO: cache this value
	auto const mask = hv::vmread(VMCS_CTRL_CR4_GUEST_HOST_MASK);

	// bits set to 1 in the mask are read from CR4, otherwise from the shadow
	cr4 cr4;
	cr4.flags = (hv::vmread(VMCS_CTRL_CR4_READ_SHADOW) & mask)
		| (hv::vmread(VMCS_GUEST_CR4) & ~mask);

	return cr4;
}

  uint16_t current_guest_cpl() {
	  vmx_segment_access_rights ss;
	  ss.flags = static_cast<uint32_t>(hv::vmread(VMCS_GUEST_SS_ACCESS_RIGHTS));
	  return ss.descriptor_privilege_level;
  }

  ia32_vmx_procbased_ctls_register read_ctrl_proc_based() {
	  ia32_vmx_procbased_ctls_register value;
	  value.flags = hv::vmread(VMCS_CTRL_PROCESSOR_BASED_VM_EXECUTION_CONTROLS);
	  return value;
  }

    void write_ctrl_proc_based(ia32_vmx_procbased_ctls_register const value) {
	  hv::vmwrite(VMCS_CTRL_PROCESSOR_BASED_VM_EXECUTION_CONTROLS, value.flags);
  }

  void inject_nmi()
  {
	  vmentry_interrupt_information interrupt_info;
	  interrupt_info.flags = 0;
	  interrupt_info.vector = nmi;
	  interrupt_info.interruption_type = non_maskable_interrupt;
	  interrupt_info.deliver_error_code = 0;
	  interrupt_info.valid = 1;
	  hv::vmwrite(VMCS_CTRL_VMENTRY_INTERRUPTION_INFORMATION_FIELD, interrupt_info.flags);
  }



  namespace hv
  {
	  void handle_host_interrupt(trap_frame* const frame) {
		  switch (frame->vector) {
			  // host NMIs
		  case nmi: {
			  auto ctrl = read_ctrl_proc_based();
			  ctrl.nmi_window_exiting = 1;
			  write_ctrl_proc_based(ctrl);

			  auto const cpu = reinterpret_cast<__vcpu*>(_readfsbase_u64());
			  ++cpu->queued_nmis;

			  break;
		  }
				  // host exceptions
		  default: {
			  // no registered exception handler
			  if (!frame->r10 || !frame->r11) {
				 /* HV_LOG_ERROR("Unhandled exception. RIP=hv.sys+%p. Vector=%u.",
					  frame->rip - reinterpret_cast<UINT64>(&__ImageBase), frame->vector);*/

				  // ensure a triple-fault
				  segment_descriptor_register_64 idtr;
				  idtr.base_address = frame->rsp;
				  idtr.limit = 0xFFF;
				  __lidt(&idtr);

				  break;
			  }

			/*  HV_LOG_HOST_EXCEPTION("Handling host exception. RIP=hv.sys+%p. Vector=%u",
				  frame->rip - reinterpret_cast<UINT64>(&__ImageBase), frame->vector);*/

			  // jump to the exception handler
			  frame->rip = frame->r10;

			  auto const e = reinterpret_cast<hv::host_exception_info*>(frame->r11);

			  e->exception_occurred = true;
			  e->vector = frame->vector;
			  e->error = frame->error;

			  // slightly helps prevent infinite exceptions
			  frame->r10 = 0;
			  frame->r11 = 0;
		  }
		  }
	  }

  }

 