#pragma once




namespace hv
{

	unsigned __int64 vmread(unsigned __int64 vmcs_field);


	template <typename T>
	void vmwrite(unsigned __int64 vmcs_field, T value)
	{
		__vmx_vmwrite(vmcs_field, (unsigned __int64)value);
	}


	void inject_interruption(unsigned __int32 vector, unsigned __int32 type, unsigned __int32 error_code, bool deliver_error_code);
}