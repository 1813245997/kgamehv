#pragma once
#include "common.h"

 
 
  
 
#define IA32_LSTAR 0xC0000082
#define SYNTHETHIC_MSR_LOW 0x40000000
#define SYNTHETHIC_MSR_HI  0x400000F0
#define MSR_MASK_LOW ((unsigned __int64)(unsigned __int32) - 1)

union __msr
{
    unsigned __int64 all;
    struct
    {
        unsigned __int32 low;
        unsigned __int32 high;
    };
};

union __ia32_efer_t
{
    unsigned __int64 all;
    struct
    {
        unsigned __int64 syscall_enable : 1;
        unsigned __int64 reserved_0 : 7;
        unsigned __int64 long_mode_enable : 1;
        unsigned __int64 reserved_1 : 1;
        unsigned __int64 long_mode_active : 1;
        unsigned __int64 execute_disable : 1;
        unsigned __int64 reserved_2 : 52;
    };
};

union __ia32_feature_control_msr
{
    unsigned __int64 all;
    struct
    {
        unsigned __int64 lock : 1;
        unsigned __int64 vmxon_inside_smx : 1;
        unsigned __int64 vmxon_outside_smx : 1;
        unsigned __int64 reserved_0 : 5;
        unsigned __int64 senter_local : 6;
        unsigned __int64 senter_global : 1;
        unsigned __int64 reserved_1 : 1;
        unsigned __int64 sgx_launch_control_enable : 1;
        unsigned __int64 sgx_global_enable : 1;
        unsigned __int64 reserved_2 : 1;
        unsigned __int64 lmce : 1;
        unsigned __int64 system_reserved : 42;
    };
};

union __vmx_misc_msr_t
{
    unsigned __int64 all;
    struct
    {
        unsigned __int64 vmx_preemption_tsc_rate : 5;
        unsigned __int64 store_lma_in_vmentry_control : 1;
        unsigned __int64 activate_state_bitmap : 3;
        unsigned __int64 reserved_0 : 5;
        unsigned __int64 pt_in_vmx : 1;
        unsigned __int64 rdmsr_in_smm : 1;
        unsigned __int64 cr3_target_value_count : 9;
        unsigned __int64 max_msr_vmexit : 3;
        unsigned __int64 allow_smi_blocking : 1;
        unsigned __int64 vmwrite_to_any : 1;
        unsigned __int64 interrupt_mod : 1;
        unsigned __int64 reserved_1 : 1;
        unsigned __int64 mseg_revision_identifier : 32;
    };
};

union __vmx_basic_msr
{
    unsigned __int64 all;
    struct
    {
        unsigned __int64 vmcs_revision_identifier : 31;
        unsigned __int64 always_0 : 1;
        unsigned __int64 vmxon_region_size : 13;
        unsigned __int64 reserved_1 : 3;
        unsigned __int64 vmxon_physical_address_width : 1;
        unsigned __int64 dual_monitor_smi : 1;
        unsigned __int64 memory_type : 4;
        unsigned __int64 io_instruction_reporting : 1;
        unsigned __int64 true_controls : 1;
    };
};