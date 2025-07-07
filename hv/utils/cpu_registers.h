#pragma once
extern "C"
{

	unsigned long long asm_read_rflags();
	unsigned long long asm_read_cr8();
	void asm_write_cr8(unsigned long long vlaue);
	void asm_cli();
	void asm_sti();

}

inline bool is_ac_flag_set(void) {
	return (asm_read_rflags() & (1UL << 18)) != 0;
}