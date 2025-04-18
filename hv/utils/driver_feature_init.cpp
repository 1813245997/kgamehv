#include "global_defs.h"
#include "driver_feature_init.h"
#include "..\hv.h"
//#define USE_MANUAL_MAP_MODE 1

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

			DbgPrint("[hv] Driver loaded.\n");
			DbgBreakPoint();
			// 初始化 ntoskrnl 导出表信息
			if (!utils::module_info::init_ntoskrnl_info())
			{
				DbgPrint("[hv] Failed to initialize ntoskrnl info.\n");
				return STATUS_UNSUCCESSFUL;
			}

			// 初始化内部函数地址
			NTSTATUS status = internal_functions::initialize_internal_functions();
			if (!NT_SUCCESS(status))
			{
				DbgPrint("[hv] Failed to initialize internal functions.\n");
				return status;
			}

			// 启动虚拟化
			if (!hv::start())
			{
				DbgPrint("[hv] Failed to virtualize system.\n");
				return STATUS_HV_OPERATION_FAILED;
			}

			// ping hypervisor 确认是否成功加载
			if (ping() == hv::hypervisor_signature)
			{
				DbgPrint("[client] Hypervisor signature matches.\n");
			}
			else
			{
				DbgPrint("[client] Failed to ping hypervisor!\n");
			}

			// 初始化隐藏模块链表
			utils::hidden_modules::initialize_hidden_module_list();

			// 获取驱动模块信息（基址与大小）
			if (!get_module_info_from_context(context, module_base, image_size))
			{
				DbgPrint("[hv] Failed to get module info from context.\n");
				return STATUS_INVALID_IMAGE_FORMAT;
			}

			// 添加当前驱动模块为隐藏模块
			utils::hidden_modules::add_hidden_module(module_base, image_size, L"MyHiddenModule");

			// 初始化所有 HOOK
			status = hook_manager::initialize_all_hooks();
			if (!NT_SUCCESS(status))
			{
				DbgPrint("[hv] Failed to initialize hooks.\n");
				return status;
			}

			DbgPrint("[hv] Driver initialization complete.\n");

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
