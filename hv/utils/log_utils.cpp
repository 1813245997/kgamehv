#include "global_defs.h"
#include "log_utils.h"
#include <ntstrsafe.h>

// ------------------------- 实现 -------------------------

namespace logger {

	LOG_CONTEXT g_log_ctx = {};

	// 初始化日志系统
	NTSTATUS init_log_system(
		const wchar_t* log_dir,
		const wchar_t* log_name_prefix,
		ULONG max_file_size,
		ULONG max_file_count)
	{
		RtlZeroMemory(&g_log_ctx, sizeof(g_log_ctx));
		 

		g_log_ctx.config.max_file_size = max_file_size;
		g_log_ctx.config.max_file_count = max_file_count;

		RtlInitUnicodeString(&g_log_ctx.config.log_dir, log_dir);
		RtlInitUnicodeString(&g_log_ctx.config.log_name_prefix, log_name_prefix);

		// 创建日志目录
		OBJECT_ATTRIBUTES objAttr;
		IO_STATUS_BLOCK iosb;
		HANDLE dir_handle;

		InitializeObjectAttributes(&objAttr,
			&g_log_ctx.config.log_dir,
			OBJ_KERNEL_HANDLE | OBJ_CASE_INSENSITIVE,
			nullptr,
			nullptr);

		NTSTATUS status = ZwCreateFile(
			&dir_handle,
			FILE_GENERIC_WRITE | FILE_GENERIC_READ | SYNCHRONIZE,
			&objAttr,
			&iosb,
			nullptr,
			FILE_ATTRIBUTE_DIRECTORY,
			FILE_SHARE_READ | FILE_SHARE_WRITE,
			FILE_OPEN_IF,
			FILE_DIRECTORY_FILE | FILE_SYNCHRONOUS_IO_NONALERT,
			nullptr,
			0
		);

		if (!NT_SUCCESS(status)) {
			return status;
		}

		ZwClose(dir_handle);

		return open_log_file();
	}
	// 内部辅助函数：关闭当前日志文件
	void close_log_file()
	{
		if (g_log_ctx.file_handle) {
			ZwClose(g_log_ctx.file_handle);
			g_log_ctx.file_handle = nullptr;
		}
	}

	// 内部辅助函数：打开当前日志文件
	NTSTATUS open_log_file()
	{
		WCHAR path_buffer[512] = {}; // 512 WCHAR，一般足够存放日志目录 + 文件名
		swprintf_s(path_buffer, ARRAY_SIZE(path_buffer), L"%wZ\\%wZ_%lu.log",
			&g_log_ctx.config.log_dir,
			&g_log_ctx.config.log_name_prefix,
			g_log_ctx.current_file_index);

		UNICODE_STRING file_path;
		RtlInitUnicodeString(&file_path, path_buffer);

		OBJECT_ATTRIBUTES objAttr;
		InitializeObjectAttributes(&objAttr,
			&file_path,
			OBJ_KERNEL_HANDLE | OBJ_CASE_INSENSITIVE,
			nullptr,
			nullptr);

		IO_STATUS_BLOCK iosb;
		NTSTATUS status = ZwCreateFile(
			&g_log_ctx.file_handle,
			FILE_GENERIC_WRITE | SYNCHRONIZE,
			&objAttr,
			&iosb,
			nullptr,
			FILE_ATTRIBUTE_NORMAL,
			FILE_SHARE_READ,
			FILE_OVERWRITE_IF,
			FILE_SYNCHRONOUS_IO_NONALERT | FILE_NON_DIRECTORY_FILE,
			nullptr,
			0
		);

		if (NT_SUCCESS(status)) {
			g_log_ctx.current_file_size = 0;
		}

		 
		return status;
	}


	// 内部辅助函数：删除最旧日志文件
	NTSTATUS delete_oldest_log_file()
	{
		WCHAR path_buffer[512] = {};

		swprintf_s(path_buffer, ARRAY_SIZE(path_buffer), L"%wZ\\%wZ_%lu.log",
			&g_log_ctx.config.log_dir,
			&g_log_ctx.config.log_name_prefix,
			g_log_ctx.current_file_index);

 

		UNICODE_STRING file_path;
		RtlInitUnicodeString(&file_path, path_buffer);

		OBJECT_ATTRIBUTES objAttr;
		InitializeObjectAttributes(&objAttr, &file_path,
			OBJ_KERNEL_HANDLE | OBJ_CASE_INSENSITIVE,
			nullptr, nullptr);

		NTSTATUS status = ZwDeleteFile(&objAttr);
		 
		return status;
	}

	// 滚动日志文件
	NTSTATUS roll_log_file_if_needed()
	{
		if (g_log_ctx.current_file_size >= g_log_ctx.config.max_file_size) {
			close_log_file();

			// 循环下一个文件编号
			g_log_ctx.current_file_index = (g_log_ctx.current_file_index + 1) % g_log_ctx.config.max_file_count;

			// 删除旧文件保证数量不超限
			delete_oldest_log_file();

			return open_log_file();
		}
		return STATUS_SUCCESS;
	}

	 
	// 写日志实现
	void LogPrint(__log_type type, const char* fmt, ...)
	{
		char message_buffer[1024] = {};
		char final_buffer[2048] = {};
		const char* log_type_str = nullptr;

		switch (type) {
		case LOG_TYPE_DEBUG: log_type_str = "[DEBUG]"; break;
		case LOG_TYPE_DUMP:  log_type_str = "[DUMP]";  break;
		case LOG_TYPE_ERROR: log_type_str = "[ERROR]"; break;
		case LOG_TYPE_INFO:  log_type_str = "[INFO]";  break;
		default: log_type_str = "[UNKNOWN]"; break;
		}

		va_list args;
		va_start(args, fmt);
		RtlStringCchVPrintfA(message_buffer, sizeof(message_buffer), fmt, args);
		va_end(args);

		LARGE_INTEGER system_time = {};
		TIME_FIELDS tf = {};
		KeQuerySystemTime(&system_time);
		ExSystemTimeToLocalTime(&system_time, &system_time);
		RtlTimeToTimeFields(&system_time, &tf);

		RtlStringCchPrintfA(final_buffer, sizeof(final_buffer),
			"[%02hd:%02hd:%02hd.%03hd]%s  %s\r\n",
			tf.Hour, tf.Minute, tf.Second, tf.Milliseconds,
			log_type_str, message_buffer);

		spinlock lock(&g_log_ctx.lock_flag);

		roll_log_file_if_needed();

		if (g_log_ctx.file_handle) {
			IO_STATUS_BLOCK iosb;
			ZwWriteFile(g_log_ctx.file_handle, nullptr, nullptr, nullptr, &iosb,
				final_buffer, (ULONG)strlen(final_buffer), nullptr, nullptr);
			g_log_ctx.current_file_size += (ULONG)strlen(final_buffer);
		}

		 
	}

	// 关闭日志系统
	void shutdown_log_system()
	{
		spinlock lock(&g_log_ctx.lock_flag);
		close_log_file();
		 
	}

}
 
