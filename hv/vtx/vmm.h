#pragma once

#include "../ia32/ia32.hpp"
 
bool vmm_init();
 
bool init_system_data();

void free_vmm_context();

unsigned long long* allocate_invalid_msr_bitmap();

unsigned long long* allocate_synthetic_msr_fault_bitmap();

 
 