#include "global_defs.h"
#include "signature_scanner.h"

namespace utils
{
	namespace signature_scanner
	{

		bool pattern_check(const char* data, const char* pattern, const char* mask)
		{
			size_t len = strlen(mask);

			for (size_t i = 0; i < len; i++)
			{
				if (data[i] == pattern[i] || mask[i] == '?')
					continue;
				else
					return false;
			}

			return true;
		}

		unsigned long long find_pattern(unsigned long long addr, size_t size, const char* pattern, const char* mask)
		{
			size -= (unsigned long)strlen(mask);
		
			for (unsigned long i = 0; i < size; i++)
			{
				auto current_addr = reinterpret_cast<const char*>(addr + i);
				 
				if (!pattern_check(current_addr, pattern, mask))
					continue;

				if (!internal_functions::pfn_mm_is_address_valid(const_cast<void*>(reinterpret_cast<const void*>(current_addr))))
					continue;

				return reinterpret_cast<unsigned long long>(current_addr);
			}

			return 0;
		}

		unsigned long long find_pattern_image(unsigned long long addr, const char* pattern, const char* mask)
		{
			PIMAGE_DOS_HEADER dos = (PIMAGE_DOS_HEADER)addr;
			if (dos->e_magic != IMAGE_DOS_SIGNATURE) return 0;

			PIMAGE_NT_HEADERS64 nt = (PIMAGE_NT_HEADERS64)(addr + dos->e_lfanew);
			if (nt->Signature != IMAGE_NT_SIGNATURE) return 0;

			PIMAGE_SECTION_HEADER section = IMAGE_FIRST_SECTION(nt);
			for (unsigned short i = 0; i < nt->FileHeader.NumberOfSections; i++)
			{
				PIMAGE_SECTION_HEADER p = &section[i];

				if (strstr((const char*)p->Name, ".text") || strstr((const char*)p->Name, "PAGE"))
				{
					DWORD64 res = find_pattern(addr + p->VirtualAddress, p->Misc.VirtualSize, pattern, mask);
					if (res)
					{
					  
						return res;
					}
				}
			}

			return 0;
		}

		unsigned long long find_pattern_image(unsigned long long addr, const char* pattern, const char* mask, const char* section_name)
		{
			PIMAGE_DOS_HEADER dos = (PIMAGE_DOS_HEADER)addr;
			if (dos->e_magic != IMAGE_DOS_SIGNATURE) return 0;

			PIMAGE_NT_HEADERS64 nt = (PIMAGE_NT_HEADERS64)(addr + dos->e_lfanew);
			if (nt->Signature != IMAGE_NT_SIGNATURE) return 0;

			PIMAGE_SECTION_HEADER section = IMAGE_FIRST_SECTION(nt);
			for (unsigned short i = 0; i < nt->FileHeader.NumberOfSections; i++)
			{
				PIMAGE_SECTION_HEADER p = &section[i];

				if (strstr((const char*)p->Name, section_name))
				{
					DWORD64 res = find_pattern(addr + p->VirtualAddress, p->Misc.VirtualSize, pattern, mask);
					if (res)
					{
						unsigned long long self_base = utils::hidden_modules::get_driver_base();
						unsigned long long self_end = self_base + utils::hidden_modules::get_driver_size();

						if (res >= self_base && res < self_end)
						{
							// 跳过自身，继续找
							continue;
						}

						return res;
					}
				}
			}

			return 0;
		}

