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

// get the value of CR0 that the guest believes is active.
// this is a mixture of the guest CR0 and the CR0 read shadow.
cr0 read_effective_guest_cr0();

// get the value of CR4 that the guest believes is active.
// this is a mixture of the guest CR4 and the CR4 read shadow.
cr4 read_effective_guest_cr4();

uint16_t current_guest_cpl();