#pragma once

 

// 寄存器相关
cr0 read_effective_guest_cr0();
cr4 read_effective_guest_cr4();
uint16_t current_guest_cpl();
bool vmx_get_current_execution_mode();

// VMCS 控制字段
ia32_vmx_procbased_ctls_register read_ctrl_proc_based();
void write_ctrl_proc_based(ia32_vmx_procbased_ctls_register const value);

// CPU 数据缓存
void cache_cpu_data(vcpu_cached_data& cached);

// 工具函数
unsigned __int32 ajdust_controls(unsigned __int32 ctl, unsigned __int32 msr);




