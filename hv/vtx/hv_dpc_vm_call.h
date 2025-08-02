#pragma once
namespace hvgt
{
	void broadcast_vmoff(KDPC*, PVOID, PVOID SystemArgument1, PVOID SystemArgument2);

	void broadcast_hook_function(KDPC*, PVOID DeferredContext, PVOID SystemArgument1, PVOID SystemArgument2);

	void broadcast_unhook_function(KDPC*, PVOID DeferredContext, PVOID SystemArgument1, PVOID SystemArgument2);

	void broadcast_break_point_int3(KDPC*, PVOID DeferredContext, PVOID SystemArgument1, PVOID SystemArgument2);

	void broadcast_remove_break_point_int3(KDPC*, PVOID DeferredContext, PVOID SystemArgument1, PVOID SystemArgument2);
	 
	void broadcast_test_vmcall(KDPC*, PVOID DeferredContext, PVOID SystemArgument1, PVOID SystemArgument2);

	void broadcast_user_exception_int3(KDPC*, PVOID DeferredContext, PVOID SystemArgument1, PVOID SystemArgument2);
	
	void broadcast_remove_user_exception_int3(KDPC*, PVOID DeferredContext, PVOID SystemArgument1, PVOID SystemArgument2);
	 
   
	
}