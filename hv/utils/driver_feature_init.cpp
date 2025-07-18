#include "global_defs.h"
#include "driver_feature_init.h"
#include "..\hv.h"
// #define USE_MANUAL_MAP_MODE 1

namespace utils
{
	namespace driver_features
	{
		static uint64_t ping() {
			hv::hypercall_input input;
			input.code = hv::hypercall_ping;
			input.key = hv::hypercall_key;
			return hv::vmx_vmcall(input);
		}

		NTSTATUS initialize_all_features(IN PVOID context)
		{
			PVOID module_base = nullptr;
			SIZE_T image_size = {};

			DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, "[hv] Driver loaded.\n");

			// 初始化 ntoskrnl 导出表信息
			DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, "[hv] Initializing ntoskrnl info...\n");
			if (!utils::module_info::init_ntoskrnl_info())
			{
				DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, "[hv] Failed to initialize ntoskrnl info.\n");
				return STATUS_UNSUCCESSFUL;
			}
			DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, "[hv] ntoskrnl info initialized successfully.\n");

			// 初始化内部函数地址
			DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, "[hv] Initializing internal functions...\n");
			NTSTATUS status = internal_functions::initialize_internal_functions();
			if (!NT_SUCCESS(status))
			{
				DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, "[hv] Failed to initialize internal functions (0x%X).\n", status);
				return status;
			}
			DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, "[hv] Internal functions initialized successfully.\n");

			DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, "[hv] Initializing paging base addresses...\n");
			status = memory::initialize_all_paging_base();
			if (!NT_SUCCESS(status))
			{
				DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, "[hv] Failed to initialize paging base addresses (0x%X).\n", status);
				return status;
			}
			DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, "[hv] Paging base addresses initialized successfully.\n");

			DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, "[hv] Initializing feature globals...\n");
			status = feature_data::initialize();
			if (!NT_SUCCESS(status))
			{
				DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL,
					"[hv] Failed to initialize feature globals (0x%X).\n", status);
				return status;
			}
			DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, "[hv] Feature globals initialized successfully.\n");

			 
			DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, "[hv] Initializing feature offsets...\n");

			status = feature_offset::initialize();
			if (!NT_SUCCESS(status))
			{
				DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL,
					"[hv] Failed to initialize feature offsets (0x%X).\n", status);
				return status;
			}
			DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, "[hv] Feature offsets initialized successfully.\n");

			
			game::kcsgo2::initialize_player_data_lock();
			
			// 启动虚拟化
			DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, "[hv] Starting virtualization...\n");
			if (!hv::start())
			{
				DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, "[hv] Failed to virtualize system.\n");
				return STATUS_HV_OPERATION_FAILED;
			}
			DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, "[hv] Virtualization started successfully.\n");

			// ping hypervisor 确认是否成功加载
			DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, "[hv] Pinging hypervisor...\n");
			if (ping() == hv::hypervisor_signature)
			{
				DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, "[client] Hypervisor signature matches.\n");
			}
			else
			{
				DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, "[client] Failed to ping hypervisor!\n");
			}

			// 初始化隐藏模块链表
			utils::hidden_modules::initialize_hidden_module_list();
			// 获取驱动模块信息（基址与大小）
			DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, "[hv] Getting module info from context...\n");
			if (!get_module_info_from_context(context, module_base, image_size))
			{
				DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, "[hv] Failed to get module info from context.\n");
				return STATUS_INVALID_IMAGE_FORMAT;
			}
			 

			// 添加当前驱动模块为隐藏模块
			 
			utils::hidden_modules::add_hidden_module(module_base, image_size, L"MyHiddenModule");
		    
			//初始化 R3内存链表
			utils::hidden_user_memory::initialize_hidden_user_memory();

		

			// 初始化所有 HOOK
			DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, "[hv] Initializing hooks...\n");
			status = hook_manager::initialize_all_hooks();
			if (!NT_SUCCESS(status))
			{
				DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, "[hv] Failed to initialize hooks (0x%X).\n", status);
				return status;
			}
			DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, "[hv] Hooks initialized successfully.\n");
			 
			// 初始化 DWM 绘制支持
			DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, "[hv] Initializing DWM draw support...\n");
			status = utils::dwm_draw::initialize();
			if (!NT_SUCCESS(status))
			{
				DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, "[hv] Failed to initialize DWM draw (0x%X).\n", status);
				return status;
			}
			DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, "[hv] DWM draw initialized successfully.\n");


			DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, "[hv] Driver initialization complete.\n");

			return STATUS_SUCCESS;
		}







		bool get_module_info_from_context(PVOID context, PVOID& module_base, SIZE_T& image_size)
		{
#ifdef USE_MANUAL_MAP_MODE
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
			return true;
		}


	}
}
