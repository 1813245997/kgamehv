#include "global_defs.h"
#include "driver_feature_init.h"
  
#include <ntstrsafe.h>
#include "dll_bin.h"

namespace utils
{
	namespace driver_features
	{
		/*static uint64_t ping() {
			hv::hypercall_input input;
			input.code = hv::hypercall_ping;
			input.key = hv::hypercall_key;
			return hv::vmx_vmcall(input);
		}*/

		NTSTATUS initialize_all_features(IN PVOID context)
		{
		   //VMProtectBegin("utils::driver_features::initialize_all_features");
			PVOID module_base = nullptr;
			SIZE_T image_size = {};

			 
			 
			// 初始化日志系统
			// 参数依次：日志目录，单文件最大大小（字节），最大日志文件数量
			NTSTATUS status = logger::init_log_system(
				L"\\SystemRoot\\Logs\\DriverLogs",  // 使用 SystemRoot
				L"MyDriver",
				1024 * 1024,  // 1MB
				5
			);
			if (!NT_SUCCESS(status)) {
				return status;
			}
			 
			LogDebug("Driver loaded.");

			LogDebug("Initializing ntoskrnl info...");
			if (!utils::module_info::init_ntoskrnl_info())
			{
				LogError("Failed to initialize ntoskrnl info.");
			  // VMProtectEnd();
				return STATUS_UNSUCCESSFUL;
			}
			LogDebug("ntoskrnl info initialized successfully.");
			utils::global::initialize_all_globals();

			LogDebug("Getting module info from context...");
			if (!get_module_info_from_context(context, module_base, image_size))
			{
				LogError("Failed to get module info from context.");
			   // VMProtectEnd();
				return STATUS_INVALID_IMAGE_FORMAT;
			}
		 
			utils::hidden_modules::set_driver_info(reinterpret_cast<unsigned long long>(module_base), image_size);
			LogDebug( 
				" Driver Base: 0x%p, Image Size: 0x%llX\n",
				module_base,
				static_cast<ULONGLONG>(image_size));
			 
			LogDebug("Initializing internal functions...");
			  status = internal_functions::initialize_internal_functions();
			if (!NT_SUCCESS(status))
			{
				LogError("Failed to initialize internal functions (0x%X).", status);
			 	 //  VMProtectEnd();
				return status;
			}
			LogDebug("Internal functions initialized successfully.");

			LogDebug("Initializing paging base addresses...");
			status = memory::initialize_all_paging_base();
			if (!NT_SUCCESS(status))
			{
				LogError("Failed to initialize paging base addresses (0x%X).", status);
			  //   VMProtectEnd();
				return status;
			}
			LogDebug("Paging base addresses initialized successfully.");

			LogDebug("Initializing feature globals...");
			 
			status = feature_data::initialize();
			if (!NT_SUCCESS(status))
			{
				LogError("Failed to initialize feature globals (0x%X).", status);
				//VMProtectEnd();
				return status;
			}
			LogDebug("Feature globals initialized successfully.");

			LogDebug("Initializing feature offsets...");
			status = feature_offset::initialize();
			if (!NT_SUCCESS(status))
			{
				LogError("Failed to initialize feature offsets (0x%X).", status);
				//   VMProtectEnd();
				return status;
			}
			LogDebug("Feature offsets initialized successfully.");



			game::kcsgo2::initialize_player_data_lock();
			config::initialize_visual_config_once();


			/*LogDebug("Process manager init_process_manager.");
			status = utils::process::init_process_manager();
			if (!NT_SUCCESS(status))
			{
				LogError("Failed to initialize process manager (0x%X).", status);
				return status;
			}
			LogDebug("Process manager initialized successfully.");*/


			LogDebug("Starting virtualization...");
			status = khyper_vt::initialize_khyper_vt();
			if (!NT_SUCCESS(status))
			{
				LogError("Failed to initialize hooks (0x%X).", status);
				//   VMProtectEnd();
				return status;
			}
			LogDebug("Virtualization started successfully.");
 
			utils::hidden_modules::initialize_hidden_module_list();

			utils::hidden_modules::add_hidden_module(module_base, image_size, L"MyHiddenModule");

			utils::hidden_user_memory::initialize_hidden_user_memory();


			LogDebug("Initializing hooks...");
			status = hook_manager::initialize_all_hooks();
			if (!NT_SUCCESS(status))
			{
				LogError("Failed to initialize hooks (0x%X).", status);
				//  VMProtectEnd();
				return status;
			}
			LogDebug("Hooks initialized successfully.");

			LogDebug("Initializing DWM draw support...");
			status = utils::dwm_draw::initialize();
			if (!NT_SUCCESS(status))
			{
				LogError("Failed to initialize DWM draw (0x%X).", status);
				//  VMProtectEnd();
				return status;
			}
			LogDebug( "DWM draw initialized successfully.");

			/*	DbgBreakPoint();
				PEPROCESS process{};
				HANDLE proces_id = (HANDLE)2504;

				PsLookupProcessByProcessId(proces_id, &process);

				PVOID A;
				utils::hook_utils::hook_user_hook_handler(process, (void*)0x7FF738F42E10, (void*)0x7FF738F42E70, &A);*/
			
			//utils::dwm_draw::test();
			
		  

			LogDebug("Driver initialization complete.");
			 	 
		  // VMProtectEnd();
			return STATUS_SUCCESS;
		}





