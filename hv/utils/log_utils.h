#pragma once


#include "../vtx/common.h"
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



namespace logger {

	struct LOG_CONFIG {
		UNICODE_STRING log_dir;        
		UNICODE_STRING log_name_prefix; 
		ULONG max_file_size;           
		ULONG max_file_count;          
	};

	struct LOG_CONTEXT {
		volatile long lock_flag;
		HANDLE file_handle;
		ULONG current_file_size;
		ULONG current_file_index;
		LOG_CONFIG config;
	};

	extern LOG_CONTEXT g_log_ctx;

	// 初始化日志系统
	NTSTATUS init_log_system(
		const wchar_t* log_dir,
		const wchar_t* log_name_prefix,
		ULONG max_file_size,
		ULONG max_file_count);

	NTSTATUS open_log_file();
	// 写日志接口
	void LogPrint(__log_type type, const char* fmt, ...);

	// 卸载/关闭日志系统
	void shutdown_log_system();

} // namespace logger
 
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