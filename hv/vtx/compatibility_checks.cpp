#include "../utils/global_defs.h"
#include "compatibility_checks.h"
 
 
 
void
cpuid_instruction_execute(uint32_t func, uint32_t subfunc, int* cpu_info)
{
	__cpuidex(cpu_info, func, subfunc);
}
 

BOOLEAN
compatibility_check_cpu_support_for_rtm()
{
    int     Regs1[4];
    int     Regs2[4];
    BOOLEAN Result;

    //
    // TSX is controlled via MSR_IA32_TSX_CTRL.  However, support for this
    // MSR is enumerated by ARCH_CAP_TSX_MSR bit in MSR_IA32_ARCH_CAPABILITIES.
    //
    // TSX control (aka MSR_IA32_TSX_CTRL) is only available after a
    // microcode update on CPUs that have their MSR_IA32_ARCH_CAPABILITIES
    // bit MDS_NO=1. CPUs with MDS_NO=0 are not planned to get
    // MSR_IA32_TSX_CTRL support even after a microcode update. Thus,
    // tsx= cmdline requests will do nothing on CPUs without
    // MSR_IA32_TSX_CTRL support.
    //

    cpuid_instruction_execute(0, 0, Regs1);
    cpuid_instruction_execute(7, 0, Regs2);

    //
    // Check RTM and MPX extensions in order to filter out TSX on Haswell CPUs
    //
    Result = Regs1[0] >= 0x7 && (Regs2[1] & 0x4800) == 0x4800;

    return Result;
}

 
uint32_t
compatibility_check_get_x86_virtual_address_width()
{
	int regs[4];

	cpuid_instruction_execute(CPUID_ADDR_WIDTH, 0, regs);

	//
	// ÌáÈ¡ eax µÄ bit 15:8
	//
	return ((regs[0] >> 8) & 0xff);
}


/**
 * @brief Get physical address width for x86 processors
 *
 * @return UINT32
 */
UINT32
compatibility_check_get_x86_physical_address_width()
{
    int Regs[4];

    cpuid_instruction_execute(CPUID_ADDR_WIDTH, 0, Regs);

    //
    // Extracting bit 7:0 from eax register
    //
    return (Regs[0] & 0x0ff);
}

/**
 * @brief Check for mode-based execution
 *
 * @return BOOLEAN
 */
BOOLEAN
compatibility_check_mode_based_execution()
{
    //
    // The PML address and PML index fields exist only on processors that support the 1-setting of
    // the "enable PML" VM - execution control
    //
    UINT32 SecondaryProcBasedVmExecControls = ajdust_controls(IA32_VMX_PROCBASED_CTLS2_MODE_BASED_EXECUTE_CONTROL_FOR_EPT_FLAG,
        IA32_VMX_PROCBASED_CTLS2);

    if (SecondaryProcBasedVmExecControls & IA32_VMX_PROCBASED_CTLS2_MODE_BASED_EXECUTE_CONTROL_FOR_EPT_FLAG)
    {
        //
        // The processor support PML
        //
        return TRUE;
    }
    else
    {
        //
        // Not supported
        //
        return FALSE;
    }
}

/**
 * @brief Check for Page Modification Logging (PML) support
 *
 * @return BOOLEAN
 */
BOOLEAN
compatibility_check_pml()
{
    //
    // The PML address and PML index fields exist only on processors that support the 1-setting of
    // the "enable PML" VM - execution control
    //
    UINT32 SecondaryProcBasedVmExecControls = ajdust_controls(IA32_VMX_PROCBASED_CTLS2_ENABLE_PML_FLAG, IA32_VMX_PROCBASED_CTLS2);

    if (SecondaryProcBasedVmExecControls & IA32_VMX_PROCBASED_CTLS2_ENABLE_PML_FLAG)
    {
        //
        // The processor support MBEC
        //
        return TRUE;
    }
    else
    {
        //
        // Not supported
        //
        return FALSE;
    }
}

/**
 * @brief Checks for the compatibility features based on current processor
 * @detail NOTE: NOT ALL OF THE CHECKS ARE PERFORMED HERE
 * @return VOID
 */
VOID
compatibility_check_perform_checks()
{
    //
    // Check if processor supports TSX (RTM)
    //
     g_compatibility_check.RtmSupport = compatibility_check_cpu_support_for_rtm();

    //
    // Get x86 processor width for virtual address
    //
     g_compatibility_check.VirtualAddressWidth = compatibility_check_get_x86_virtual_address_width();

    //
    // Get x86 processor width for physical address
    //
    g_compatibility_check.PhysicalAddressWidth = compatibility_check_get_x86_physical_address_width();

    //
    // Check Mode-based execution compatibility
    //
    g_compatibility_check.ModeBasedExecutionSupport = compatibility_check_mode_based_execution();

    //
    // Check PML support
    //
    g_compatibility_check.PmlSupport = compatibility_check_pml();

    //
    // Log for testing
    //
   LogDebug("Mode based execution: %s | PML: %s",
       g_compatibility_check.ModeBasedExecutionSupport ? "true" : "false",
        g_compatibility_check.PmlSupport ? "true" : "false");
}

BOOLEAN check_address_canonicality(UINT64 vaddr, PBOOLEAN is_kernel_address)
{
	UINT64 Addr = (UINT64)vaddr;
	UINT64 MaxVirtualAddrLowHalf, MinVirtualAddressHighHalf;

	//
	// Get processor's address width for VA
	//
	UINT32 AddrWidth =  g_compatibility_check.VirtualAddressWidth;

	//
	// get max address in lower-half canonical addr space
	// e.g. if width is 48, then 0x00007FFF_FFFFFFFF
	//
	MaxVirtualAddrLowHalf = ((UINT64)1ull << (AddrWidth - 1)) - 1;

	//
	// get min address in higher-half canonical addr space
	// e.g., if width is 48, then 0xFFFF8000_00000000
	//
	MinVirtualAddressHighHalf = ~MaxVirtualAddrLowHalf;

	//
	// Check to see if the address in a canonical address
	//
	if ((Addr > MaxVirtualAddrLowHalf) && (Addr < MinVirtualAddressHighHalf))
	{
		*is_kernel_address = FALSE;
		return FALSE;
	}

	//
	// Set whether it's a kernel address or not
	//
	if (MinVirtualAddressHighHalf < Addr)
	{
		*is_kernel_address = TRUE;
	}
	else
	{
		*is_kernel_address = FALSE;
	}

	return TRUE;
}