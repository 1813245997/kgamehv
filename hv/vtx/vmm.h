#pragma once

#include "../ia32/ia32.hpp"
 
bool vmm_init();

void free_vmm_context();

unsigned long long* allocate_invalid_msr_bitmap();

unsigned long long* allocate_synthetic_msr_fault_bitmap();

cr0 read_effective_guest_cr0();

cr4 read_effective_guest_cr4();

uint16_t current_guest_cpl();

ia32_vmx_procbased_ctls_register read_ctrl_proc_based();

void write_ctrl_proc_based(ia32_vmx_procbased_ctls_register const value);
 
bool vmx_get_current_execution_mode();