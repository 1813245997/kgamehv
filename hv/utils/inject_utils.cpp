#include "global_defs.h"
#include "MemLoadDll.h"
#include "inject_utils.h"

namespace utils
{
	namespace inject_utils
	{
		typedef struct _freeitem_info
		{
			WORK_QUEUE_ITEM work_item;       // ԭ workitem
			HANDLE pid;                      // ���� pid
			ULONG64 is_execute_addr;         // ԭ IsExecuteAddr
			ULONG64 base_addr_free_size;     // ԭ BaseAddrfreeSize
			ULONG64 dll_base;                // ԭ Dllbase
			ULONG64 dll_base_size;           // ԭ DllbaseSize
			ULONG64 dll_images;              // ԭ DLLimags
		} freeitem_info, * p_freeitem_info;

	
		NTSTATUS hijack_thread_inject_dll_x64(HANDLE process_id, PVOID dll_shellcode, ULONG dll_size)
		{


			PEPROCESS target_process{};
			NTSTATUS status = STATUS_SUCCESS;
			PUCHAR base_addr{};
			PUCHAR dll_addr{};
			PUCHAR image_addr{};
			PUCHAR previous_mode_ptr = nullptr;
			ULONG64 init_kernel = 0;
			PKTRAP_FRAME k_trap = nullptr;
			ULONG64 old_rip = 0;
			PUCHAR temp_ptr = nullptr;

			if (!process_id || !dll_shellcode || dll_size == 0)
			{
				return STATUS_INVALID_PARAMETER;
			}





			status = utils::internal_functions::pfn_ps_lookup_process_by_process_id (process_id, &target_process);

			if (!NT_SUCCESS(status))
			{
				return status;
			}

			if (utils::internal_functions::pfn_ps_get_process_exit_status(target_process) != STATUS_PENDING)
			{
				utils::internal_functions::pfn_ob_dereference_object(target_process);
				return STATUS_UNSUCCESSFUL;  
			}

			PETHREAD main_thread = utils::thread_utils::nt_get_process_main_thread(target_process);
			if (!main_thread)
			{
				return STATUS_UNSUCCESSFUL;  
			}
			utils::internal_functions::pfn_ob_dereference_object(target_process);
			PUCHAR kShellcode = reinterpret_cast<PUCHAR>(utils::memory::allocate_independent_pages(dll_size, PAGE_EXECUTE_READWRITE));
		    if (!kShellcode)
			{
				return STATUS_UNSUCCESSFUL;
		    }

			memcpy(kShellcode, dll_shellcode, dll_size);

			KAPC_STATE kApcState = { 0 };

			utils::internal_functions::pfn_ke_stack_attach_process (target_process, &kApcState);

			SIZE_T size = sizeof(MemLoadShellcode_x64) + PAGE_SIZE;
			
			status = utils::memory::allocate_user_hidden_exec_memory(target_process, (PVOID*)&base_addr, size, true, false);
			if (!NT_SUCCESS(status))
			{
				return status;
			}

			PUCHAR ms = base_addr + PAGE_SIZE;

			memcpy(ms, MemLoadShellcode_x64, sizeof(MemLoadShellcode_x64));

			status = utils::memory::allocate_user_hidden_exec_memory(target_process, (PVOID*)&dll_addr, dll_size, true, false);
			if (!NT_SUCCESS(status))
			{
				return status;
			}
			memcpy(dll_addr, kShellcode, dll_size);
			PIMAGE_DOS_HEADER pDos = (PIMAGE_DOS_HEADER)dll_addr;

			PIMAGE_NT_HEADERS pNts = (PIMAGE_NT_HEADERS)(dll_addr + pDos->e_lfanew);

			SIZE_T ImageSize = pNts->OptionalHeader.SizeOfImage;

			status = utils::memory::allocate_user_hidden_exec_memory(target_process, (PVOID*)&image_addr, ImageSize, true, false);
			if (!NT_SUCCESS(status))
			{
				return status;
			}
			ms[0x50f] = 0x90;
			ms[0x510] = 0x48;
			ms[0x511] = 0xB8;
			*(PULONG64)&ms[0x512] = (ULONG64)image_addr;


			while (true)
			{

				status = utils::internal_functions::pfn_ps_suspend_thread (main_thread, NULL);
				if (NT_SUCCESS(status))
				{
					previous_mode_ptr = (PUCHAR)((ULONG64)main_thread + utils::scanner_offset:: find_thread_previous_mode_offset());
					//KeGetPreviousMode();
					if (*previous_mode_ptr == UserMode)
					{
						init_kernel = *(PULONG64)((PUCHAR)main_thread + 0x28);
						k_trap = (PKTRAP_FRAME)(init_kernel - sizeof(KTRAP_FRAME));
						old_rip = k_trap->Rip;
						temp_ptr = (PUCHAR)old_rip;
						temp_ptr -= 2;
						if (temp_ptr[0] == 0xF && temp_ptr[1] == 0x5 && temp_ptr[2] == 0xc3)
						{
							goto injetThread;
						}
					}
					


				}
				utils::internal_functions::pfn_ps_resume_thread(main_thread, NULL);

			}
		injetThread:

		 
			char bufcode[] =
			{
				0x50, //push  rax
				0x51, //push  rcx   
				0x52, //push  rdx
				0x53, //push  rbx												//
				0x55, 															//
				0x56, 															//
				0x57, 															//
				0x41, 0x50, 													//
				0x41, 0x51, 													//
				0x41, 0x52, 													//
				0x41, 0x53, 													//
				0x41, 0x54, 													//
				0x41, 0x55, 													//
				0x41, 0x56, 													//
				0x41, 0x57, 													//
				0x48, 0xB9,	0x00,0x00,  0x73, 0x3B, 0x0D, 0x02, 0x00,0X00,		//
				0x48, 0xB8, 0x99, 0x89, 0x67, 0x45, 0x23, 0x01, 0x00,0x00, 		//
				0x48, 0x81, 0xEC, 0xA0, 0x00, 0x00, 0x00, 						//
				0xFF, 0xD0, 													//
				0x48, 0x81, 0xC4, 0xA0, 0x00, 0x00, 0x00, 						//
				0x41, 0x5F, 													//
				0x41, 0x5E,														//
				0x41, 0x5D, 													//
				0x41, 0x5C, 													//
				0x41, 0x5B, 													//
				0x41, 0x5A, 													//
				0x41, 0x59, 													//
				0x41, 0x58, 													//
				0x5F, 															//
				0x5E, 															//
				0x5D, 															//
				0x5B, 															//
				0x5A,															//
				0x59, 															//
				0x48, 0xB8, 0x89, 0x67, 0x45, 0x23, 0x01, 0x00, 0x00, 0x00, 	//
				0x48, 0xC7, 0x00, 0x01, 0x00, 0x00, 0x00, 0x58, 				//
				0xFF, 0x25, 0x00, 0x00, 0x00, 0x00,								//
				0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00					//
			};
			 

			*(PULONG64)&bufcode[25] = (ULONG64)dll_addr;
			*(PULONG64)&bufcode[35] = (ULONG64)ms;
			*(PULONG64)&bufcode[83] = (ULONG64)base_addr + 0x500;
			*(PULONG64)&bufcode[105] = k_trap->Rip;

			memcpy(base_addr, bufcode, sizeof(bufcode));

			k_trap->Rip = (ULONG64)base_addr;

			p_freeitem_info item = reinterpret_cast<p_freeitem_info>(utils::memory::allocate_independent_pages(PAGE_SIZE, PAGE_EXECUTE_READWRITE));

			item->is_execute_addr = (ULONG64)base_addr + 0x500;
			item->pid = process_id;
			item->base_addr_free_size = size;
			item->dll_base = (ULONG64)dll_addr;
			item->dll_base_size = dll_size;
			item->dll_images = (ULONG64)image_addr;


			ExInitializeWorkItem(&item->work_item, ex_free_memory_work_item, item);

			ExQueueWorkItem(&item->work_item, DelayedWorkQueue);

			utils::internal_functions::pfn_ps_resume_thread(main_thread, NULL);

			utils::internal_functions::pfn_ke_unstack_detach_process (&kApcState);

			ObDereferenceObject(main_thread);

			utils::memory::free_independent_pages(kShellcode, dll_size);

			return status;
		}

