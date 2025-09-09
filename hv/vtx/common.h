#pragma once
#include <ntddk.h>
#include <xmmintrin.h>
#include "ept.h"
#include "spinlock.h"
#include "poolmanager.h"
#include "exception.h"
#include "mtrr.h"
#include "../ia32/ia32.hpp"
 
extern "C" size_t __fastcall LDE(const void* lpData, unsigned int size);

#define VMCALL_IDENTIFIER 0xCDAEFAEDBBAEBEEF

#define VMM_TAG 'vhra'
#define VMM_STACK_SIZE 0x6000

#define LARGE_PAGE_SIZE 0x200000
#define GET_PFN(_VAR_) (_VAR_ >> PAGE_SHIFT)

#define MASK_GET_HIGHER_32BITS(_ARG_)(_ARG_ & 0xffffffff00000000)
#define MASK_GET_LOWER_32BITS(_ARG_)(_ARG_ & 0xffffffff)
#define MASK_GET_LOWER_16BITS(_ARG_)(_ARG_ & 0xffff)
#define MASK_GET_LOWER_8BITS(_ARG_)(_ARG_ & 0xff)
#define MASK_32BITS 0xffffffff

/**
 * @brief Hypervisor reserved range for RDMSR and WRMSR
 *
 */
#define RESERVED_MSR_RANGE_LOW 0x40000000
#define RESERVED_MSR_RANGE_HI  0x400000F0

#define CPUID_ADDR_WIDTH 0x80000008

struct __vmexit_guest_registers
{
    __m128 xmm[6];
    unsigned __int64 r15;
    unsigned __int64 r14;                  
    unsigned __int64 r13;
    unsigned __int64 r12;                  
    unsigned __int64 r11;
    unsigned __int64 r10;                  
    unsigned __int64 r9;
    unsigned __int64 r8;                   
    unsigned __int64 rdi;
    unsigned __int64 rsi;
    unsigned __int64 rbp;
    unsigned __int64 rsp;
    unsigned __int64 rbx;
    unsigned __int64 rdx;                  
    unsigned __int64 rcx;
    unsigned __int64 rax;
};

struct __vmcs
{
    union
    {
        unsigned int all;
        struct
        {
            unsigned int revision_identifier : 31;
            unsigned int shadow_vmcs_indicator : 1;
        };
    } header;
    unsigned int abort_indicator;
    char data[0x1000 - 2 * sizeof(unsigned)];
};

struct vcpu_cached_data {
	// maximum number of bits in a physical address (MAXPHYSADDR)
	uint64_t max_phys_addr;

	// reserved bits in CR0/CR4
	uint64_t vmx_cr0_fixed0;
	uint64_t vmx_cr0_fixed1;
	uint64_t vmx_cr4_fixed0;
	uint64_t vmx_cr4_fixed1;

	// mask of unsupported processor state components for XCR0
	uint64_t xcr0_unsupported_mask;

	// IA32_FEATURE_CONTROL
	ia32_feature_control_register feature_control;
	ia32_feature_control_register guest_feature_control;

	// IA32_VMX_MISC
	ia32_vmx_misc_register vmx_misc;

	// CPUID 0x01
	cpuid_eax_01 cpuid_01;
};
// TODO: move to ia32?
struct vmx_msr_entry {
	uint32_t msr_idx;
	uint32_t _reserved;
	uint64_t msr_data;
};
struct __vcpu
{
    void* vmm_stack;

    __vmcs* vmcs;
    unsigned __int64 vmcs_physical;

    __vmcs* vmxon;
    unsigned __int64 vmxon_physical;

    struct __vmexit_info
    {
        __vmexit_guest_registers* guest_registers;

        unsigned __int64 guest_rip;

        rflags guest_rflags;

        unsigned __int64 instruction_length;

        unsigned __int64 reason;

        unsigned __int64 qualification;

        unsigned __int64 instruction_information;

    }vmexit_info;

    struct __vcpu_status
    {
        unsigned __int64 vmx_on;
        unsigned __int64 vmm_launched;
    }vcpu_status;

    struct __vmx_off_state
    {
        unsigned __int64  vmx_off_executed;
        unsigned __int64  guest_rip;
        unsigned __int64  guest_rsp;
    }vmx_off_state;

    struct __vcpu_bitmaps
    {
        unsigned __int8* msr_bitmap;
        unsigned __int64 msr_bitmap_physical;

        unsigned __int8* io_bitmap_a;
        unsigned __int64 io_bitmap_a_physical;

        unsigned __int8* io_bitmap_b;
        unsigned __int64 io_bitmap_b_physical;
    }vcpu_bitmaps;

	// vm-exit MSR store area
	struct alignas(0x10) {
		vmx_msr_entry tsc;
		vmx_msr_entry perf_global_ctrl;
		vmx_msr_entry aperf;
		vmx_msr_entry mperf;
	} msr_exit_store;

	// vm-entry MSR load area
	struct alignas(0x10) {
		vmx_msr_entry aperf;
		vmx_msr_entry mperf;
	} msr_entry_load;

    __ept_state* ept_state;
	// cached values that are assumed to NEVER change
	vcpu_cached_data cached;
	// the number of NMIs that need to be delivered
	uint32_t volatile queued_nmis;

	// current TSC offset
	uint64_t tsc_offset;

	// current preemption timer
	uint64_t preemption_timer;

	// the overhead caused by world-transitions
	uint64_t vm_exit_tsc_overhead;
    uint64_t vm_exit_last_tsc;
	uint64_t vm_exit_mperf_overhead;
	uint64_t vm_exit_ref_tsc_overhead;

 
 

	// whether to use TSC offsetting for the current vm-exit--false by default
	bool hide_vm_exit_overhead;

    bool is_on_vmx_root_mode;
};

struct __mtrr_info
{
    __mtrr_range_descriptor memory_range[100];
    unsigned __int32 enabled_memory_ranges;
    unsigned __int8 default_memory_type;
};

struct __vmm_context
{
    __vcpu** vcpu_table;
    pool_manager::__pool_manager* pool_manager;
    __mtrr_info mtrr_info;
    unsigned __int32 processor_count;
    unsigned __int32 highest_basic_leaf;
    bool hv_presence;
    bool initialized;
};

extern __vmm_context* g_vmm_context;

extern LIST_ENTRY g_ept_breakpoint_hook_list;


