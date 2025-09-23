#include "../utils/global_defs.h"
#include "hypercalls.h"

namespace hv
{

	namespace hc
	{
		volatile long vmcs_dump_lock = 0;

		void  test(__vcpu* vcpu)
		{
			UNREFERENCED_PARAMETER(vcpu);
			vcpu->vmexit_info.guest_registers->rax = true;
			adjust_rip(vcpu);
		}


			
		void vmoff(__vcpu* vcpu)
		{
			__writecr3(hv::vmread(GUEST_CR3));

			vcpu->vmx_off_state.guest_rip = vcpu->vmexit_info.guest_rip + vcpu->vmexit_info.instruction_length;
			vcpu->vmx_off_state.guest_rsp = vcpu->vmexit_info.guest_registers->rsp;
			vcpu->vmx_off_state.vmx_off_executed = true;

			__writemsr(IA32_FS_BASE, hv::vmread(GUEST_FS_BASE));
			__writemsr(IA32_GS_BASE, hv::vmread(GUEST_GS_BASE));
			__reload_gdtr(hv::vmread(GUEST_GDTR_BASE), hv::vmread(GUEST_GDTR_LIMIT));
			__reload_idtr(hv::vmread(GUEST_IDTR_BASE), hv::vmread(GUEST_IDTR_LIMIT));

			__vmx_off();

			__writecr4(__readcr4() & (~0x2000));
			adjust_rip(vcpu);
			 
		}


		void install_ept_hook(__vcpu* vcpu)
		{
			auto const orig_page_pfn = vcpu->vmexit_info.guest_registers-> rcx;
			auto const exec_page_pfn = vcpu->vmexit_info.guest_registers->rdx;

			ept::hook_page_pfn_ept(*vcpu->ept_state, orig_page_pfn, exec_page_pfn);

			adjust_rip(vcpu);
		}

		void remove_ept_hook(__vcpu* vcpu)
		{
			auto const orig_page_pfn = vcpu->vmexit_info.guest_registers->rcx;
			ept::unhook_page_pfn_ept(*vcpu->ept_state, orig_page_pfn);
			adjust_rip(vcpu);
		}

		void dump_pools_info(__vcpu* vcpu)
		{
			PLIST_ENTRY current = g_vmm_context->pool_manager->list_of_allocated_pools;

			spinlock pool_lock(&g_vmm_context->pool_manager->lock_for_reading_pool);

			DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, "-----------------------------------POOL MANAGER DUMP-----------------------------------\r\n");

			while (g_vmm_context->pool_manager->list_of_allocated_pools != current->Flink)
			{
				current = current->Flink;

				// Get the head of the record
				pool_manager:: __pool_table* pool_table = (pool_manager::__pool_table*)CONTAINING_RECORD(current, pool_manager::__pool_table, pool_list);

				LogDump("Address: 0x%X    Size: %llu    Intention: %s    Is Busy: %s    Recycled: %s",
					pool_table->address, pool_table->size, intention_to_string(pool_table->intention), pool_table->is_busy ? "Yes" : "No",
					pool_table->recycled ? "Yes" : "No");
			}

			DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, "-----------------------------------POOL MANAGER DUMP-----------------------------------\r\n");
			adjust_rip(vcpu);
		}

		void dump_vmcs(__vcpu* vcpu)
		{
			spinlock dump_lock(&vmcs_dump_lock);

			DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, "-----------------------------------VMCS CORE %u DUMP-----------------------------------\r\n", KeGetCurrentProcessorIndex());

			// Natural Guest Register State Fields
			LogDump("GUEST_CR0: 0x%llX", vmread(GUEST_CR0));
			LogDump("GUEST_CR3: 0x%llX", vmread(GUEST_CR3));
			LogDump("GUEST_CR4: 0x%llX", vmread(GUEST_CR4));
			LogDump("GUEST_ES_BASE: 0x%llX", vmread(GUEST_ES_BASE));
			LogDump("GUEST_CS_BASE: 0x%llX", vmread(GUEST_CS_BASE));
			LogDump("GUEST_SS_BASE: 0x%llX", vmread(GUEST_SS_BASE));
			LogDump("GUEST_DS_BASE: 0x%llX", vmread(GUEST_DS_BASE));
			LogDump("GUEST_FS_BASE: 0x%llX", vmread(GUEST_FS_BASE));
			LogDump("GUEST_GS_BASE: 0x%llX", vmread(GUEST_GS_BASE));
			LogDump("GUEST_LDTR_BASE: 0x%llX", vmread(GUEST_LDTR_BASE));
			LogDump("GUEST_TR_BASE: 0x%llX", vmread(GUEST_TR_BASE));
			LogDump("GUEST_GDTR_BASE: 0x%llX", vmread(GUEST_GDTR_BASE));
			LogDump("GUEST_IDTR_BASE: 0x%llX", vmread(GUEST_IDTR_BASE));
			LogDump("GUEST_DR7: 0x%llX", vmread(GUEST_DR7));
			LogDump("GUEST_RSP: 0x%llX", vmread(GUEST_RSP));
			LogDump("GUEST_RIP: 0x%llX", vmread(GUEST_RIP));
			LogDump("GUEST_RFLAGS: 0x%llX", vmread(GUEST_RFLAGS));
			LogDump("GUEST_SYSENTER_ESP: 0x%llX", vmread(GUEST_SYSENTER_ESP));
			LogDump("GUEST_SYSENTER_EIP: 0x%llX", vmread(GUEST_SYSENTER_EIP));
			LogDump("GUEST_S_CET: 0x%llX", vmread(GUEST_S_CET));
			LogDump("GUEST_SSP: 0x%llX", vmread(GUEST_SSP));
			LogDump("GUEST_INTERRUPT_SSP_TABLE_ADDR: 0x%llX", vmread(GUEST_INTERRUPT_SSP_TABLE_ADDR));

			// 64-bit Guest Register State Fields
			LogDump("GUEST_VMCS_LINK_POINTER: 0x%llX", vmread(GUEST_VMCS_LINK_POINTER));
			LogDump("GUEST_DEBUG_CONTROL: 0x%llX", vmread(GUEST_DEBUG_CONTROL));
			LogDump("GUEST_PAT: 0x%llX", vmread(GUEST_PAT));
			LogDump("GUEST_EFER: 0x%llX", vmread(GUEST_EFER));
			LogDump("GUEST_PERF_GLOBAL_CONTROL: 0x%llX", vmread(GUEST_PERF_GLOBAL_CONTROL));
			LogDump("GUEST_PDPTE0: 0x%llX", vmread(GUEST_PDPTE0));
			LogDump("GUEST_PDPTE1: 0x%llX", vmread(GUEST_PDPTE1));
			LogDump("GUEST_PDPTE2: 0x%llX", vmread(GUEST_PDPTE2));
			LogDump("GUEST_PDPTE3: 0x%llX", vmread(GUEST_PDPTE3));
			LogDump("GUEST_BNDCFGS: 0x%llX", vmread(GUEST_BNDCFGS));
			LogDump("GUEST_RTIT_CTL: 0x%llX", vmread(GUEST_RTIT_CTL));
			LogDump("GUEST_PKRS: 0x%llX", vmread(GUEST_PKRS));

			// 32-Bit Guest Register State Fields
			LogDump("GUEST_ES_LIMIT: 0x%llX", vmread(GUEST_ES_LIMIT));
			LogDump("GUEST_CS_LIMIT: 0x%llX", vmread(GUEST_CS_LIMIT));
			LogDump("GUEST_SS_LIMIT: 0x%llX", vmread(GUEST_SS_LIMIT));
			LogDump("GUEST_DS_LIMIT: 0x%llX", vmread(GUEST_DS_LIMIT));
			LogDump("GUEST_FS_LIMIT: 0x%llX", vmread(GUEST_FS_LIMIT));
			LogDump("GUEST_GS_LIMIT: 0x%llX", vmread(GUEST_GS_LIMIT));
			LogDump("GUEST_LDTR_LIMIT: 0x%llX", vmread(GUEST_LDTR_LIMIT));
			LogDump("GUEST_TR_LIMIT: 0x%llX", vmread(GUEST_TR_LIMIT));
			LogDump("GUEST_GDTR_LIMIT: 0x%llX", vmread(GUEST_GDTR_LIMIT));
			LogDump("GUEST_IDTR_LIMIT: 0x%llX", vmread(GUEST_IDTR_LIMIT));
			LogDump("GUEST_ES_ACCESS_RIGHTS: 0x%llX", vmread(GUEST_ES_ACCESS_RIGHTS));
			LogDump("GUEST_CS_ACCESS_RIGHTS: 0x%llX", vmread(GUEST_CS_ACCESS_RIGHTS));
			LogDump("GUEST_SS_ACCESS_RIGHTS: 0x%llX", vmread(GUEST_SS_ACCESS_RIGHTS));
			LogDump("GUEST_DS_ACCESS_RIGHTS: 0x%llX", vmread(GUEST_DS_ACCESS_RIGHTS));
			LogDump("GUEST_FS_ACCESS_RIGHTS: 0x%llX", vmread(GUEST_FS_ACCESS_RIGHTS));
			LogDump("GUEST_GS_ACCESS_RIGHTS: 0x%llX", vmread(GUEST_GS_ACCESS_RIGHTS));
			LogDump("GUEST_LDTR_ACCESS_RIGHTS: 0x%llX", vmread(GUEST_LDTR_ACCESS_RIGHTS));
			LogDump("GUEST_TR_ACCESS_RIGHTS: 0x%llX", vmread(GUEST_TR_ACCESS_RIGHTS));
			LogDump("GUEST_INTERRUPTIBILITY_STATE: 0x%llX", vmread(GUEST_INTERRUPTIBILITY_STATE));
			LogDump("GUEST_ACTIVITY_STATE: 0x%llX", vmread(GUEST_ACTIVITY_STATE));
			LogDump("GUEST_SMBASE: 0x%llX", vmread(GUEST_SMBASE));
			LogDump("GUEST_SYSENTER_CS: 0x%llX", vmread(GUEST_SYSENTER_CS));
			LogDump("GUEST_VMX_PREEMPTION_TIMER_VALUE: 0x%llX", vmread(GUEST_VMX_PREEMPTION_TIMER_VALUE));

			// 16-Bit Guest Register State Fields
			LogDump("GUEST_ES_SELECTOR: 0x%llX", vmread(GUEST_ES_SELECTOR));
			LogDump("GUEST_CS_SELECTOR: 0x%llX", vmread(GUEST_CS_SELECTOR));
			LogDump("GUEST_SS_SELECTOR: 0x%llX", vmread(GUEST_SS_SELECTOR));
			LogDump("GUEST_DS_SELECTOR: 0x%llX", vmread(GUEST_DS_SELECTOR));
			LogDump("GUEST_FS_SELECTOR: 0x%llX", vmread(GUEST_FS_SELECTOR));
			LogDump("GUEST_GS_SELECTOR: 0x%llX", vmread(GUEST_GS_SELECTOR));
			LogDump("GUEST_LDTR_SELECTOR: 0x%llX", vmread(GUEST_LDTR_SELECTOR));
			LogDump("GUEST_TR_SELECTOR: 0x%llX", vmread(GUEST_TR_SELECTOR));
			LogDump("GUEST_GUEST_INTERRUPT_STATUS: 0x%llX", vmread(GUEST_GUEST_INTERRUPT_STATUS));
			LogDump("GUEST_PML_INDEX: 0x%llX", vmread(GUEST_PML_INDEX));

			// Natural Host Register State Fields
			LogDump("HOST_CR0: 0x%llX", vmread(HOST_CR0));
			LogDump("HOST_CR3: 0x%llX", vmread(HOST_CR3));
			LogDump("HOST_CR4: 0x%llX", vmread(HOST_CR4));
			LogDump("HOST_FS_BASE: 0x%llX", vmread(HOST_FS_BASE));
			LogDump("HOST_GS_BASE: 0x%llX", vmread(HOST_GS_BASE));
			LogDump("HOST_TR_BASE: 0x%llX", vmread(HOST_TR_BASE));
			LogDump("HOST_GDTR_BASE: 0x%llX", vmread(HOST_GDTR_BASE));
			LogDump("HOST_IDTR_BASE: 0x%llX", vmread(HOST_IDTR_BASE));
			LogDump("HOST_SYSENTER_ESP: 0x%llX", vmread(HOST_SYSENTER_ESP));
			LogDump("HOST_SYSENTER_EIP: 0x%llX", vmread(HOST_SYSENTER_EIP));
			LogDump("HOST_RSP: 0x%llX", vmread(HOST_RSP));
			LogDump("HOST_RIP: 0x%llX", vmread(HOST_RIP));
			LogDump("HOST_S_CET: 0x%llX", vmread(HOST_S_CET));
			LogDump("HOST_SSP: 0x%llX", vmread(HOST_SSP));
			LogDump("HOST_INTERRUPT_SSP_TABLE_ADDR: 0x%llX", vmread(HOST_INTERRUPT_SSP_TABLE_ADDR));

			// 64-bit Host Register State Fields
			LogDump("HOST_PAT: 0x%llX", vmread(HOST_PAT));
			LogDump("HOST_EFER: 0x%llX", vmread(HOST_EFER));
			LogDump("HOST_PERF_GLOBAL_CTRL: 0x%llX", vmread(HOST_PERF_GLOBAL_CTRL));
			LogDump("HOST_PKRS: 0x%llX", vmread(HOST_PKRS));

			// 32-bit Host Register State Fields
			LogDump("HOST_SYSENTER_CS: 0x%llX", vmread(HOST_SYSENTER_CS));

			// 16-bit Host Register State Fields
			LogDump("HOST_ES_SELECTOR: 0x%llX", vmread(HOST_ES_SELECTOR));
			LogDump("HOST_CS_SELECTOR: 0x%llX", vmread(HOST_CS_SELECTOR));
			LogDump("HOST_SS_SELECTOR: 0x%llX", vmread(HOST_SS_SELECTOR));
			LogDump("HOST_DS_SELECTOR: 0x%llX", vmread(HOST_DS_SELECTOR));
			LogDump("HOST_FS_SELECTOR: 0x%llX", vmread(HOST_FS_SELECTOR));
			LogDump("HOST_GS_SELECTOR: 0x%llX", vmread(HOST_GS_SELECTOR));
			LogDump("HOST_TR_SELECTOR: 0x%llX", vmread(HOST_TR_SELECTOR));

			// Natural Control Register State Fields
			LogDump("CONTROL_CR0_GUEST_HOST_MASK: 0x%llX", vmread(CONTROL_CR0_GUEST_HOST_MASK));
			LogDump("CONTROL_CR4_GUEST_HOST_MASK: 0x%llX", vmread(CONTROL_CR4_GUEST_HOST_MASK));
			LogDump("CONTROL_CR0_READ_SHADOW: 0x%llX", vmread(CONTROL_CR0_READ_SHADOW));
			LogDump("CONTROL_CR4_READ_SHADOW: 0x%llX", vmread(CONTROL_CR4_READ_SHADOW));
			LogDump("CONTROL_CR3_TARGET_VALUE_0: 0x%llX", vmread(CONTROL_CR3_TARGET_VALUE_0));
			LogDump("CONTROL_CR3_TARGET_VALUE_1: 0x%llX", vmread(CONTROL_CR3_TARGET_VALUE_1));
			LogDump("CONTROL_CR3_TARGET_VALUE_2: 0x%llX", vmread(CONTROL_CR3_TARGET_VALUE_2));
			LogDump("CONTROL_CR3_TARGET_VALUE_3: 0x%llX", vmread(CONTROL_CR3_TARGET_VALUE_3));

			// 64-bit Control Register State Fields
			LogDump("CONTROL_BITMAP_IO_A_ADDRESS: 0x%llX", vmread(CONTROL_BITMAP_IO_A_ADDRESS));
			LogDump("CONTROL_BITMAP_IO_B_ADDRESS: 0x%llX", vmread(CONTROL_BITMAP_IO_B_ADDRESS));
			LogDump("CONTROL_MSR_BITMAPS_ADDRESS: 0x%llX", vmread(CONTROL_MSR_BITMAPS_ADDRESS));
			LogDump("CONTROL_VMEXIT_MSR_STORE_ADDRESS: 0x%llX", vmread(CONTROL_VMEXIT_MSR_STORE_ADDRESS));
			LogDump("CONTROL_VMEXIT_MSR_LOAD_ADDRESS: 0x%llX", vmread(CONTROL_VMEXIT_MSR_LOAD_ADDRESS));
			LogDump("CONTROL_VMENTER_MSR_LOAD_ADDRESS: 0x%llX", vmread(CONTROL_VMENTER_MSR_LOAD_ADDRESS));
			LogDump("CONTROL_VMCS_EXECUTIVE_POINTER: 0x%llX", vmread(CONTROL_VMCS_EXECUTIVE_POINTER));
			LogDump("CONTROL_PML_ADDRESS: 0x%llX", vmread(CONTROL_PML_ADDRESS));
			LogDump("CONTROL_TSC_OFFSET: 0x%llX", vmread(CONTROL_TSC_OFFSET));
			LogDump("CONTROL_VIRTUAL_APIC_ADDRESS: 0x%llX", vmread(CONTROL_VIRTUAL_APIC_ADDRESS));
			LogDump("CONTROL_APIC_ACCESS_ADDRESS: 0x%llX", vmread(CONTROL_APIC_ACCESS_ADDRESS));
			LogDump("CONTROL_POSTED_INTERRUPT_DESCRIPTOR_ADDRESS: 0x%llX", vmread(CONTROL_POSTED_INTERRUPT_DESCRIPTOR_ADDRESS));
			LogDump("CONTROL_VM_FUNCTION_CONTROLS: 0x%llX", vmread(CONTROL_VM_FUNCTION_CONTROLS));
			LogDump("CONTROL_EPT_POINTER: 0x%llX", vmread(CONTROL_EPT_POINTER));
			LogDump("CONTROL_EOI_EXIT_BITMAP_0: 0x%llX", vmread(CONTROL_EOI_EXIT_BITMAP_0));
			LogDump("CONTROL_EOI_EXIT_BITMAP_1: 0x%llX", vmread(CONTROL_EOI_EXIT_BITMAP_1));
			LogDump("CONTROL_EOI_EXIT_BITMAP_2: 0x%llX", vmread(CONTROL_EOI_EXIT_BITMAP_2));
			LogDump("CONTROL_EOI_EXIT_BITMAP_3: 0x%llX", vmread(CONTROL_EOI_EXIT_BITMAP_3));
			LogDump("CONTROL_EPTP_LIST_ADDRESS: 0x%llX", vmread(CONTROL_EPTP_LIST_ADDRESS));
			LogDump("CONTROL_VMREAD_BITMAP_ADDRESS: 0x%llX", vmread(CONTROL_VMREAD_BITMAP_ADDRESS));
			LogDump("CONTROL_VMWRITE_BITMAP_ADDRESS: 0x%llX", vmread(CONTROL_VMWRITE_BITMAP_ADDRESS));
			LogDump("CONTROL_VIRTUALIZATION_EXCEPTION_INFORMATION_ADDRESS: 0x%llX", vmread(CONTROL_VIRTUALIZATION_EXCEPTION_INFORMATION_ADDRESS));
			LogDump("CONTROL_XSS_EXITING_BITMAP: 0x%llX", vmread(CONTROL_XSS_EXITING_BITMAP));
			LogDump("CONTROL_ENCLS_EXITING_BITMAP: 0x%llX", vmread(CONTROL_ENCLS_EXITING_BITMAP));
			LogDump("CONTROL_SUB_PAGE_PERMISSION_TABLE_POINTER: 0x%llX", vmread(CONTROL_SUB_PAGE_PERMISSION_TABLE_POINTER));
			LogDump("CONTROL_TSC_MULTIPLIER: 0x%llX", vmread(CONTROL_TSC_MULTIPLIER));
			LogDump("CONTROL_ENCLV_EXITING_BITMAP: 0x%llX", vmread(CONTROL_ENCLV_EXITING_BITMAP));

			// 32-bit Control Register State Fields
			LogDump("CONTROL_PIN_BASED_VM_EXECUTION_CONTROLS: 0x%llX", vmread(CONTROL_PIN_BASED_VM_EXECUTION_CONTROLS));
			LogDump("CONTROL_PRIMARY_PROCESSOR_BASED_VM_EXECUTION_CONTROLS: 0x%llX", vmread(CONTROL_PRIMARY_PROCESSOR_BASED_VM_EXECUTION_CONTROLS));
			LogDump("CONTROL_EXCEPTION_BITMAP: 0x%llX", vmread(CONTROL_EXCEPTION_BITMAP));
			LogDump("CONTROL_PAGE_FAULT_ERROR_CODE_MASK: 0x%llX", vmread(CONTROL_PAGE_FAULT_ERROR_CODE_MASK));
			LogDump("CONTROL_PAGE_FAULT_ERROR_CODE_MATCH: 0x%llX", vmread(CONTROL_PAGE_FAULT_ERROR_CODE_MATCH));
			LogDump("CONTROL_CR3_TARGET_COUNT: 0x%llX", vmread(CONTROL_CR3_TARGET_COUNT));
			LogDump("CONTROL_VM_EXIT_CONTROLS: 0x%llX", vmread(CONTROL_VM_EXIT_CONTROLS));
			LogDump("CONTROL_VM_EXIT_MSR_STORE_COUNT: 0x%llX", vmread(CONTROL_VM_EXIT_MSR_STORE_COUNT));
			LogDump("CONTROL_VM_EXIT_MSR_LOAD_COUNT: 0x%llX", vmread(CONTROL_VM_EXIT_MSR_LOAD_COUNT));
			LogDump("CONTROL_VM_ENTRY_CONTROLS: 0x%llX", vmread(CONTROL_VM_ENTRY_CONTROLS));
			LogDump("CONTROL_VM_ENTRY_MSR_LOAD_COUNT: 0x%llX", vmread(CONTROL_VM_ENTRY_MSR_LOAD_COUNT));
			LogDump("CONTROL_VM_ENTRY_INTERRUPTION_INFORMATION_FIELD: 0x%llX", vmread(CONTROL_VM_ENTRY_INTERRUPTION_INFORMATION_FIELD));
			LogDump("CONTROL_VM_ENTRY_EXCEPTION_ERROR_CODE: 0x%llX", vmread(CONTROL_VM_ENTRY_EXCEPTION_ERROR_CODE));
			LogDump("CONTROL_VM_ENTRY_INSTRUCTION_LENGTH: 0x%llX", vmread(CONTROL_VM_ENTRY_INSTRUCTION_LENGTH));
			LogDump("CONTROL_TPR_THRESHOLD: 0x%llX", vmread(CONTROL_TPR_THRESHOLD));
			LogDump("CONTROL_SECONDARY_PROCESSOR_BASED_VM_EXECUTION_CONTROLS: 0x%llX", vmread(CONTROL_SECONDARY_PROCESSOR_BASED_VM_EXECUTION_CONTROLS));
			LogDump("CONTROL_PLE_GAP: 0x%llX", vmread(CONTROL_PLE_GAP));
			LogDump("CONTROL_PLE_WINDOW: 0x%llX", vmread(CONTROL_PLE_WINDOW));

			// 16-bit Control Register State Fields
			LogDump("CONTROL_VIRTUAL_PROCESSOR_IDENTIFIER: 0x%llX", vmread(CONTROL_VIRTUAL_PROCESSOR_IDENTIFIER));
			LogDump("CONTROL_POSTED_INTERRUPT_NOTIFICATION_VECTOR: 0x%llX", vmread(CONTROL_POSTED_INTERRUPT_NOTIFICATION_VECTOR));
			LogDump("CONTROL_EPTP_INDEX: 0x%llX", vmread(CONTROL_EPTP_INDEX));

			// Natural Read only Register State Fields
			LogDump("EXIT_QUALIFICATION: 0x%llX", vmread(EXIT_QUALIFICATION));
			LogDump("IO_RCX: 0x%llX", vmread(IO_RCX));
			LogDump("IO_RSI: 0x%llX", vmread(IO_RSI));
			LogDump("IO_RDI: 0x%llX", vmread(IO_RDI));
			LogDump("IO_RIP: 0x%llX", vmread(IO_RIP));
			LogDump("GUEST_LINEAR_ADDRESS: 0x%llX", vmread(GUEST_LINEAR_ADDRESS));

			// 64-bit Read only Register State Fields
			LogDump("GUEST_PHYSICAL_ADDRESS: 0x%llX", vmread(GUEST_PHYSICAL_ADDRESS));

			// 32-bit Read only Register State Fields
			LogDump("VM_INSTRUCTION_ERROR: 0x%llX", vmread(VM_INSTRUCTION_ERROR));
			LogDump("EXIT_REASON: 0x%llX", vmread(EXIT_REASON));
			LogDump("VM_EXIT_INTERRUPTION_INFORMATION: 0x%llX", vmread(VM_EXIT_INTERRUPTION_INFORMATION));
			LogDump("VM_EXIT_INTERRUPTION_ERROR_CODE: 0x%llX", vmread(VM_EXIT_INTERRUPTION_ERROR_CODE));
			LogDump("IDT_VECTORING_INFORMATION_FIELD: 0x%llX", vmread(IDT_VECTORING_INFORMATION_FIELD));
			LogDump("IDT_VECTORING_ERROR_CODE: 0x%llX", vmread(IDT_VECTORING_ERROR_CODE));
			LogDump("VM_EXIT_INSTRUCTION_LENGTH: 0x%llX", vmread(VM_EXIT_INSTRUCTION_LENGTH));
			LogDump("VM_EXIT_INSTRUCTION_INFORMATION: 0x%llX", vmread(VM_EXIT_INSTRUCTION_INFORMATION));

			DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, "-----------------------------------VMCS CORE %u DUMP-----------------------------------\r\n", KeGetCurrentProcessorIndex());
			adjust_rip(vcpu);

		}
			
		void read_phys_mem(__vcpu* vcpu)
		{
			auto const ctx =  vcpu->vmexit_info.guest_registers;

			auto const dst = reinterpret_cast<uint8_t*>(ctx->rcx);
			auto const src = host_physical_memory_base + ctx->rdx;
			auto const size = ctx->r8;

			size_t bytes_read = 0;

			while (bytes_read < size) {
				size_t dst_remaining = 0;

				// translate the guest buffer into hypervisor space
				auto const curr_dst = gva2hva(dst + bytes_read, &dst_remaining);

				if (!curr_dst) {
					// guest virtual address that caused the fault
				 
					__writecr2(reinterpret_cast<uint64_t>(dst + bytes_read));
					page_fault_exception error;
					error.flags = 0;
					error.present = 0;
					error.write = 1;
					error.user_mode_access = (current_guest_cpl() == 3);

					hv::inject_interruption(page_fault, INTERRUPT_TYPE_HARDWARE_EXCEPTION, error.flags, true);
					 
					return;
				}

				auto const curr_size = min(dst_remaining, size - bytes_read);

				host_exception_info e;
				memcpy_safe(e, curr_dst, src + bytes_read, curr_size);

				if (e.exception_occurred) {

					hv::inject_interruption(general_protection, INTERRUPT_TYPE_HARDWARE_EXCEPTION, 0, true);
					return;
				}

				bytes_read += curr_size;
			}

			ctx->rax = bytes_read;
			adjust_rip(vcpu);
		}

		void write_phys_mem(__vcpu* vcpu)
		{
			auto const ctx = vcpu->vmexit_info.guest_registers;

			// arguments
			auto const dst = host_physical_memory_base + ctx->rcx;
			auto const src = reinterpret_cast<uint8_t*>(ctx->rdx);
			auto const size = ctx->r8;

			size_t bytes_read = 0;

			while (bytes_read < size) {
				size_t src_remaining = 0;

				// translate the guest buffer into hypervisor space
				auto const curr_src = gva2hva(src + bytes_read, &src_remaining);

				if (!curr_src) {
					// guest virtual address that caused the fault
					__writecr2(reinterpret_cast<uint64_t>(dst + bytes_read));

					page_fault_exception error;
					error.flags = 0;
					error.present = 0;
					error.write = 0;
					error.user_mode_access = (current_guest_cpl() == 3);

					hv::inject_interruption(page_fault, INTERRUPT_TYPE_HARDWARE_EXCEPTION, error.flags, true);
					return;
				}

				auto const curr_size = min(size - bytes_read, src_remaining);

				host_exception_info e;
				memcpy_safe(e, dst + bytes_read, curr_src, curr_size);

				if (e.exception_occurred) {
					hv::inject_interruption(general_protection, INTERRUPT_TYPE_HARDWARE_EXCEPTION, 0, true);
					return;
				}

				bytes_read += curr_size;
			}

			ctx->rax = bytes_read;
			adjust_rip(vcpu);
		}

		void read_virt_mem(__vcpu* vcpu)
		{
			auto const ctx = vcpu->vmexit_info.guest_registers;
			// arguments
			cr3 guest_cr3 = g_vmm_context->system_cr3;
			if (ctx->rcx)
				guest_cr3.flags = ctx->rcx;

			auto const dst = reinterpret_cast<uint8_t*>(ctx->rdx);
			auto const src = reinterpret_cast<uint8_t*>(ctx->r8);
			auto const size = ctx->r9;

			size_t bytes_read = 0;

			while (bytes_read < size) {
				size_t dst_remaining = 0, src_remaining = 0;

				// translate the guest virtual addresses into host virtual addresses.
				// this has to be done 1 page at a time. :(
				auto const curr_dst = gva2hva(dst + bytes_read, &dst_remaining);
				auto const curr_src = gva2hva(guest_cr3, src + bytes_read, &src_remaining);

				if (!curr_dst) {
					// guest virtual address that caused the fault
					__writecr2(reinterpret_cast<uint64_t>(dst + bytes_read));

					page_fault_exception error;
					error.flags = 0;
					error.present = 0;
					error.write = 1;
					error.user_mode_access = (current_guest_cpl() == 3);

					hv::inject_interruption(page_fault, INTERRUPT_TYPE_HARDWARE_EXCEPTION, error.flags, true);
					return;
				}

				// this means that the target memory isn't paged in. there's nothing
				// we can do about that since we're not currently in that process's context.
				if (!curr_src)
					break;

				// the maximum allowed size that we can read at once with the translated HVAs
				auto const curr_size = min(size - bytes_read, min(dst_remaining, src_remaining));

				host_exception_info e;
				memcpy_safe(e, curr_dst, curr_src, curr_size);

				if (e.exception_occurred) {
					// this REALLY shouldn't happen... ever...
					hv::inject_interruption(general_protection, INTERRUPT_TYPE_HARDWARE_EXCEPTION, 0, true);
					return;
				}

				bytes_read += curr_size;
			}

			ctx->rax = bytes_read;
			adjust_rip(vcpu);
		}

		void write_virt_mem(__vcpu* vcpu)
		{
			auto const ctx = vcpu->vmexit_info.guest_registers;
			cr3 guest_cr3 = g_vmm_context->system_cr3;
			if (ctx->rcx)
				guest_cr3.flags = ctx->rcx;

			auto const dst = reinterpret_cast<uint8_t*>(ctx->rdx);
			auto const src = reinterpret_cast<uint8_t*>(ctx->r8);
			auto const size = ctx->r9;

			size_t bytes_read = 0;

			while (bytes_read < size) {
				size_t dst_remaining = 0, src_remaining = 0;

				// translate the guest virtual addresses into host virtual addresses.
				// this has to be done 1 page at a time. :(
				auto const curr_dst = gva2hva(guest_cr3, dst + bytes_read, &dst_remaining);
				auto const curr_src = gva2hva(src + bytes_read, &src_remaining);

				if (!curr_src) {
					// guest virtual address that caused the fault
					__writecr2(reinterpret_cast<uint64_t>(src + bytes_read));

					page_fault_exception error;
					error.flags = 0;
					error.present = 0;
					error.write = 0;
					error.user_mode_access = (current_guest_cpl() == 3);

					hv::inject_interruption(page_fault, INTERRUPT_TYPE_HARDWARE_EXCEPTION, error.flags, true);
					return;
				}

				// this means that the target memory isn't paged in. there's nothing
				// we can do about that since we're not currently in that process's context.
				if (!curr_dst)
					break;

				// the maximum allowed size that we can read at once with the translated HVAs
				auto const curr_size = min(size - bytes_read, min(dst_remaining, src_remaining));

				host_exception_info e;
				memcpy_safe(e, curr_dst, curr_src, curr_size);

				if (e.exception_occurred) {
					// this REALLY shouldn't happen... ever...
					hv::inject_interruption(general_protection, INTERRUPT_TYPE_HARDWARE_EXCEPTION, 0, true);
					return;
				}

				bytes_read += curr_size;
			}

			ctx->rax = bytes_read;
			adjust_rip(vcpu);
		}

		void query_process_cr3(__vcpu* vcpu)
		{
			auto const ctx = vcpu->vmexit_info.guest_registers;
			auto const target_pid = ctx->rcx;
		
			// System process
			if (target_pid == 4) {
				ctx->rax = g_vmm_context->system_cr3.flags;
				adjust_rip(vcpu);
				return;
			}
		
			ctx->rax = 0;
		
			// ActiveProcessLinks is right after UniqueProcessId in memory
			auto const apl_offset = utils::feature_offset::g_eprocess_unique_process_id_offset + 8;
			auto const head = reinterpret_cast<uint8_t*>(
				reinterpret_cast<uint64_t>(g_vmm_context->system_eprocess) + apl_offset
			);
			auto curr_entry = head;
		
			// Iterate over every EPROCESS in the ActiveProcessLinks linked list
			do {
				// Get the next entry in the linked list
				if (sizeof(curr_entry) != read_guest_virtual_memory(
						g_vmm_context->system_cr3,
						curr_entry + offsetof(LIST_ENTRY, Flink),
						&curr_entry,
						sizeof(curr_entry))) {
					break;
				}
		
				// EPROCESS
				auto const process = curr_entry - apl_offset;
		
				// EPROCESS::UniqueProcessId
				uint64_t pid = 0;
				if (sizeof(pid) != read_guest_virtual_memory(
						g_vmm_context->system_cr3,
						process + utils::feature_offset::g_eprocess_unique_process_id_offset,
						&pid,
						sizeof(pid))) {
					break;
				}
		
				// We found the target process
				if (target_pid == pid) {
					// EPROCESS::DirectoryTableBase
					uint64_t cr3 = 0;
					if (sizeof(cr3) != read_guest_virtual_memory(
							g_vmm_context->system_cr3,
							process + 0x28,
							&cr3,
							sizeof(cr3))) {
						break;
					}
		
					ctx->rax = cr3;
					break;
				}
			} while (curr_entry != head);
		
			adjust_rip(vcpu);
		}
		
	}

}