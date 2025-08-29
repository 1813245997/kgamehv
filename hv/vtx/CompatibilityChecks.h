#pragma once

/**
 * @brief The status of available features in the processor
 *
 */
typedef struct _COMPATIBILITY_CHECKS_STATUS
{
    BOOLEAN IsX2Apic;                  // X2APIC or XAPIC routine
    BOOLEAN RtmSupport;                // check for RTM support
    BOOLEAN PmlSupport;                // check Page Modification Logging (PML) support
    BOOLEAN ModeBasedExecutionSupport; // check for mode based execution support (processors after Kaby Lake release will support this feature)
    BOOLEAN ExecuteOnlySupport;        // Support for execute-only pages (indicating that data accesses are not allowed while instruction fetches are allowed)
    UINT32  VirtualAddressWidth;       // Virtual address width for x86 processors
    UINT32  PhysicalAddressWidth;      // Physical address width for x86 processors

} COMPATIBILITY_CHECKS_STATUS, * PCOMPATIBILITY_CHECKS_STATUS;

//////////////////////////////////////////////////
//				   Functions					//
//////////////////////////////////////////////////

VOID
compatibility_check_perform_checks();

BOOLEAN check_address_canonicality(UINT64 vaddr, PBOOLEAN is_kernel_address);
 

