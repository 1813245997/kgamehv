#pragma once

#define ENABLE_HV_DEBUG_LOG 1 // Set to 0 to disable debug logging

#define USE_MANUAL_MAP_MODE 1 // Enable manual mapping for driver loading

#define ENABLE_GAME_DRAW_TYPE3 2 // 1 means using the first game drawing scheme


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