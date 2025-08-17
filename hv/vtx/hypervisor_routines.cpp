#include "hv.h"
#include "hypervisor_routines.h"


namespace hv
{
	unsigned __int64 vmread(unsigned __int64 vmcs_field)
	{
		unsigned __int64 value;
		__vmx_vmread(vmcs_field, &value);
		return value;
	}


	void inject_interruption(unsigned __int32 vector, unsigned __int32 type, unsigned __int32 error_code, bool deliver_error_code)
	{
		vmentry_interrupt_information interrupt = { 0 };

		interrupt.interruption_type = type;
		interrupt.vector = vector;
		interrupt.deliver_error_code = deliver_error_code;
		interrupt.valid = 1;

		if (type == software_exception || type == privileged_software_exception || type == software_interrupt)
			hv::vmwrite<unsigned __int64>(VMCS_CTRL_VMENTRY_INSTRUCTION_LENGTH, hv::vmread(VMCS_VMEXIT_INSTRUCTION_LENGTH));

		if (deliver_error_code == true)
			hv::vmwrite<unsigned __int64>(VMCS_CTRL_VMENTRY_EXCEPTION_ERROR_CODE, error_code);

		hv::vmwrite<unsigned __int64>(VMCS_CTRL_VMENTRY_INTERRUPTION_INFORMATION_FIELD, interrupt.flags);
	}
}

 