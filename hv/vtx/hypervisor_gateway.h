#pragma once
namespace hvgt
{
	/// <summary>
	/// Turn off virtual machine
	/// </summary>
	void vmoff();

	/// <summary>
	/// Set/Unset presence of hypervisor
	/// </summary>
	/// <param name="value"> If false, hypervisor is not visible via cpuid interface, If true, it become visible</param>
	void hypervisor_visible(bool value);

	/// <summary>
	/// Unhook all pages and invalidate tlb
	/// </summary>
	/// <returns> status </returns>
	bool unhook_all_functions();

	/// <summary>
	/// Unhook single page and invalidate tlb
	/// </summary>
	/// <param name="page_physcial_address"></param>
	/// <returns> status </returns>
	bool unhook_function(void* function_address);

	/// <summary>
	/// Unhook functions by process PID and invalidate TLB
	/// </summary>
	/// <param name="process_id">The PID of the process whose hooks should be removed</param>
	/// <returns>status</returns>
	bool unhook_by_pid(HANDLE process_id);

	/// <summary>
	/// Hook function via ept and invalidates mappings
	/// </summary>
	/// <param name="target_address">Address of function which we want to hook</param>
	/// <param name="hook_function">Address of function which is used to call original function</param>
	/// <param name="origin_function">Address of function which is used to call original function</param>
	/// <returns> status </returns>
	bool hook_kernel_function(_In_ void* target_address, _In_ void* hook_function,_Inout_ void** origin_function);

	/**
	 * @brief Installs a user-mode function hook.
	 *
	 * This function sets up a hook in user mode by redirecting calls from the target function to
	 * the specified hook function. It also provides a way to store the address of the original function,
	 * enabling the possibility of calling it from the hook function.
	 *
	 * @param target_address The address of the function to hook. This is the function whose execution
	 *                       will be redirected to the hook function.
	 * @param hook_function The address of the function to be used as the hook. This function will
	 *                      replace the original target function and will be called instead of it.
	 * @param origin_function A pointer to a location where the address of the original function will
	 *                        be stored. This allows for calling the original function from within
	 *                        the hook function if needed.
	 * @return bool Returns `true` if the hook was successfully installed, or `false` if the installation
	 *               failed. This typically indicates issues with setting up the hook or modifying the
	 *               function's memory.
	 */
	bool InstallUserHook_function(void* target_address, void* hook_function, void** origin_function );
	/// <summary>
	/// Check if we can communicate with hypervisor
	/// </summary>
	/// <returns> status </returns>
	bool test_vmcall();

	/// <summary>
	/// Send irp with information to allocate memory
	/// </summary>
	/// <returns> status </returns>
	bool send_irp_perform_allocation();

	/// <summary>
	/// Dump info about allocated pools (Use Dbgview to see information)
	/// </summary>
	void dump_pool_manager();
}
