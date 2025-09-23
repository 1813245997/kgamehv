#pragma once
extern "C"
{
	unsigned short __read_ldtr(void);
	unsigned short __read_tr(void);
	unsigned short __read_cs(void);
	unsigned short __read_ss(void);
	unsigned short __read_ds(void);
	unsigned short __read_es(void);
	unsigned short __read_fs(void);
	unsigned short __read_gs(void);
	void __sgdt(void*);
	void __sidt(void*);
	unsigned __int32 __load_ar(unsigned __int16);
	 
	 
	void __reload_gdtr(unsigned __int64 base, unsigned long limit);
	void __reload_idtr(unsigned __int64 base, unsigned long limit);
	void __invept(unsigned __int32 type, void* descriptors);
	void __invvpid(unsigned __int32 type, void* descriptors);
	void __writecr2(unsigned __int64 cr2);
	int __cdecl _rdseed16_step(unsigned __int16* return_value);
	int __cdecl _rdseed32_step(unsigned __int32* return_value);
	int __cdecl _rdseed64_step(unsigned __int64* return_value);

 
}