		VOID ex_free_memory_work_item(_In_ PVOID Parameter)
		{
			 
			p_freeitem_info item = (p_freeitem_info)Parameter;

			PEPROCESS Process = NULL;

			NTSTATUS st = utils::internal_functions::pfn_ps_lookup_process_by_process_id (item->pid, &Process);

			if (!NT_SUCCESS(st))
			{
				return;
			}

			if (utils::internal_functions::pfn_ps_get_process_exit_status(Process) != STATUS_PENDING)
			{
				utils::internal_functions::pfn_ob_dereference_object(Process);
				return;
			}
			 

			ULONG64 exeValue = 0;
			SIZE_T pro = 0;
			BOOLEAN isSuccess = FALSE;

			int count = 0;
			
			while (1)
			{
				if (count > 10000) break;
				NTSTATUS status = utils::internal_functions::pfn_mm_copy_virtual_memory   (Process, (PVOID)item->is_execute_addr, IoGetCurrentProcess(), &exeValue, 1, KernelMode, &pro);
				if (NT_SUCCESS(status) && exeValue == 1)
				{
					isSuccess = TRUE;
					break;
				}
				else
				{
					status = utils::internal_functions::pfn_mm_copy_virtual_memory(Process, (PVOID)item->is_execute_addr, IoGetCurrentProcess(), &exeValue, 8, KernelMode, &pro);
					if (NT_SUCCESS(status) && exeValue == 1)
					{
						isSuccess = TRUE;
						break;
					}

				}
				 utils::thread_utils::sleep_ms (10 );
				count++;

			}

		 

			KAPC_STATE kApcState = { 0 };
		 
			utils::internal_functions::pfn_ke_stack_attach_process(Process, &kApcState);

			if (isSuccess)
			{
				ULONG64 BaseAddr = (item->is_execute_addr - 0x500);
				utils::memory::free_user_memory (item->pid, (PVOID)BaseAddr, item->base_addr_free_size);
				utils::memory::free_user_memory(item->pid, (PVOID)item->dll_base, item->dll_base_size);
				utils::internal_functions::pfn_mm_copy_virtual_memory(Process, (PVOID)item->dll_images, IoGetCurrentProcess(), (PVOID)item->dll_images, 1, KernelMode, &pro);
				//memset((PVOID)item->dll_base, 0, sizeof(IMAGE_DOS_HEADER)); 
			}

			utils::internal_functions::pfn_ke_unstack_detach_process(&kApcState);
			utils::internal_functions::pfn_ob_dereference_object(Process);
			ExFreePool(item);

		
		}

