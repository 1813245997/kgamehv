#include <ntifs.h>
#include "memory_pool.h"
#include "ntos_struct_def.h"
#include "internal_function_defs.h"

namespace MemoryPoolTagger {
#define RANDOM_SEED_INIT 0x3AF84E05
	static ULONG RandomSeed = RANDOM_SEED_INIT;

	ULONG RtlNextRandom(ULONG Min, ULONG Max) // [Min,Max)
	{
		if (RandomSeed == RANDOM_SEED_INIT)  // One-time seed initialisation. It doesn't have to be good, just not the same every time
			RandomSeed = (ULONG)__rdtsc();

		// NB: In user mode, the correct scale for RtlUniform/RtlRandom/RtlRandomEx is different on Win 10+:
		// Scale = (RtlNtMajorVersion() >= 10 ? MAXUINT32 : MAXINT32) / (Max - Min);
		// The KM versions seem to have been unaffected by this change, at least up until RS3.
		// If this ever starts returning values >= Max, try the above scale instead
		const ULONG Scale = (ULONG)MAXINT32 / (Max - Min);
		return  utils::internal_functions::pfn_rtl_random_ex(&RandomSeed) / Scale + Min;
	}

	ULONG32 pool_tag()
	{
		ULONG PoolTags[] =
		{
			' prI', // Allocated IRP packets
			'+prI', // I/O verifier allocated IRP packets
			'eliF', // File objects
			'atuM', // Mutant objects
			'sFtN', // ntfs.sys!StrucSup.c
			'ameS', // Semaphore objects
			'RwtE', // Etw KM RegEntry
			'nevE', // Event objects
			' daV', // Mm virtual address descriptors
			'sdaV', // Mm virtual address descriptors (short)
			'aCmM', // Mm control areas for mapped files
			'  oI', // I/O manager
			'tiaW', // WaitCompletion Packets
			'eSmM', // Mm secured VAD allocation
			'CPLA', // ALPC port objects
			'GwtE', // ETW GUID
			' ldM', // Memory Descriptor Lists
			'erhT', // Thread objects
			'cScC', // Cache Manager Shared Cache Map
			'KgxD', // Vista display driver support
		};

		ULONG NumPoolTags = ARRAYSIZE(PoolTags);
		const ULONG Index = RtlNextRandom(0, NumPoolTags);
		NT_ASSERT(Index <= NumPoolTags - 1);
		return PoolTags[Index];
	}

}


