#pragma once
namespace utils
{
	namespace signature_scanner
	{
		bool pattern_check(const char* data, const char* pattern, const char* mask);

		unsigned long long find_pattern(unsigned long long addr, size_t size, const char* pattern, const char* mask);

		unsigned long long find_pattern_image(unsigned long long addr, const char* pattern, const char* mask);

		unsigned long long find_pattern_image(unsigned long long addr, const char* pattern, const char* mask, const char* section_name);

		unsigned long long img_get_image_section(unsigned long long image_base, const char* section_name, PULONG size_of_section);

		unsigned long long resolve_relative_address(_In_ PVOID instruction, _In_ ULONG offset_offset, _In_ ULONG instruction_size);

		unsigned long long find_module_export_by_name(const unsigned long long image_base, const char* export_name);


	}
}