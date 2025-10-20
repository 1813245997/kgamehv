#pragma once

 

 
#include "../vtx/vmm.h"
/**
* @file LogMacros.h
* @brief Defines macros for logging various levels of messages and a function for log printing.
*
* @date 2024-04-20
* @author killer
*/

 
 
#pragma once
 

enum __log_type {
	LOG_TYPE_DEBUG,
	LOG_TYPE_DUMP,
	LOG_TYPE_ERROR,
	LOG_TYPE_INFO
};

 
enum __log_output_mode {
	LOG_OUTPUT_FILE,      
	LOG_OUTPUT_DBGPRINT   
};



namespace logger {

	struct LOG_CONFIG {
		UNICODE_STRING log_dir;        
		UNICODE_STRING log_name_prefix; 
		ULONG max_file_size;           
		ULONG max_file_count;          
		__log_output_mode output_mode;   
	};

	struct LOG_CONTEXT {
		volatile long lock_flag;
		HANDLE file_handle;
		ULONG current_file_size;
		ULONG current_file_index;
		LOG_CONFIG config;
	};

	extern LOG_CONTEXT g_log_ctx;

 
	NTSTATUS init_log_system(
		const wchar_t* log_dir,
		const wchar_t* log_name_prefix,
		ULONG max_file_size,
		ULONG max_file_count,
		__log_output_mode output_mode = LOG_OUTPUT_FILE);

	NTSTATUS open_log_file();
 
	void LogPrint(__log_type type, const char* fmt, ...);

 
	void shutdown_log_system();

} // namespace logger
 
#if ENABLE_HV_DEBUG_LOG
#define LogError(format, ...) \
    logger::LogPrint(LOG_TYPE_ERROR, "  [VMXMode:%d][%s:%d] " format, \
                      vmx_get_current_execution_mode(), __func__, __LINE__, ##__VA_ARGS__)

#define LogDebug(format, ...) \
    logger::LogPrint(LOG_TYPE_DEBUG, "  [VMXMode:%d][%s:%d] " format, \
                      vmx_get_current_execution_mode(), __func__, __LINE__, ##__VA_ARGS__)

#define LogDump(format, ...) \
    logger::LogPrint(LOG_TYPE_DUMP, "  [VMXMode:%d][%s:%d] " format, \
                     vmx_get_current_execution_mode(), __func__, __LINE__, ##__VA_ARGS__)

#define LogInfo(format, ...) \
    logger::LogPrint(LOG_TYPE_INFO, "  [VMXMode:%d][%s:%d] " format, \
                     vmx_get_current_execution_mode(), __func__, __LINE__, ##__VA_ARGS__)
#else
// 当调试日志被禁用时，这些宏展开为空
#define LogError(format, ...) ((void)0)
#define LogDebug(format, ...) ((void)0)
#define LogDump(format, ...) ((void)0)
#define LogInfo(format, ...) ((void)0)
#endif

 
 