		// 写入模块信息到文件
		void write_module_info_log(PVOID base, SIZE_T size)
		{
			UNICODE_STRING log_dir = RTL_CONSTANT_STRING(L"\\??\\C:\\Log");
			UNICODE_STRING log_path = RTL_CONSTANT_STRING(L"\\??\\C:\\Log\\driver_module_info.log");
			OBJECT_ATTRIBUTES obj_attr;
			IO_STATUS_BLOCK io_status;

			// 确保目录存在
			HANDLE dir_handle = nullptr;
			InitializeObjectAttributes(&obj_attr, &log_dir, OBJ_KERNEL_HANDLE | OBJ_CASE_INSENSITIVE, nullptr, nullptr);
			ZwCreateFile(
				&dir_handle,
				FILE_LIST_DIRECTORY | SYNCHRONIZE,
				&obj_attr,
				&io_status,
				nullptr,
				FILE_ATTRIBUTE_DIRECTORY,
				FILE_SHARE_READ | FILE_SHARE_WRITE,
				FILE_OPEN_IF,
				FILE_DIRECTORY_FILE | FILE_SYNCHRONOUS_IO_NONALERT,
				nullptr,
				0
			);
			if (dir_handle) ZwClose(dir_handle);

			// 打开并覆盖日志文件
			HANDLE file_handle = nullptr;
			InitializeObjectAttributes(&obj_attr, &log_path, OBJ_KERNEL_HANDLE | OBJ_CASE_INSENSITIVE, nullptr, nullptr);
			NTSTATUS status = ZwCreateFile(
				&file_handle,
				GENERIC_WRITE | SYNCHRONIZE,
				&obj_attr,
				&io_status,
				nullptr,
				FILE_ATTRIBUTE_NORMAL,
				FILE_SHARE_READ | FILE_SHARE_WRITE,
				FILE_OVERWRITE_IF,  // 每次写入都清空原文件
				FILE_NON_DIRECTORY_FILE | FILE_SYNCHRONOUS_IO_NONALERT,
				nullptr,
				0
			);
			
			if (!NT_SUCCESS(status))
				return;

			// 写入内容
			CHAR buffer[128]{};
			SIZE_T len = sprintf_s(buffer, sizeof(buffer),
				"ModuleBase: 0x%p, ImageSize: 0x%llX\r\n", base, static_cast<ULONGLONG>(size));

			ZwWriteFile(
				file_handle,
				nullptr,
				nullptr,
				nullptr,
				&io_status,
				buffer,
				static_cast<ULONG>(len),
				nullptr,
				nullptr
			);

			ZwClose(file_handle);
		}

		bool get_module_info_from_context(PVOID context, PVOID& module_base, SIZE_T& image_size)
		{
#if USE_MANUAL_MAP_MODE == 1
			// 手动映射模式
			module_base = context;

			auto dos_header = reinterpret_cast<PIMAGE_DOS_HEADER>(module_base);
			if (dos_header->e_magic != IMAGE_DOS_SIGNATURE)
				return false;

			auto nt_headers = reinterpret_cast<PIMAGE_NT_HEADERS64>(
				reinterpret_cast<PUCHAR>(dos_header) + dos_header->e_lfanew);

			if (nt_headers->Signature != IMAGE_NT_SIGNATURE)
				return false;

			image_size = nt_headers->OptionalHeader.SizeOfImage;
#else
			// 常规驱动对象模式
			auto driver_object = static_cast<PDRIVER_OBJECT>(context);
			module_base = driver_object->DriverStart;
			image_size = driver_object->DriverSize;
#endif

			write_module_info_log(module_base, image_size);
			return true;
		}


	}
}
