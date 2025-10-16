#include "global_defs.h"
#include "driver_feature_init.h"
  

 

namespace utils
{
	namespace driver_features
	{
		void kunordered_map_usage_examples() {
			// 1. 基本使用 - 整数键值对
			utils::kunordered_map<int, int> int_map;

			// 插入数据
			int_map[1] = 100;
			int_map[2] = 200;
			int_map[3] = 300;

			// 访问数据
			int value = int_map[2];  // value = 200

			// 查找数据
			auto it = int_map.find(1);
			if (it != int_map.end()) {
				// 找到了，it->first 是键，it->second 是值
				int key = it->first;    // key = 1
				int val = it->second;   // val = 100
			}

			// 2. 字符串键值对
			utils::kunordered_map<const char*, int> string_map;

			string_map["apple"] = 10;
			string_map["banana"] = 20;
			string_map["orange"] = 30;

			// 查找字符串键
			auto str_it = string_map.find("banana");
			if (str_it != string_map.end()) {
				int count = str_it->second;  // count = 20
			}

			// 3. 自定义类型键值对
			struct ProcessInfo {
				ULONG pid;
				ULONG64 base_address;
				char name[64];

				ProcessInfo() : pid(0), base_address(0) {
					RtlZeroMemory(name, sizeof(name));
				}

				ProcessInfo(ULONG p, ULONG64 addr, const char* n) : pid(p), base_address(addr) {
					RtlZeroMemory(name, sizeof(name));
					if (n) {
						RtlCopyMemory(name, n, min(strlen(n), sizeof(name) - 1));
					}
				}
			};

			utils::kunordered_map<ULONG, ProcessInfo> process_map;

			// 插入进程信息
			ProcessInfo info1(1234, 0x400000, "notepad.exe");
			ProcessInfo info2(5678, 0x500000, "calc.exe");

			process_map[1234] = info1;
			process_map[5678] = info2;

			// 查找进程
			auto proc_it = process_map.find(1234);
			if (proc_it != process_map.end()) {
				ProcessInfo& proc = proc_it->second;
				// 使用 proc.pid, proc.base_address, proc.name
			}

			// 4. 遍历所有元素
			for (auto& pair : int_map) {
				int key = pair.first;
				int val = pair.second;
				// 处理每个键值对
			}

			// 5. 删除元素
			int_map.erase(2);  // 删除键为2的元素

			// 6. 检查容器状态
			bool is_empty = int_map.empty();
			size_t size = int_map.size();
			size_t buckets = int_map.bucket_count();

			// 7. 清空容器
			int_map.clear();

			// 8. 使用自定义哈希函数
			struct CustomHash {
				size_t operator()(const char* key) const {
					if (!key) return 0;
					size_t hash = 0;
					while (*key) {
						hash = hash * 31 + *key++;
					}
					return hash;
				}
			};

			utils::kunordered_map<const char*, int, CustomHash> custom_map;
			custom_map["test"] = 42;
		}

		// 在驱动中使用的高级示例
		void driver_usage_example() {
			// 用于存储进程ID到进程对象的映射
			utils::kunordered_map<ULONG, PEPROCESS> process_map;

			// 用于存储模块基址到模块信息的映射
			struct ModuleInfo {
				ULONG64 base_address;
				ULONG size;
				char name[256];
				bool is_hidden;
			};

			utils::kunordered_map<ULONG64, ModuleInfo> module_map;

			// 用于存储钩子信息的映射
			struct HookInfo {
				PVOID original_address;
				PVOID hook_address;
				UCHAR original_bytes[16];
				bool is_active;
			};

			utils::kunordered_map<PVOID, HookInfo> hook_map;

			// 使用示例：
			// 1. 注册进程
			PEPROCESS process = nullptr;
			// ... 获取进程对象 ...
			process_map[1234] = process;

			// 2. 注册模块
			ModuleInfo mod_info;
			mod_info.base_address = 0x400000;
			mod_info.size = 0x1000;
			RtlCopyMemory(mod_info.name, "ntdll.dll", 10);
			mod_info.is_hidden = false;

			module_map[0x400000] = mod_info;

			// 3. 注册钩子
			HookInfo hook_info;
			hook_info.original_address = (PVOID)0x401000;
			hook_info.hook_address = (PVOID)0x500000;
			hook_info.is_active = true;
			// ... 保存原始字节 ...

			hook_map[(PVOID)0x401000] = hook_info;

			// 4. 查找和操作
			auto proc_it = process_map.find(1234);
			if (proc_it != process_map.end()) {
				PEPROCESS found_process = proc_it->second;
				// 使用找到的进程对象
			}

			// 5. 遍历所有钩子
			for (auto& hook_pair : hook_map) {
				PVOID addr = hook_pair.first;
				HookInfo& info = hook_pair.second;

				if (info.is_active) {
					// 处理活跃的钩子
				}
			}

			// 6. 清理（在驱动卸载时）
			process_map.clear();
			module_map.clear();
			hook_map.clear();
		}

		// 性能测试示例
		void performance_test_example() {
			utils::kunordered_map<ULONG, ULONG> test_map;

			// 插入大量数据
			for (ULONG i = 0; i < 10000; ++i) {
				test_map[i] = i * 2;
			}

			// 查找测试
			for (ULONG i = 0; i < 1000; ++i) {
				auto it = test_map.find(i * 10);
				if (it != test_map.end()) {
					// 找到数据
				}
			}

			// 删除测试
			for (ULONG i = 0; i < 5000; ++i) {
				test_map.erase(i);
			}
		}


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
			 DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, "Driver Base: 0x%p, Image Size: 0x%llX\n", module_base, static_cast<ULONGLONG>(image_size));
			 
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

			LogInfo("Initializing mouse class...");
			status = utils::mouse_class::initialize_mouse_class();
			if (!NT_SUCCESS(status))
			{
				LogError("Failed to initialize mouse class (0x%X).", status);
				//  VMProtectEnd();
				return status;
			}
			LogInfo("Mouse class initialized successfully.");

			LogInfo("Initializing key board class...");
			status = utils::key_board_class::initialize_key_board_class();
			if (!NT_SUCCESS(status))
			{
				LogError("Failed to initialize key board class (0x%X).", status);
				//  VMProtectEnd();
				return status;
			}
			LogInfo("Key board class initialized successfully.");

			LogInfo("Initializing dwm drawing...");
			status = utils::kernel_dwm_drawing::initialize_dwm_drawing();
			if (!NT_SUCCESS(status))
			{
				LogError("Failed to initialize dwm drawing (0x%X).", status);
				//  VMProtectEnd();
				return status;
			}
			LogInfo("Dwm drawing initialized successfully.");


			DbgBreakPoint();
			kunordered_map_usage_examples();


		 
		
			    
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
