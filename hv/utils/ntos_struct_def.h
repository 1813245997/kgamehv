#include "types.h"
extern "C"
{

#define MM_COPY_MEMORY_PHYSICAL             0x1
#define MM_COPY_MEMORY_VIRTUAL              0x2

#ifndef _SYSTEM_INFORMATION_CLASS
	typedef enum _SYSTEM_INFORMATION_CLASS
	{
		SystemBasicInformation = 0,
		SystemProcessorInformation = 1,
		SystemPerformanceInformation = 2,
		SystemTimeOfDayInformation = 3,
		SystemPathInformation = 4,
		SystemProcessInformation = 5,
		SystemCallCountInformation = 6,
		SystemDeviceInformation = 7,
		SystemProcessorPerformanceInformation = 8,
		SystemFlagsInformation = 9,
		SystemCallTimeInformation = 10,
		SystemModuleInformation = 11,
		SystemLocksInformation = 12,
		SystemStackTraceInformation = 13,
		SystemPagedPoolInformation = 14,
		SystemNonPagedPoolInformation = 15,
		SystemHandleInformation = 16,
		SystemObjectInformation = 17,
		SystemFileCacheInformation = 21,
		SystemPageFileInformation = 18,
		SystemVdmInstemulInformation = 19,
		SystemVdmBopInformation = 20,
		SystemPoolTagInformation = 22,
		SystemInterruptInformation = 23,
		SystemDpcBehaviorInformation = 24,
		SystemFullMemoryInformation = 25,
		SystemLoadGdiDriverInformation = 26,
		SystemUnloadGdiDriverInformation = 27,
		SystemTimeAdjustmentInformation = 28,
		SystemSummaryMemoryInformation = 29,
		SystemMirrorMemoryInformation = 30,
		SystemPerformanceTraceInformation = 31,
		SystemObsolete0 = 32,
		SystemExceptionInformation = 33,
		SystemCrashDumpStateInformation = 34,
		SystemKernelDebuggerInformation = 35,
		SystemContextSwitchInformation = 36,
		SystemRegistryQuotaInformation = 37,
		SystemExtendServiceTableInformation = 38,
		SystemPrioritySeperation = 39,
		SystemVerifierAddDriverInformation = 40,
		SystemVerifierRemoveDriverInformation = 41,
		SystemProcessorIdleInformation = 42,
		SystemLegacyDriverInformation = 43,
		SystemCurrentTimeZoneInformation = 44,
		SystemLookasideInformation = 45,
		SystemTimeSlipNotification = 46,
		SystemSessionCreate = 47,
		SystemSessionDetach = 48,
		SystemSessionInformation = 49,
		SystemRangeStartInformation = 50,
		SystemVerifierInformation = 51,
		SystemVerifierThunkExtend = 52,
		SystemSessionProcessInformation = 53,
		SystemLoadGdiDriverInSystemSpace = 54,
		SystemNumaProcessorMap = 55,
		SystemPrefetcherInformation = 56,
		SystemExtendedProcessInformation = 57,
		SystemRecommendedSharedDataAlignment = 58,
		SystemComPlusPackage = 59,
		SystemNumaAvailableMemory = 60,
		SystemProcessorPowerInformation = 61,
		SystemEmulationBasicInformation = 62,
		SystemEmulationProcessorInformation = 63,
		SystemExtendedHandleInformation = 64,
		SystemLostDelayedWriteInformation = 65,
		SystemBigPoolInformation = 66,
		SystemSessionPoolTagInformation = 67,
		SystemSessionMappedViewInformation = 68,
		SystemHotpatchInformation = 69,
		SystemObjectSecurityMode = 70,
		SystemWatchdogTimerHandler = 71,
		SystemWatchdogTimerInformation = 72,
		SystemLogicalProcessorInformation = 73,
		SystemWow64SharedInformationObsolete = 74,
		SystemRegisterFirmwareTableInformationHandler = 75,
		SystemFirmwareTableInformation = 76,
		SystemModuleInformationEx = 77,
		SystemVerifierTriageInformation = 78,
		SystemSuperfetchInformation = 79,
		SystemMemoryListInformation = 80,
		SystemFileCacheInformationEx = 81,
		SystemThreadPriorityClientIdInformation = 82,
		SystemProcessorIdleCycleTimeInformation = 83,
		SystemVerifierCancellationInformation = 84,
		SystemProcessorPowerInformationEx = 85,
		SystemRefTraceInformation = 86,
		SystemSpecialPoolInformation = 87,
		SystemProcessIdInformation = 88,
		SystemErrorPortInformation = 89,
		SystemBootEnvironmentInformation = 90,
		SystemHypervisorInformation = 91,
		SystemVerifierInformationEx = 92,
		SystemTimeZoneInformation = 93,
		SystemImageFileExecutionOptionsInformation = 94,
		SystemCoverageInformation = 95,
		SystemPrefetchPatchInformation = 96,
		SystemVerifierFaultsInformation = 97,
		SystemSystemPartitionInformation = 98,
		SystemSystemDiskInformation = 99,
		SystemProcessorPerformanceDistribution = 100,
		SystemNumaProximityNodeInformation = 101,
		SystemDynamicTimeZoneInformation = 102,
		SystemCodeIntegrityInformation = 103,
		SystemProcessorMicrocodeUpdateInformation = 104,
		SystemProcessorBrandString = 105,
		SystemVirtualAddressInformation = 106,
		SystemLogicalProcessorAndGroupInformation = 107,
		SystemProcessorCycleTimeInformation = 108,
		SystemStoreInformation = 109,
		SystemRegistryAppendString = 110,
		SystemAitSamplingValue = 111,
		SystemVhdBootInformation = 112,
		SystemCpuQuotaInformation = 113,
		SystemNativeBasicInformation = 114,
		SystemErrorPortTimeouts = 115,
		SystemLowPriorityIoInformation = 116,
		SystemBootEntropyInformation = 117,
		SystemVerifierCountersInformation = 118,
		SystemPagedPoolInformationEx = 119,
		SystemSystemPtesInformationEx = 120,
		SystemNodeDistanceInformation = 121,
		SystemAcpiAuditInformation = 122,
		SystemBasicPerformanceInformation = 123,
		SystemQueryPerformanceCounterInformation = 124,
		SystemSessionBigPoolInformation = 125,
		SystemBootGraphicsInformation = 126,
		SystemScrubPhysicalMemoryInformation = 127,
		SystemBadPageInformation = 128,
		SystemProcessorProfileControlArea = 129,
		SystemCombinePhysicalMemoryInformation = 130,
		SystemEntropyInterruptTimingInformation = 131,
		SystemConsoleInformation = 132,
		SystemPlatformBinaryInformation = 133,
		SystemPolicyInformation = 134,
		SystemHypervisorProcessorCountInformation = 135,
		SystemDeviceDataInformation = 136,
		SystemDeviceDataEnumerationInformation = 137,
		SystemMemoryTopologyInformation = 138,
		SystemMemoryChannelInformation = 139,
		SystemBootLogoInformation = 140,
		SystemProcessorPerformanceInformationEx = 141,
		SystemSpare0 = 142,
		SystemSecureBootPolicyInformation = 143,
		SystemPageFileInformationEx = 144,
		SystemSecureBootInformation = 145,
		SystemEntropyInterruptTimingRawInformation = 146,
		SystemPortableWorkspaceEfiLauncherInformation = 147,
		SystemFullProcessInformation = 148,
		SystemKernelDebuggerInformationEx = 149,
		SystemBootMetadataInformation = 150,
		SystemSoftRebootInformation = 151,
		SystemElamCertificateInformation = 152,
		SystemOfflineDumpConfigInformation = 153,
		SystemProcessorFeaturesInformation = 154,
		SystemRegistryReconciliationInformation = 155,
		SystemEdidInformation = 156,
		MaxSystemInfoClass = 157,
		SystemSupportedProcessArchitectures = 0xb5
	} SYSTEM_INFORMATION_CLASS, * PSYSTEM_INFORMATION_CLASS;
#endif


#ifndef SYSTEM_HANDLE_TABLE_ENTRY_INFO_EX
	typedef struct _SYSTEM_HANDLE_TABLE_ENTRY_INFO_EX {
		PVOID Object;
		ULONG_PTR UniqueProcessId;
		ULONG_PTR HandleValue;
		ULONG GrantedAccess;
		USHORT CreatorBackTraceIndex;
		USHORT ObjectTypeIndex;
		ULONG  HandleAttributes;
		ULONG  Reserved;
	} SYSTEM_HANDLE_TABLE_ENTRY_INFO_EX, * PSYSTEM_HANDLE_TABLE_ENTRY_INFO_EX;

	typedef struct _SYSTEM_HANDLE_INFORMATION_EX {
		ULONG_PTR NumberOfHandles;
		ULONG_PTR Reserved;
		SYSTEM_HANDLE_TABLE_ENTRY_INFO_EX Handles[1];
	} SYSTEM_HANDLE_INFORMATION_EX, * PSYSTEM_HANDLE_INFORMATION_EX;
#endif

	typedef struct _SYSTEM_MODULE_ENTRY
	{
		HANDLE Section;
		PVOID MappedBase;
		PVOID ImageBase;
		ULONG ImageSize;
		ULONG Flags;
		USHORT LoadOrderIndex;
		USHORT InitOrderIndex;
		USHORT LoadCount;
		USHORT OffsetToFileName;
		UCHAR FullPathName[256];
	} SYSTEM_MODULE_ENTRY, * PSYSTEM_MODULE_ENTRY;
	typedef struct _SYSTEM_MODULE {
		PVOID 	Reserved1;
		PVOID 	Reserved2;
		PVOID 	ImageBaseAddress;
		ULONG 	ImageSize;
		ULONG 	Flags;
		unsigned short 	Id;
		unsigned short 	Rank;
		unsigned short 	Unknown;
		unsigned short 	NameOffset;
		unsigned char 	Name[MAXIMUM_FILENAME_LENGTH];
	} SYSTEM_MODULE, * PSYSTEM_MODULE;
	typedef struct _SYSTEM_MODULE_INFORMATION {
		ULONG                       ModulesCount;
		SYSTEM_MODULE_ENTRY         Modules[1];
		ULONG                       Count;
		SYSTEM_MODULE 	            Sys_Modules[1];
	} SYSTEM_MODULE_INFORMATION, * PSYSTEM_MODULE_INFORMATION;

#ifndef SYSTEM_PROCESS_INFORMATION
	typedef struct _SYSTEM_THREAD_INFORMATION
	{
		LARGE_INTEGER KernelTime;
		LARGE_INTEGER UserTime;
		LARGE_INTEGER CreateTime;
		ULONG WaitTime;
		PVOID StartAddress;
		CLIENT_ID ClientId;
		KPRIORITY Priority;
		LONG BasePriority;
		ULONG ContextSwitches;
		ULONG ThreadState;
		KWAIT_REASON WaitReason;
	}SYSTEM_THREAD_INFORMATION, * PSYSTEM_THREAD_INFORMATION;
	typedef struct _SYSTEM_PROCESS_INFORMATION
	{
		ULONG NextEntryOffset;
		ULONG NumberOfThreads;
		LARGE_INTEGER WorkingSetPrivateSize; // since VISTA
		ULONG HardFaultCount; // since WIN7
		ULONG NumberOfThreadsHighWatermark; // since WIN7
		ULONGLONG CycleTime; // since WIN7
		LARGE_INTEGER CreateTime;
		LARGE_INTEGER UserTime;
		LARGE_INTEGER KernelTime;
		UNICODE_STRING ImageName;
		KPRIORITY BasePriority;
		HANDLE UniqueProcessId;
		HANDLE InheritedFromUniqueProcessId;
		ULONG HandleCount;
		ULONG SessionId;
		ULONG_PTR UniqueProcessKey; // since VISTA (requires SystemExtendedProcessInformation)
		SIZE_T PeakVirtualSize;
		SIZE_T VirtualSize;
		ULONG PageFaultCount;
		SIZE_T PeakWorkingSetSize;
		SIZE_T WorkingSetSize;
		SIZE_T QuotaPeakPagedPoolUsage;
		SIZE_T QuotaPagedPoolUsage;
		SIZE_T QuotaPeakNonPagedPoolUsage;
		SIZE_T QuotaNonPagedPoolUsage;
		SIZE_T PagefileUsage;
		SIZE_T PeakPagefileUsage;
		SIZE_T PrivatePageCount;
		LARGE_INTEGER ReadOperationCount;
		LARGE_INTEGER WriteOperationCount;
		LARGE_INTEGER OtherOperationCount;
		LARGE_INTEGER ReadTransferCount;
		LARGE_INTEGER WriteTransferCount;
		LARGE_INTEGER OtherTransferCount;
		SYSTEM_THREAD_INFORMATION Threads[1];
	} SYSTEM_PROCESS_INFORMATION, * PSYSTEM_PROCESS_INFORMATION;

	typedef struct _SYSTEM_SESSION_PROCESS_INFORMATION
	{
		ULONG SessionId;
		ULONG SizeOfBuf;
		PVOID Buffer;
	}SYSTEM_SESSION_PROCESS_INFORMATION, * PSYSTEM_SESSION_PROCESS_INFORMATION;
#endif




	typedef struct _IMAGE_RUNTIME_FUNCTION_ENTRY RUNTIME_FUNCTION, * PRUNTIME_FUNCTION;


#define UNWIND_HISTORY_TABLE_SIZE 12
	typedef struct _UNWIND_HISTORY_TABLE_ENTRY {
		DWORD64 ImageBase;
		PRUNTIME_FUNCTION FunctionEntry;
	} UNWIND_HISTORY_TABLE_ENTRY, * PUNWIND_HISTORY_TABLE_ENTRY;

	typedef struct _UNWIND_HISTORY_TABLE {
		DWORD Count;
		BYTE  LocalHint;
		BYTE  GlobalHint;
		BYTE  Search;
		BYTE  Once;
		DWORD64 LowAddress;
		DWORD64 HighAddress;
		UNWIND_HISTORY_TABLE_ENTRY Entry[UNWIND_HISTORY_TABLE_SIZE];
	} UNWIND_HISTORY_TABLE, * PUNWIND_HISTORY_TABLE;

	typedef struct _MM_COPY_ADDRESS {
		union {
			PVOID VirtualAddress;
			PHYSICAL_ADDRESS PhysicalAddress;
		};
	} MM_COPY_ADDRESS, * PMMCOPY_ADDRESS;


	typedef struct _MM_UNLOADED_DRIVER
	{
		UNICODE_STRING 	Name;
		PVOID 			ModuleStart;
		PVOID 			ModuleEnd;
		ULONG64 		UnloadTime;
	} MM_UNLOADED_DRIVER, * PMM_UNLOADED_DRIVER;

	typedef struct _pte_64
	{
		ULONG64 valid : 1;               // [0]
		ULONG64 write : 1;               // [1]
		ULONG64 owner : 1;               // [2]
		ULONG64 write_through : 1;       // [3]
		ULONG64 cache_disable : 1;       // [4]
		ULONG64 accessed : 1;            // [5]
		ULONG64 dirty : 1;               // [6]
		ULONG64 large_page : 1;          // [7]
		ULONG64 global : 1;              // [8]
		ULONG64 copy_on_write : 1;       // [9]
		ULONG64 prototype : 1;           // [10]
		ULONG64 reserved0 : 1;           // [11]
		ULONG64 page_frame_number : 36;  // [12:47]
		ULONG64 reserved1 : 4;           // [48:51]
		ULONG64 software_ws_index : 11;  // [52:62]
		ULONG64 no_execute : 1;          // [63]
	} mypte_64, * ppte_64;

	typedef struct _HANDLE_TABLE_ENTRY_INFO {


		//
		//  The following field contains the audit mask for the handle if one
		//  exists.  The purpose of the audit mask is to record all of the accesses
		//  that may have been audited when the handle was opened in order to
		//  support "per operation" based auditing.  It is computed by walking the
		//  SACL of the object being opened and keeping a record of all of the audit
		//  ACEs that apply to the open operation going on.  Each set bit corresponds
		//  to an access that would be audited.  As each operation takes place, its
		//  corresponding access bit is removed from this mask.
		//

		ACCESS_MASK AuditMask;

	} HANDLE_TABLE_ENTRY_INFO, * PHANDLE_TABLE_ENTRY_INFO;

	//隐藏全局句柄表自己的句柄
	typedef struct _HANDLE_TABLE_ENTRY // Size=16
	{
		union
		{
			ULONG_PTR VolatileLowValue; // Size=8 Offset=0
			ULONG_PTR LowValue; // Size=8 Offset=0
			struct _HANDLE_TABLE_ENTRY_INFO* InfoTable; // Size=8 Offset=0
			struct
			{
				ULONG64 Unlocked : 1; // Size=8 Offset=0 BitOffset=0 BitCount=1
				ULONG64 RefCnt : 16; // Size=8 Offset=0 BitOffset=1 BitCount=16
				ULONG64 Attributes : 3; // Size=8 Offset=0 BitOffset=17 BitCount=3
				ULONG64 ObjectPointerBits : 44; //  对象地址 <<4  || 0XFFFF00000000000
			};
		};
		union
		{
			ULONG_PTR HighValue; // Size=8 Offset=8
			struct _HANDLE_TABLE_ENTRY* NextFreeHandleEntry; // Size=8 Offset=8
			ULONG64 LeafHandleValue; // Size=8 Offset=8
			struct
			{
				ULONG GrantedAccessBits : 25; //  对象权限
				ULONG NoRightsUpgrade : 1; // Size=4 Offset=8 BitOffset=25 BitCount=1
				ULONG Spare : 6; // Size=4 Offset=8 BitOffset=26 BitCount=6
			};
		};
		ULONG TypeInfo; // Size=4 Offset=12
	} HANDLE_TABLE_ENTRY, * PHANDLE_TABLE_ENTRY;


#define HANDLE_TRACE_DB_STACK_SIZE 16

	typedef struct _HANDLE_TRACE_DB_ENTRY {
		CLIENT_ID ClientId;
		HANDLE Handle;
#define HANDLE_TRACE_DB_OPEN    1
#define HANDLE_TRACE_DB_CLOSE   2
#define HANDLE_TRACE_DB_BADREF  3
		ULONG Type;
		PVOID StackTrace[HANDLE_TRACE_DB_STACK_SIZE];
	} HANDLE_TRACE_DB_ENTRY, * PHANDLE_TRACE_DB_ENTRY;

	typedef struct _HANDLE_TRACE_DEBUG_INFO {

		//
		// Reference count for this structure
		//
		LONG RefCount;

		//
		// Size of the trace table in entries
		//

		ULONG TableSize;

		//
		// this flag will clean the TraceDb.
		// once the TraceDb is cleaned, this flag will be reset.
		// it is needed for setting the HANDLE_TRACE_DEBUG_INFO_COMPACT_CLOSE_HANDLE
		// dynamically via KD
		//
#define HANDLE_TRACE_DEBUG_INFO_CLEAN_DEBUG_INFO        0x1

	//
	// this flag will do the following: for each close
	// it will look for a matching open, remove the open
	// entry and compact TraceDb
	// NOTE: this should not be used for HANDLE_TRACE_DB_BADREF
	//      because you will not have the close trace
	//
#define HANDLE_TRACE_DEBUG_INFO_COMPACT_CLOSE_HANDLE    0x2

	//
	// setting this flag will break into debugger when the trace list
	// wraps around. This way you will have a chance to loot at old entries
	// before they are deleted
	//
#define HANDLE_TRACE_DEBUG_INFO_BREAK_ON_WRAP_AROUND    0x4

	//
	// these are status flags, do not set them explicitly
	//
#define HANDLE_TRACE_DEBUG_INFO_WAS_WRAPPED_AROUND      0x40000000
#define HANDLE_TRACE_DEBUG_INFO_WAS_SOMETIME_CLEANED    0x80000000

		ULONG BitMaskFlags;

		FAST_MUTEX CloseCompactionLock;

		//
		// Current index for the stack trace DB
		//
		ULONG CurrentStackIndex;

		//
		// Save traces of those who open and close handles
		//
		HANDLE_TRACE_DB_ENTRY TraceDb[1];

	} HANDLE_TRACE_DEBUG_INFO, * PHANDLE_TRACE_DEBUG_INFO;

	//0x40 bytes (sizeof)
	struct _HANDLE_TABLE_FREE_LIST
	{
		ULONG64 FreeListLock;                                      //0x0
		union HANDLE_TABLE_ENTRY* FirstFreeHandleEntry;                        //0x8
		union HANDLE_TABLE_ENTRY* LastFreeHandleEntry;                         //0x10
		LONG HandleCount;                                                       //0x18
		ULONG HighWaterMark;                                                    //0x1c
	};

	typedef struct _HANDLE_TABLE
	{
		ULONG NextHandleNeedingPool;                                            //0x0
		LONG ExtraInfoPages;                                                    //0x4
		volatile ULONGLONG TableCode;                                           //0x8
		struct _EPROCESS* QuotaProcess;                                         //0x10
		struct _LIST_ENTRY HandleTableList;                                     //0x18
		ULONG UniqueProcessId;                                                  //0x28
		union
		{
			ULONG Flags;                                                        //0x2c
			struct
			{
				UCHAR StrictFIFO : 1;                                             //0x2c
				UCHAR EnableHandleExceptions : 1;                                 //0x2c
				UCHAR Rundown : 1;                                                //0x2c
				UCHAR Duplicated : 1;                                             //0x2c
				UCHAR RaiseUMExceptionOnInvalidHandleClose : 1;                   //0x2c
			};
		};
		ULONG64 HandleContentionEvent;                             //0x30
		ULONG64 HandleTableLock;                                   //0x38
		union
		{
			struct _HANDLE_TABLE_FREE_LIST FreeLists[1];                        //0x40
			struct
			{
				UCHAR ActualEntry[32];                                          //0x40
				struct _HANDLE_TRACE_DEBUG_INFO* DebugInfo;                     //0x60
			};
		};


	}HANDLE_TABLE, * PHANDLE_TABLE;

	typedef struct _EXHANDLE {

		union {

			struct {

				//
				//  Application available tag bits
				//

				ULONG TagBits : 2;

				//
				//  The handle table entry index
				//

				ULONG Index : 30;

			};

			HANDLE GenericHandleOverlay;

#define HANDLE_VALUE_INC 4 // Amount to increment the Value to get to the next handle

			ULONG_PTR Value;
		};

	} EXHANDLE, * PEXHANDLE;

	typedef struct _PEB_LDR_DATA
	{
		ULONG Length;
		UCHAR Initialized;
		PVOID SsHandle;
		LIST_ENTRY InLoadOrderModuleList;
		LIST_ENTRY InMemoryOrderModuleList;
		LIST_ENTRY InInitializationOrderModuleList;
	} PEB_LDR_DATA, * PPEB_LDR_DATA;


	typedef struct _LDR_DATA_TABLE_ENTRY
	{
		LIST_ENTRY InLoadOrderLinks;
		LIST_ENTRY InMemoryOrderLinks;
		LIST_ENTRY InInitializationOrderLinks;
		PVOID DllBase;
		PVOID EntryPoint;
		ULONG SizeOfImage;
		UNICODE_STRING FullDllName;
		UNICODE_STRING BaseDllName;
		ULONG Flags;
		USHORT LoadCount;
		USHORT TlsIndex;
		LIST_ENTRY HashLinks;
		ULONG TimeDateStamp;
	} LDR_DATA_TABLE_ENTRY, * PLDR_DATA_TABLE_ENTRY;

	typedef struct _PEB
	{
		UCHAR InheritedAddressSpace;
		UCHAR ReadImageFileExecOptions;
		UCHAR BeingDebugged;
		UCHAR BitField;
		PVOID Mutant;
		PVOID ImageBaseAddress;
		PPEB_LDR_DATA Ldr;
		PVOID ProcessParameters;
		PVOID SubSystemData;
		PVOID ProcessHeap;
		PVOID FastPebLock;
		PVOID AtlThunkSListPtr;
		PVOID IFEOKey;
		PVOID CrossProcessFlags;
		PVOID UserSharedInfoPtr;
		ULONG SystemReserved;
		ULONG AtlThunkSListPtr32;
		PVOID ApiSetMap;
	} PEB, * PPEB;

	typedef struct _PEB_LDR_DATA32
	{
		ULONG Length;
		UCHAR Initialized;
		ULONG SsHandle;
		LIST_ENTRY32 InLoadOrderModuleList;
		LIST_ENTRY32 InMemoryOrderModuleList;
		LIST_ENTRY32 InInitializationOrderModuleList;
	} PEB_LDR_DATA32, * PPEB_LDR_DATA32;

	typedef struct _LDR_DATA_TABLE_ENTRY32
	{
		LIST_ENTRY32 InLoadOrderLinks;
		LIST_ENTRY32 InMemoryOrderLinks;
		LIST_ENTRY32 InInitializationOrderLinks;
		ULONG DllBase;
		ULONG EntryPoint;
		ULONG SizeOfImage;
		UNICODE_STRING32 FullDllName;
		UNICODE_STRING32 BaseDllName;
		ULONG Flags;
		USHORT LoadCount;
		USHORT TlsIndex;
		LIST_ENTRY32 HashLinks;
		ULONG TimeDateStamp;
	} LDR_DATA_TABLE_ENTRY32, * PLDR_DATA_TABLE_ENTRY32;

	typedef struct _PEB32
	{
		UCHAR InheritedAddressSpace;
		UCHAR ReadImageFileExecOptions;
		UCHAR BeingDebugged;
		UCHAR BitField;
		ULONG Mutant;
		ULONG ImageBaseAddress;
		ULONG Ldr;
		ULONG ProcessParameters;
		ULONG SubSystemData;
		ULONG ProcessHeap;
		ULONG FastPebLock;
		ULONG AtlThunkSListPtr;
		ULONG IFEOKey;
		ULONG CrossProcessFlags;
		ULONG UserSharedInfoPtr;
		ULONG SystemReserved;
		ULONG AtlThunkSListPtr32;
		ULONG ApiSetMap;
	} PEB32, * PPEB32;

	typedef struct _WOW64_PROCESS
	{
		PPEB32 Wow64;
	} WOW64_PROCESS, * PWOW64_PROCESS;

	typedef struct _SSDT
	{
		LONG* ServiceTable;
		PVOID CounterTable;
		ULONG64 SyscallsNumber;
		PVOID ArgumentTable;
	}_SSDT, * PSSDT;

	typedef enum _WINDOWINFOCLASS {
		WindowProcess,
		WindowThread,
		WindowActiveWindow,
		WindowFocusWindow,
		WindowIsHung,
		WindowClientBase,
		WindowIsForegroundThread,
	} WINDOWINFOCLASS;

	typedef struct tagPOINT
	{
		LONG  x;
		LONG  y;
	} POINT ;

	typedef struct tagRECT {
		LONG left;
		LONG top;
		LONG right;
		LONG bottom;
	} RECT;


	//0x18 bytes (sizeof)
	struct _CURDIR
	{
		struct _UNICODE_STRING DosPath;                                         //0x0
		VOID* Handle;                                                           //0x10
	};
	//0x18 bytes (sizeof)
	struct _RTL_DRIVE_LETTER_CURDIR
	{
		USHORT Flags;                                                           //0x0
		USHORT Length;                                                          //0x2
		ULONG TimeStamp;                                                        //0x4
		struct _STRING DosPath;                                                 //0x8
	};

		//0x440 bytes (sizeof)
	 typedef	struct _RTL_USER_PROCESS_PARAMETERS
	{
		ULONG MaximumLength;                                                    //0x0
		ULONG Length;                                                           //0x4
		ULONG Flags;                                                            //0x8
		ULONG DebugFlags;                                                       //0xc
		VOID* ConsoleHandle;                                                    //0x10
		ULONG ConsoleFlags;                                                     //0x18
		VOID* StandardInput;                                                    //0x20
		VOID* StandardOutput;                                                   //0x28
		VOID* StandardError;                                                    //0x30
		struct _CURDIR CurrentDirectory;                                        //0x38
		struct _UNICODE_STRING DllPath;                                         //0x50
		struct _UNICODE_STRING ImagePathName;                                   //0x60
		struct _UNICODE_STRING CommandLine;                                     //0x70
		VOID* Environment;                                                      //0x80
		ULONG StartingX;                                                        //0x88
		ULONG StartingY;                                                        //0x8c
		ULONG CountX;                                                           //0x90
		ULONG CountY;                                                           //0x94
		ULONG CountCharsX;                                                      //0x98
		ULONG CountCharsY;                                                      //0x9c
		ULONG FillAttribute;                                                    //0xa0
		ULONG WindowFlags;                                                      //0xa4
		ULONG ShowWindowFlags;                                                  //0xa8
		struct _UNICODE_STRING WindowTitle;                                     //0xb0
		struct _UNICODE_STRING DesktopInfo;                                     //0xc0
		struct _UNICODE_STRING ShellInfo;                                       //0xd0
		struct _UNICODE_STRING RuntimeData;                                     //0xe0
		struct _RTL_DRIVE_LETTER_CURDIR CurrentDirectores[32];                  //0xf0
		ULONGLONG EnvironmentSize;                                              //0x3f0
		ULONGLONG EnvironmentVersion;                                           //0x3f8
		VOID* PackageDependencyData;                                            //0x400
		ULONG ProcessGroupId;                                                   //0x408
		ULONG LoaderThreads;                                                    //0x40c
		struct _UNICODE_STRING RedirectionDllName;                              //0x410
		struct _UNICODE_STRING HeapPartitionName;                               //0x420
		ULONGLONG* DefaultThreadpoolCpuSetMasks;                                //0x430
		ULONG DefaultThreadpoolCpuSetMaskCount;                                 //0x438
		ULONG DefaultThreadpoolThreadMaximum;                                   //0x43c
	}*PRTL_USER_PROCESS_PARAMETERS;




	 struct __nt_kprocess
	 {
		 DISPATCHER_HEADER Header;                                       //0x0
		 LIST_ENTRY ProfileListHead;                                     //0x18
		 ULONGLONG DirectoryTableBase;
	 };

}

extern "C"
{
	NTSYSAPI  NTSTATUS NTAPI ZwQueryInformationProcess(
		IN  HANDLE ProcessHandle,
		IN  PROCESSINFOCLASS ProcessInformationClass,
		OUT PVOID ProcessInformation,
		IN  ULONG ProcessInformationLength,
		IN  PULONG ReturnLength
	);
	NTSYSAPI
		VOID
		NTAPI
		KeGenericCallDpc(
			_In_ PKDEFERRED_ROUTINE Routine,
			_In_opt_ PVOID Context
		);

	NTSYSAPI
		VOID
		NTAPI
		KeSignalCallDpcDone(
			_In_ PVOID SystemArgument1
		);

	NTSYSAPI
		LOGICAL
		NTAPI
		KeSignalCallDpcSynchronize(
			_In_ PVOID SystemArgument2
		);

}

