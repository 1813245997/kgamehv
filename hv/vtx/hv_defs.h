#pragma once

#define  _MINIMAL  1


#define QUERY_CPUID_BIT(x, b)        ((x) & (1 << b))
#define SET_CPUID_BIT(x, b)            (x = (x) | (1 << b))
#define CLR_CPUID_BIT(x, b)            ((x) & ~(1 << b))

#define CPUID_EXTENDED_FEATURES           0x00000007
#define CPUID_HV_VENDOR_AND_MAX_FUNCTIONS 0x40000000
#define CPUID_HV_INTERFACE                0x40000001
#define CPUID_PROCESSOR_FEATURES          0x00000001
#define CPU_BASED_MONITOR_TRAP_FLAG 0x08000000


#define MASK_EPT_PML1_OFFSET(_VAR_) ((unsigned __int64)_VAR_ & 0xFFFULL)
#define MASK_EPT_PML1_INDEX(_VAR_) ((_VAR_ & 0x1FF000ULL) >> 12)
#define MASK_EPT_PML2_INDEX(_VAR_) ((_VAR_ & 0x3FE00000ULL) >> 21)
#define MASK_EPT_PML3_INDEX(_VAR_) ((_VAR_ & 0x7FC0000000ULL) >> 30)
#define MASK_EPT_PML4_INDEX(_VAR_) ((_VAR_ & 0xFF8000000000ULL) >> 39)

#define LARGE_PAGE_SIZE 0x200000
#define GET_PFN(_VAR_) (_VAR_ >> PAGE_SHIFT)

#define VMCALL_IDENTIFIER 0xCDAEFAEDBBAEBEEF

#define VMM_TAG 'vhra'
#define VMM_STACK_SIZE 0x6000



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

#define NON_CANONICIAL_ADDRESS_END 0xFFFF800000000000
#define NON_CANONICIAL_ADDRESS_START 0x0000800000000000