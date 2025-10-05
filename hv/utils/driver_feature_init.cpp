#include "global_defs.h"
#include "driver_feature_init.h"
  

 

namespace utils
{
	namespace driver_features
	{
	 

		NTSTATUS initialize_all_features(IN PVOID context)
		{
		   //VMProtectBegin("utils::driver_features::initialize_all_features");
			PVOID module_base = nullptr;
			SIZE_T image_size = {};

			 
		 
			NTSTATUS status = logger::init_log_system(
				L"\\SystemRoot\\Logs\\DriverLogs",   
				L"MyDriver",
				1024 * 1024*1,  
				5
			);
			if (!NT_SUCCESS(status)) {
				return status;
			}
			 
			LogInfo("Driver loaded.");

			LogInfo("Initializing ntoskrnl info...");
			if (!utils::module_info::init_ntoskrnl_info())
			{
				LogError("Failed to initialize ntoskrnl info.");
			  // VMProtectEnd();
				return STATUS_UNSUCCESSFUL;
			}
			LogInfo("ntoskrnl info initialized successfully.");
			utils::global::initialize_all_globals();

			LogInfo("Getting module info from context...");
			if (!get_module_info_from_context(context, module_base, image_size))
			{
				LogError("Failed to get module info from context.");
			   // VMProtectEnd();
				return STATUS_INVALID_IMAGE_FORMAT;
			}
		 
			utils::hidden_modules::set_driver_info(reinterpret_cast<unsigned long long>(module_base), image_size);
			LogInfo( 
				" Driver Base: 0x%p, Image Size: 0x%llX\n",
				module_base,
				static_cast<ULONGLONG>(image_size));
			 
			LogInfo("Initializing internal functions...");
			  status = internal_functions::initialize_internal_functions();
			if (!NT_SUCCESS(status))
			{
				LogError("Failed to initialize internal functions (0x%X).", status);
			 	 //  VMProtectEnd();
				return status;
			}
			LogInfo("Internal functions initialized successfully.");

			LogInfo("Initializing paging base addresses...");
			status = memory::initialize_all_paging_base();
			if (!NT_SUCCESS(status))
			{
				LogError("Failed to initialize paging base addresses (0x%X).", status);
			  //   VMProtectEnd();
				return status;
			}
			LogInfo("Paging base addresses initialized successfully.");

			LogInfo("Initializing feature globals...");
			

			 
			status = feature_data::initialize();
			if (!NT_SUCCESS(status))
			{
				LogError("Failed to initialize feature globals (0x%X).", status);
				//VMProtectEnd();
				return status;
			}
			LogInfo("Feature globals initialized successfully.");

			LogInfo("Initializing feature offsets...");
			status = feature_offset::initialize();
			if (!NT_SUCCESS(status))
			{
				LogError("Failed to initialize feature offsets (0x%X).", status);
				//   VMProtectEnd();
				return status;
			}
			LogInfo("Feature offsets initialized successfully.");

			LogInfo("Starting virtualization...");
			status = khyper_vt::initialize_khyper_vt();
			if (!NT_SUCCESS(status))
			{
				LogError("Failed to initialize hooks (0x%X).", status);
				//   VMProtectEnd();
				return status;
			}
			LogInfo("Virtualization started successfully.");
 
			utils::hidden_modules::initialize_hidden_module_list();

			utils::hidden_modules::add_hidden_module(module_base, image_size, L"MyHiddenModule");

			utils::hidden_user_memory::initialize_hidden_user_memory();

			status = utils::module_info::initialize_all_user_modules();
			if (!NT_SUCCESS(status))
			{
				LogError("Failed to initialize user modules (0x%X).", status);
				//  VMProtectEnd();
				return status;
			}
			LogInfo("User modules initialized successfully.");

			LogInfo("Initializing user call utils...");
			status = utils::user_call::initialize_user_call_utils();
			if (!NT_SUCCESS(status))
			{
				LogError("Failed to initialize user call utils (0x%X).", status);
				//  VMProtectEnd();
				return status;
			}
			LogInfo("User call utils initialized successfully.");

			LogInfo("Initializing game cheat...");
			status = game::game_cheat::initialize_game_cheat( GameType::DeltaForce);
			if (!NT_SUCCESS(status))
			{
				LogError("Failed to initialize game cheat (0x%X).", status);
				//  VMProtectEnd();
				return status;
			}
			LogInfo("Game cheat initialized successfully.");

			LogInfo("Initializing hooks...");
			status = hook_manager::initialize_all_hooks();
			if (!NT_SUCCESS(status))
			{
				LogError("Failed to initialize hooks (0x%X).", status);
				//  VMProtectEnd();
				return status;
			}
			LogInfo("Hooks initialized successfully.");

			LogInfo("Initializing dwm drawing...");
			status = utils::kernel_dwm_drawing::initialize_dwm_drawing();
			if (!NT_SUCCESS(status))
			{
				LogError("Failed to initialize dwm drawing (0x%X).", status);
				//  VMProtectEnd();
				return status;
			}
			LogInfo("Dwm drawing initialized successfully.");
			    
			LogInfo("Driver initialization complete.");
			 	 
		  // VMProtectEnd();
			return STATUS_SUCCESS;
		}





 

		bool get_module_info_from_context(PVOID context, PVOID& module_base, SIZE_T& image_size)
		{
#if USE_MANUAL_MAP_MODE == 1
		 
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
			 
			auto driver_object = static_cast<PDRIVER_OBJECT>(context);
			module_base = driver_object->DriverStart;
			image_size = driver_object->DriverSize;
#endif

			 
			return true;
		}


	}
}
