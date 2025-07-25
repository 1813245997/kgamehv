#pragma once
namespace utils
{
	namespace  khyper_vt
	   {
		enum class CpuVendor
		{
			unknown,  ///< Unknown or unsupported CPU vendor.
			intel,    ///< Intel CPU.
			amd      ///< AMD CPU.
		};


		NTSTATUS initialize_khyper_vt();

		NTSTATUS  initialize_cpu_vendor(CpuVendor* cpu_type);

		NTSTATUS is_cpu_virtualization_supported(CpuVendor cpu_type);

		NTSTATUS is_ept_or_rvi_supported(CpuVendor cpu_type);

		NTSTATUS is_virtualization_enabled_in_bios(CpuVendor cpu_type);

		NTSTATUS initialize_khyper_game_vm(CpuVendor cpu_type);

		NTSTATUS initialize_intel_vtx();

		NTSTATUS initialize_amd_svm();

		bool is_intel_cpu();
	   }
}