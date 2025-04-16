#pragma once

extern "C"
{

	NTSYSAPI
		PIMAGE_NT_HEADERS
		NTAPI
		RtlImageNtHeader(
			PVOID Base
		);

	NTSYSAPI
		NTSTATUS
		NTAPI
		ZwQuerySystemInformation(
			IN SYSTEM_INFORMATION_CLASS SystemInformationClass,
			OUT PVOID                   SystemInformation,
			IN ULONG                    Length,
			OUT PULONG                  ReturnLength
		);
}
