#pragma once
namespace hv
{
	void for_each_logical_processor(void (*callback_fn)(void*), void* context  );
}