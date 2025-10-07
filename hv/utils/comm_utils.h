#pragma once

namespace utils
{
	namespace comm
	{
		NTSTATUS get_driver_object(const wchar_t* driver_name, PDRIVER_OBJECT* driver_object);
	}
}