#include "global_defs.h"
#include "log_utils.h"
#include <ntstrsafe.h>
void LogPrint(__log_type type, const char* fmt, ...)
{
	const ULONG TIME_BUF_SIZE = 32;
	const ULONG MSG_BUF_SIZE = 512;
	const ULONG OUTPUT_BUF_SIZE = 1024;

	char time_buffer[32] = {};
	char message_buffer[1024] = {};
	char output_buffer[1024] = {};
	   
	const char* log_type_str = nullptr;
	LARGE_INTEGER system_time = {};
	LARGE_INTEGER local_time = {};
	TIME_FIELDS time_fields = {};

	switch (type) {
	case LOG_TYPE_DEBUG: log_type_str = "[DEBUG]"; break;
	case LOG_TYPE_DUMP: log_type_str = "[DUMP]"; break;
	case LOG_TYPE_ERROR: log_type_str = "[ERROR]"; break;
	case LOG_TYPE_INFO: log_type_str = "[INFO]"; break;
	default: log_type_str = "[UNKNOWN]"; break;
	}

	KeQuerySystemTime(&system_time);
	ExSystemTimeToLocalTime(&system_time, &local_time);
	RtlTimeToTimeFields(&local_time, &time_fields);

	RtlStringCchPrintfA(
		time_buffer,
		TIME_BUF_SIZE,
		"[%02hd:%02hd:%02hd.%03hd]",
		time_fields.Hour,
		time_fields.Minute,
		time_fields.Second,
		time_fields.Milliseconds
	);

	va_list args;
	va_start(args, fmt);
	RtlStringCchVPrintfA(message_buffer, MSG_BUF_SIZE, fmt, args);
	va_end(args);

	RtlStringCchPrintfA(
		output_buffer,
		OUTPUT_BUF_SIZE,
		"%s%s  %s\r\n",
		time_buffer,
		log_type_str,
		message_buffer
	);

#if ENABLE_HV_DEBUG_LOG

	DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, "%s", output_buffer);

#else

	DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_INFO_LEVEL, "%s", output_buffer);

#endif

	 
}
