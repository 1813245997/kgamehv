#pragma once

namespace hv
{
	void ForEachLogicalProcessor(void (*callback_fn)(void*), void* context);
}

