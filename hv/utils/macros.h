#pragma once

#define ENABLE_HV_DEBUG_LOG 1// Set to 0 to disable debug logging

#define USE_MANUAL_MAP_MODE 1 // Enable manual mapping for driver loading

#define ENABLE_GAME_DRAW_TYPE3 1 // 1 means using the first game drawing scheme


#define S_OK                                   ((HRESULT)0L)
#define S_FALSE                                ((HRESULT)1L)



#define MmiInvaildAddressValue ((PVOID64)~0)


#define MmiEntryToAddress(v)			(((ULONG64)v)&0x000FFFFFFFFFF000)
#define MmiEntryFlag_Present			((ULONG64)0x0000000000000001)
#define MmiEntryFlag_Write				((ULONG64)0x0000000000000002)
#define MmiEntryFlag_UserAccessible		((ULONG64)0x0000000000000004)
#define MmiEntryFlag_WriteThrough		((ULONG64)0x0000000000000008)
#define MmiEntryFlag_DisableCache		((ULONG64)0x0000000000000010)
#define MmiEntryFlag_Accessed			((ULONG64)0x0000000000000020)
#define MmiEntryFlag_Dirty				((ULONG64)0x0000000000000040)
#define MmiEntryFlag_HugePage			((ULONG64)0x0000000000000080)
#define MmiEntryFlag_Global				((ULONG64)0x0000000000000100)
#define MmiEntryFlag_NoExecute			((ULONG64)0x8000000000000000)

#define MmiEntryFlag_EntryPage			(MmiEntryFlag_Present | MmiEntryFlag_Write | MmiEntryFlag_Accessed | MmiEntryFlag_Dirty)
#define MmiEntryFlag_ReadOnlyPage			(MmiEntryFlag_Present | MmiEntryFlag_Accessed | MmiEntryFlag_Dirty)

#define MmiCheckFlag(e,f) (e&f)

#define BITS_PER_LONG (sizeof(unsigned long) * 8)
#define ORDER_LONG    (sizeof(unsigned long) == 4 ? 5 : 6)

#define BITMAP_ENTRY(_nr, _bmap) ((_bmap))[(_nr) / BITS_PER_LONG]
#define BITMAP_SHIFT(_nr)        ((_nr) % BITS_PER_LONG)

#define NULL_ZERO   0
#define NULL64_ZERO 0ull



#define SUCCEEDED(hr) (((HRESULT)(hr)) >= 0)
#define FAILED(hr) (((HRESULT)(hr)) < 0)




 
#pragma endregion

#define IS_INTRESOURCE(_r) ((((ULONG_PTR)(_r)) >> 16) == 0)
#define MAKEINTRESOURCEA(i) ((LPSTR)((ULONG_PTR)((WORD)(i))))
#define MAKEINTRESOURCEW(i) ((LPWSTR)((ULONG_PTR)((WORD)(i))))
#ifdef UNICODE
#define MAKEINTRESOURCE  MAKEINTRESOURCEW
#else
#define MAKEINTRESOURCE  MAKEINTRESOURCEA
#endif // !UNICODE

#define IDC_ARROW           MAKEINTRESOURCE(32512)
#define IDC_IBEAM           MAKEINTRESOURCE(32513)
#define IDC_WAIT            MAKEINTRESOURCE(32514)
#define IDC_CROSS           MAKEINTRESOURCE(32515)
#define IDC_UPARROW         MAKEINTRESOURCE(32516)
#define IDC_SIZE            MAKEINTRESOURCE(32640)  /* OBSOLETE: use IDC_SIZEALL */
#define IDC_ICON            MAKEINTRESOURCE(32641)  /* OBSOLETE: use IDC_ARROW */
#define IDC_SIZENWSE        MAKEINTRESOURCE(32642)
#define IDC_SIZENESW        MAKEINTRESOURCE(32643)
#define IDC_SIZEWE          MAKEINTRESOURCE(32644)
#define IDC_SIZENS          MAKEINTRESOURCE(32645)
#define IDC_SIZEALL         MAKEINTRESOURCE(32646)
#define IDC_NO              MAKEINTRESOURCE(32648) /*not in win3.1 */

#define IDC_HAND            MAKEINTRESOURCE(32649)
#define IDC_APPSTARTING     MAKEINTRESOURCE(32650) /*not in win3.1 */


#define VMM_TAG 'vhra'

#define ARRAY_SIZE(x) (sizeof(x) / sizeof((x)[0]))


				  /**
				   * @brief Hypervisor reserved range for RDMSR and WRMSR
				   *
				   */
#define RESERVED_MSR_RANGE_LOW 0x40000000
#define RESERVED_MSR_RANGE_HI  0x400000F0

				   /**
					* @brief Cpuid to get virtual address width
					*
					*/
#define CPUID_ADDR_WIDTH 0x80000008


#define SM_CXSCREEN             0
#define SM_CYSCREEN             1


 
#define KPCR_TSS_BASE_OFFSET         0x008
#define TSS_IST_OFFSET               0x01c


#define KEY_MAKE  0
#define KEY_BREAK 1
#define KEY_E0    2
#define KEY_E1    4
#define KEY_TERMSRV_SET_LED 8
#define KEY_TERMSRV_SHADOW  0x10
#define KEY_TERMSRV_VKPACKET 0x20

#define KEY_DOWN                KEY_MAKE
#define KEY_UP                  KEY_BREAK
#define KEY_BLANK                -1

// Extended key flags for specific keys
#define KEY_EXTENDED_FLAGS       2    // Extended keys flags value (E0 prefix) - used for HOME, Insert, Delete, etc.