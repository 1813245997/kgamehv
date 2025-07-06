#pragma once
namespace utils
{
	namespace thread_utils
	{
		unsigned long long get_user_stack_ptr();

		void sleep(ULONG seconds);

		void sleep_ms(ULONG milliseconds);
	}
}