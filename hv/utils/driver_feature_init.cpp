#include "global_defs.h"
#include "driver_feature_init.h"
#include "..\hv.h"


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

			LogDebug("Driver loaded.");

			LogDebug("Initializing ntoskrnl info...");
			if (!utils::module_info::init_ntoskrnl_info())
			{
				LogError("Failed to initialize ntoskrnl info.");
				return STATUS_UNSUCCESSFUL;
			}
			LogDebug("ntoskrnl info initialized successfully.");
			utils::global::initialize_all_globals();

			LogDebug("Getting module info from context...");
			if (!get_module_info_from_context(context, module_base, image_size))
			{
				LogError("Failed to get module info from context.");
				return STATUS_INVALID_IMAGE_FORMAT;
			}
		 
			utils::hidden_modules::set_driver_info(reinterpret_cast<unsigned long long>(module_base), image_size);
			LogDebug( 
				" Driver Base: 0x%p, Image Size: 0x%llX\n",
				module_base,
				static_cast<ULONGLONG>(image_size));
			DbgPrintEx(77,0,
				" Driver Base: 0x%p, Image Size: 0x%llX\n",
				module_base,
				static_cast<ULONGLONG>(image_size));
			LogDebug("Initializing internal functions...");
			NTSTATUS status = internal_functions::initialize_internal_functions();
			if (!NT_SUCCESS(status))
			{
				LogError("Failed to initialize internal functions (0x%X).", status);
				return status;
			}
			LogDebug("Internal functions initialized successfully.");

			LogDebug("Initializing paging base addresses...");
			status = memory::initialize_all_paging_base();
			if (!NT_SUCCESS(status))
			{
				LogError("Failed to initialize paging base addresses (0x%X).", status);
				return status;
			}
			LogDebug("Paging base addresses initialized successfully.");

			LogDebug("Initializing feature globals...");
			status = feature_data::initialize();
			if (!NT_SUCCESS(status))
			{
				LogError("Failed to initialize feature globals (0x%X).", status);
				return status;
			}
			LogDebug("Feature globals initialized successfully.");

			LogDebug("Initializing feature offsets...");
			status = feature_offset::initialize();
			if (!NT_SUCCESS(status))
			{
				LogError("Failed to initialize feature offsets (0x%X).", status);
				return status;
			}
			LogDebug("Feature offsets initialized successfully.");

			game::kcsgo2::initialize_player_data_lock();
			KMenuConfig::  initialize_visual_config_once();
			LogDebug("Starting virtualization...");
			if (!hv::start())
			{
				LogError("Failed to virtualize system.");
				return STATUS_HV_OPERATION_FAILED;
			}
			LogDebug("Virtualization started successfully.");

			LogDebug("Pinging hypervisor...");
			if (ping() == hv::hypervisor_signature)
			{
				LogDebug("Hypervisor signature matches.");
			}
			else
			{
				LogError("Failed to ping hypervisor!");
			}

			utils::hidden_modules::initialize_hidden_module_list();

			utils::hidden_modules::add_hidden_module(module_base, image_size, L"MyHiddenModule");

			utils::hidden_user_memory::initialize_hidden_user_memory();


			LogDebug("Initializing hooks...");
			status = hook_manager::initialize_all_hooks();
			if (!NT_SUCCESS(status))
			{
				LogError("Failed to initialize hooks (0x%X).", status);
				return status;
			}
			LogDebug("Hooks initialized successfully.");


			LogDebug("Initializing DWM draw support...");
			status = utils::dwm_draw::initialize();
			if (!NT_SUCCESS(status))
			{
				LogError("Failed to initialize DWM draw (0x%X).", status);
				return status;
			}
			LogDebug("DWM draw initialized successfully.");


			LogDebug("Driver initialization complete.");
				 
		 
			return STATUS_SUCCESS;
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
			return true;
		}


	}
}