		NTSTATUS remote_thread_inject_x64_dll(HANDLE process_id, PVOID dll_shellcode, ULONG dll_size)
		{
			 
			PEPROCESS process = NULL;
			NTSTATUS status = utils::internal_functions::pfn_ps_lookup_process_by_process_id (process_id, &process);
			KAPC_STATE kApcState = { 0 };

			if (!NT_SUCCESS(status)) {
				return status;
			}

			if (!dll_shellcode)
			{
				return STATUS_UNSUCCESSFUL;
			}

			if (!dll_size)
			{
				return STATUS_UNSUCCESSFUL;
			}


			 
			if (utils::internal_functions::pfn_ps_get_process_exit_status(process) != STATUS_PENDING) {
				utils::internal_functions::pfn_ob_dereference_object (process);
				return STATUS_UNSUCCESSFUL;
			}

			
			PUCHAR kernel_dll_copy = reinterpret_cast<PUCHAR>(
				utils::memory::allocate_independent_pages(dll_size, PAGE_READWRITE)
				);
			if (!kernel_dll_copy) {
				utils::internal_functions::pfn_ob_dereference_object(process);
				return STATUS_INSUFFICIENT_RESOURCES;
			}
			 
			memcpy(kernel_dll_copy, dll_shellcode, dll_size);

		 
			BOOLEAN allocated_user_dll = FALSE;
			BOOLEAN allocated_shellcode = FALSE;
			BOOLEAN allocated_image_memory = FALSE;

			PUCHAR user_dll = NULL;
			PUCHAR user_shellcode = NULL;
			SIZE_T user_shellcode_size = sizeof(MemLoadShellcode_x64);
			PUCHAR user_image = NULL;
			SIZE_T user_image_size = 0;

			utils::internal_functions::pfn_ke_stack_attach_process(process, &kApcState);

			do {
				 
				status = utils::memory::allocate_user_hidden_exec_memory(
					process,
					(PVOID*)&user_dll,
					dll_size,
					true,
					true
				);
				if (!NT_SUCCESS(status)) break;

				memcpy(user_dll, kernel_dll_copy, dll_size);
				allocated_user_dll = TRUE;

				 
				status = utils::memory::allocate_user_hidden_exec_memory(
					process,
					(PVOID*)&user_shellcode,
					sizeof(MemLoadShellcode_x64),
					true,
					true
				);
				if (!NT_SUCCESS(status)) break;

				memcpy(user_shellcode, MemLoadShellcode_x64, user_shellcode_size);
				allocated_shellcode = TRUE;

				 
				PIMAGE_DOS_HEADER dos_header = (PIMAGE_DOS_HEADER)user_dll;
				PIMAGE_NT_HEADERS nt_headers = (PIMAGE_NT_HEADERS)(user_dll + dos_header->e_lfanew);
				user_image_size = nt_headers->OptionalHeader.SizeOfImage;

				 
				status = utils::memory::allocate_user_hidden_exec_memory(
					process,
					(PVOID*)&user_image,
					user_image_size,
					true,
					true
				);
				if (!NT_SUCCESS(status)) break;

				allocated_image_memory = TRUE;

				 
				user_shellcode[0x50F] = 0x90;
				user_shellcode[0x510] = 0x48;
				user_shellcode[0x511] = 0xB8;
				*(PULONG64)&user_shellcode[0x512] = (ULONG64)user_image;

				 
				PETHREAD thread = NULL;
				status = utils::thread_utils::my_create_thread(
					process,
					user_shellcode,
					user_dll,
					&thread
				);

				if (NT_SUCCESS(status)) {

					 
					uint64_t exe_address = reinterpret_cast<uint64_t>(
						utils::internal_functions::pfn_ps_get_process_section_base_address(process)
						);

					 
					ULONG seed = static_cast<ULONG>(__rdtsc());    
					ULONG random_offset = RtlRandomEx(&seed) % 10001;

					*reinterpret_cast<ULONG64*>(
						reinterpret_cast<PUCHAR>(thread) + utils::feature_offset::g_start_address_offset
						) = exe_address + random_offset;

					*reinterpret_cast<ULONG64*>(
						reinterpret_cast<PUCHAR>(thread) + utils::feature_offset::g_win32_start_address_offset
						) = exe_address + random_offset;

					 
					KeWaitForSingleObject(thread, Executive, KernelMode, FALSE, NULL);
					utils::internal_functions::pfn_ob_dereference_object(thread);
				}
				else {
					 
					allocated_image_memory = TRUE;
				}

			} while (0);


			
			 
			if (allocated_user_dll) {
				utils::memory::free_user_memory(process_id, user_dll, dll_size);
			}

			if (allocated_shellcode) {
				utils::memory::free_user_memory(process_id, user_shellcode, user_shellcode_size);
			}
		
		 
			if (allocated_image_memory)
			{
				PIMAGE_DOS_HEADER dos_header = reinterpret_cast<PIMAGE_DOS_HEADER>(user_image);
				PIMAGE_NT_HEADERS nt_headers = reinterpret_cast<PIMAGE_NT_HEADERS>(user_image + dos_header->e_lfanew);

				PIMAGE_SECTION_HEADER section = IMAGE_FIRST_SECTION(nt_headers);
				for (UINT i = 0; i < nt_headers->FileHeader.NumberOfSections; ++i, ++section)
				{
					PUCHAR section_base = user_image + section->VirtualAddress;
					SIZE_T section_size = section->Misc.VirtualSize;
					ULONG protect = PAGE_NOACCESS;

					if (memcmp(section->Name, ".text", 5) == 0)
						protect = PAGE_EXECUTE_READ;
					else if (memcmp(section->Name, ".rdata", 6) == 0)
						protect = PAGE_READONLY;
					else if (memcmp(section->Name, ".data", 5) == 0 || memcmp(section->Name, ".pdata", 6) == 0)
						protect = PAGE_READWRITE;
					else if (memcmp(section->Name, ".rsrc", 5) == 0)
						protect = PAGE_READONLY;
					else if (memcmp(section->Name, ".reloc", 6) == 0)
						protect = PAGE_READONLY;
					else
						protect = PAGE_READONLY;  

					auto previous_mode = utils::thread_utils::ke_set_previous_mode(KernelMode);
					ULONG old_protect = 0;

					utils::internal_functions::pfn_nt_protect_virtual_memory(
						NtCurrentProcess(),
						reinterpret_cast<PVOID*>(&section_base),
						&section_size,
						protect,
						&old_protect
					);

					utils::thread_utils::ke_set_previous_mode(previous_mode);
				}

				RtlZeroMemory(user_image,sizeof(IMAGE_DOS_HEADER));
			}
		 
		  

			utils::internal_functions::pfn_ke_unstack_detach_process(&kApcState);
			utils::internal_functions::pfn_ob_dereference_object(process);
			utils::memory::free_independent_pages(kernel_dll_copy, dll_size);

			return status;




		}
	}
}