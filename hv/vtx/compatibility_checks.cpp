#include "../utils/global_defs.h"
#include "compatibility_checks.h"
#include "vt_global.h"
 
#include "vmcs.h"
 


/**
 * @brief Check whether the processor supports RTM or not
 *
 * @return BOOLEAN
 */

VOID
CommonCpuidInstruction(UINT32 Func, UINT32 SubFunc, int* CpuInfo)
{
    __cpuidex(CpuInfo, Func, SubFunc);
}

BOOLEAN
CompatibilityCheckCpuSupportForRtm()
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

    CommonCpuidInstruction(0, 0, Regs1);
    CommonCpuidInstruction(7, 0, Regs2);

    //
    // Check RTM and MPX extensions in order to filter out TSX on Haswell CPUs
    //
    Result = Regs1[0] >= 0x7 && (Regs2[1] & 0x4800) == 0x4800;

    return Result;
}

/**
 * @brief Get virtual address width for x86 processors
 *
 * @return UINT32
 */
UINT32
CompatibilityCheckGetX86VirtualAddressWidth()
{
    int Regs[4];

    CommonCpuidInstruction(CPUID_ADDR_WIDTH, 0, Regs);

    //
    // Extracting bit 15:8 from eax register
    //
    return ((Regs[0] >> 8) & 0x0ff);
}

/**
 * @brief Get physical address width for x86 processors
 *
 * @return UINT32
 */
UINT32
CompatibilityCheckGetX86PhysicalAddressWidth()
{
    int Regs[4];

    CommonCpuidInstruction(CPUID_ADDR_WIDTH, 0, Regs);

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
CompatibilityCheckModeBasedExecution()
{
    //
    // The PML address and PML index fields exist only on processors that support the 1-setting of
    // the "enable PML" VM - execution control
    //
    UINT32 SecondaryProcBasedVmExecControls = hv::ajdust_controls(IA32_VMX_PROCBASED_CTLS2_MODE_BASED_EXECUTE_CONTROL_FOR_EPT_FLAG,
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
CompatibilityCheckPml()
{
    //
    // The PML address and PML index fields exist only on processors that support the 1-setting of
    // the "enable PML" VM - execution control
    //
    UINT32 SecondaryProcBasedVmExecControls = hv::ajdust_controls(IA32_VMX_PROCBASED_CTLS2_ENABLE_PML_FLAG, IA32_VMX_PROCBASED_CTLS2);

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
    g_compatibility_check.RtmSupport = CompatibilityCheckCpuSupportForRtm();

    //
    // Get x86 processor width for virtual address
    //
    g_compatibility_check.VirtualAddressWidth = CompatibilityCheckGetX86VirtualAddressWidth();

    //
    // Get x86 processor width for physical address
    //
    g_compatibility_check.PhysicalAddressWidth = CompatibilityCheckGetX86PhysicalAddressWidth();

    //
    // Check Mode-based execution compatibility
    //
    g_compatibility_check.ModeBasedExecutionSupport = CompatibilityCheckModeBasedExecution();

    //
    // Check PML support
    //
    g_compatibility_check.PmlSupport = CompatibilityCheckPml();

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
    UINT32 AddrWidth = g_compatibility_check.VirtualAddressWidth;

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