		unsigned long long img_get_image_section(unsigned long long ImageBase, const char* SectionName, PULONG SizeOfSection)
		{
			PIMAGE_DOS_HEADER dos = (PIMAGE_DOS_HEADER)ImageBase;
			if (dos->e_magic != IMAGE_DOS_SIGNATURE) return 0;

			PIMAGE_NT_HEADERS64 nt = (PIMAGE_NT_HEADERS64)(ImageBase + dos->e_lfanew);
			if (nt->Signature != IMAGE_NT_SIGNATURE) return 0;

			PIMAGE_SECTION_HEADER section = IMAGE_FIRST_SECTION(nt);
			for (unsigned short i = 0; i < nt->FileHeader.NumberOfSections; i++)
			{
				PIMAGE_SECTION_HEADER p = &section[i];

				if (strstr((const char*)p->Name, SectionName))
				{
					if (SizeOfSection) *SizeOfSection = p->SizeOfRawData;
					return (unsigned long long)p + p->VirtualAddress;
				}
			}

			return 0;
		}

		unsigned long long resolve_relative_address(_In_ PVOID instruction, _In_ ULONG offset_offset, _In_ ULONG instruction_size)
		{
			ULONG_PTR Instr = (ULONG_PTR)instruction;
			LONG RipOffset = *(PLONG)(Instr + offset_offset);
			unsigned long long ResolvedAddr = (unsigned long long) (Instr + instruction_size + RipOffset);

			return ResolvedAddr;
		}

		unsigned long long find_module_export_by_name(const unsigned long long image_base, const char* export_name)
		{
			if (!image_base)
			{
				return 0;

			}

			if (reinterpret_cast<PIMAGE_DOS_HEADER>(image_base)->e_magic != IMAGE_DOS_SIGNATURE)
			{
				return 0;

			}

			const auto ntHeader = reinterpret_cast<PIMAGE_NT_HEADERS64>(image_base + reinterpret_cast<PIMAGE_DOS_HEADER>(image_base)->e_lfanew);
			const auto exportDirectory = reinterpret_cast<PIMAGE_EXPORT_DIRECTORY>(image_base + ntHeader->OptionalHeader.DataDirectory[0].VirtualAddress);
			if (!exportDirectory)
			{
				return 0;

			}


			const auto exportedFunctions = reinterpret_cast<PULONG>(image_base + exportDirectory->AddressOfFunctions);
			const auto exportedNames = reinterpret_cast<PULONG>(image_base + exportDirectory->AddressOfNames);
			const auto exportedNameOrdinals = reinterpret_cast<PSHORT>(image_base + exportDirectory->AddressOfNameOrdinals);

			for (size_t i{}; i < exportDirectory->NumberOfNames; ++i) {
				const auto functionName = reinterpret_cast<const char*>(image_base + exportedNames[i]);
			 
				if (!strcmp(export_name, functionName)) {
					auto functionPointer = image_base + exportedFunctions[exportedNameOrdinals[i]];

					return  functionPointer ;
				}
			}

			return 0;
		}

		unsigned long long find_function_begin(unsigned long long image_base, unsigned long long target_addr)
		{
			auto* base_ptr = reinterpret_cast<PUCHAR>(image_base);
			auto* dos_header = reinterpret_cast<IMAGE_DOS_HEADER*>(base_ptr);
			auto* nt_headers = reinterpret_cast<IMAGE_NT_HEADERS*>(base_ptr + dos_header->e_lfanew);

			const auto& exception_directory = nt_headers->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXCEPTION];


			if (exception_directory.VirtualAddress == 0)
			{
				return 0;
			}

			if (exception_directory.Size == 0)
			{
				return 0;
			}

			auto* function_entries = reinterpret_cast<IMAGE_RUNTIME_FUNCTION_ENTRY*>(
				base_ptr + exception_directory.VirtualAddress);

			size_t entry_count = exception_directory.Size / sizeof(IMAGE_RUNTIME_FUNCTION_ENTRY);

			for (size_t i = 0; i < entry_count; ++i)
			{
				unsigned long long begin_addr = image_base + function_entries[i].BeginAddress;
				unsigned long long end_addr = image_base + function_entries[i].EndAddress;

				if (target_addr >= begin_addr && target_addr < end_addr)
				{
					return begin_addr;
				}
			}

			return 0;
		}

	 }
}

