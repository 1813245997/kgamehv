#include "../utils/global_defs.h"
#include "timing.h"
#include "../asm/vm_intrin.h"
#include "vmcall_reason.h"

namespace hv
{
	void measure_vm_exit_overheads(__vcpu*  cpu) {
		cpu->vm_exit_tsc_overhead = measure_vm_exit_tsc_overhead();
		cpu->vm_exit_mperf_overhead = measure_vm_exit_mperf_overhead();
		cpu->vm_exit_ref_tsc_overhead = measure_vm_exit_ref_tsc_overhead();

		LogDebug("[hv] Measured VM-exit overhead (TSC = %zi).\n", cpu->vm_exit_tsc_overhead);
		LogDebug("[hv] Measured VM-exit overhead (MPERF = %zi).\n", cpu->vm_exit_mperf_overhead);
		LogDebug("[hv] Measured VM-exit overhead (CPU_CLK_UNHALTED.REF_TSC = %zi).\n",
			cpu->vm_exit_ref_tsc_overhead);
	}

 
	// measure the overhead of a vm-exit (RDTSC)
	uint64_t measure_vm_exit_tsc_overhead() {
		_disable();

		 

		uint64_t lowest_vm_exit_overhead = ~0ull;
		uint64_t lowest_timing_overhead = ~0ull;

		// perform the measurement 10 times and use the smallest time
		for (int i = 0; i < 10; ++i) {
			_mm_lfence();
			auto start = __rdtsc();
			_mm_lfence();

			_mm_lfence();
			auto end = __rdtsc();
			_mm_lfence();

			auto const timing_overhead = (end - start);

			 
			__vm_call(VMCALL_TEST, 0, 0, 0);

			_mm_lfence();
			start = __rdtsc();
			_mm_lfence();

			__vm_call(VMCALL_TEST, 0, 0, 0);

			_mm_lfence();
			end = __rdtsc();
			_mm_lfence();

			auto const vm_exit_overhead = (end - start);

			if (vm_exit_overhead < lowest_vm_exit_overhead)
				lowest_vm_exit_overhead = vm_exit_overhead;
			if (timing_overhead < lowest_timing_overhead)
				lowest_timing_overhead = timing_overhead;
		}

		_enable();
		return lowest_vm_exit_overhead - lowest_timing_overhead;
	}


	// measure the overhead of a vm-exit (CPU_CLK_UNHALTED.REF_TSC)
	uint64_t measure_vm_exit_ref_tsc_overhead() {
		_disable();

		 

		ia32_fixed_ctr_ctrl_register curr_fixed_ctr_ctrl;
		curr_fixed_ctr_ctrl.flags = __readmsr(IA32_FIXED_CTR_CTRL);

		ia32_perf_global_ctrl_register curr_perf_global_ctrl;
		curr_perf_global_ctrl.flags = __readmsr(IA32_PERF_GLOBAL_CTRL);

		// enable fixed counter #2
		auto new_fixed_ctr_ctrl = curr_fixed_ctr_ctrl;
		new_fixed_ctr_ctrl.en2_os = 1;
		new_fixed_ctr_ctrl.en2_usr = 0;
		new_fixed_ctr_ctrl.en2_pmi = 0;
		new_fixed_ctr_ctrl.any_thread2 = 0;
		__writemsr(IA32_FIXED_CTR_CTRL, new_fixed_ctr_ctrl.flags);

		// enable fixed counter #2
		auto new_perf_global_ctrl = curr_perf_global_ctrl;
		new_perf_global_ctrl.en_fixed_ctrn |= (1ull << 2);
		__writemsr(IA32_PERF_GLOBAL_CTRL, new_perf_global_ctrl.flags);

		uint64_t lowest_vm_exit_overhead = ~0ull;
		uint64_t lowest_timing_overhead = ~0ull;

		// perform the measurement 10 times and use the smallest time
		for (int i = 0; i < 10; ++i) {
			_mm_lfence();
			auto start = __readmsr(IA32_FIXED_CTR2);
			_mm_lfence();

			_mm_lfence();
			auto end = __readmsr(IA32_FIXED_CTR2);
			_mm_lfence();

			auto const timing_overhead = (end - start);

			__vm_call(VMCALL_TEST, 0, 0, 0);

			_mm_lfence();
			start = __readmsr(IA32_FIXED_CTR2);
			_mm_lfence();

			__vm_call(VMCALL_TEST, 0, 0, 0);

			_mm_lfence();
			end = __readmsr(IA32_FIXED_CTR2);
			_mm_lfence();

			auto const vm_exit_overhead = (end - start);

			if (vm_exit_overhead < lowest_vm_exit_overhead)
				lowest_vm_exit_overhead = vm_exit_overhead;
			if (timing_overhead < lowest_timing_overhead)
				lowest_timing_overhead = timing_overhead;
		}

		// restore MSRs
		__writemsr(IA32_PERF_GLOBAL_CTRL, curr_perf_global_ctrl.flags);
		__writemsr(IA32_FIXED_CTR_CTRL, curr_fixed_ctr_ctrl.flags);

		_enable();
		return lowest_vm_exit_overhead - lowest_timing_overhead;
	}

	// measure the overhead of a vm-exit (IA32_MPERF)
	uint64_t measure_vm_exit_mperf_overhead() {
		_disable();

		 

		uint64_t lowest_vm_exit_overhead = ~0ull;
		uint64_t lowest_timing_overhead = ~0ull;

		// perform the measurement 10 times and use the smallest time
		for (int i = 0; i < 10; ++i) {
			_mm_lfence();
			auto start = __readmsr(IA32_MPERF);
			_mm_lfence();

			_mm_lfence();
			auto end = __readmsr(IA32_MPERF);
			_mm_lfence();

			auto const timing_overhead = (end - start);

			__vm_call(VMCALL_TEST, 0, 0, 0);
		 

			_mm_lfence();
			start = __readmsr(IA32_MPERF);
			_mm_lfence();

			__vm_call(VMCALL_TEST, 0, 0, 0);

			_mm_lfence();
			end = __readmsr(IA32_MPERF);
			_mm_lfence();

			auto const vm_exit_overhead = (end - start);

			if (vm_exit_overhead < lowest_vm_exit_overhead)
				lowest_vm_exit_overhead = vm_exit_overhead;
			if (timing_overhead < lowest_timing_overhead)
				lowest_timing_overhead = timing_overhead;
		}

		_enable();
		return lowest_vm_exit_overhead - lowest_timing_overhead;
	}
}