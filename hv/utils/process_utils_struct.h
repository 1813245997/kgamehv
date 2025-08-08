#pragma once
typedef struct _PROCESS_ENTRY
{
	LIST_ENTRY list_entry;
	HANDLE process_id;
	PEPROCESS process;
	PUNICODE_STRING image_path;
	PUNICODE_STRING process_name;
	LARGE_INTEGER create_time;
} PROCESS_ENTRY, * PPROCESS_ENTRY;