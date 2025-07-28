#pragma once
/// <summary>
/// Initialize and launch vmm
/// </summary>
/// <returns> status </returns>
bool vmm_init();

/// <summary>
/// Deallocate all structures
/// </summary>
void free_vmm_context();

unsigned long long* allocate_invalid_msr_bitmap();

unsigned long long* allocate_synthetic_msr_fault_bitmap();

// get the value of CR0 that the guest believes is active.
// this is a mixture of the guest CR0 and the CR0 read shadow.
cr0 read_effective_guest_cr0();

// get the value of CR4 that the guest believes is active.
// this is a mixture of the guest CR4 and the CR4 read shadow.
cr4 read_effective_guest_cr4();

uint16_t current_guest_cpl();

ia32_vmx_procbased_ctls_register read_ctrl_proc_based();

void write_ctrl_proc_based(ia32_vmx_procbased_ctls_register const value);

void inject_nmi();