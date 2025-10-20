#include "stdafx.h"

 
//DWORD get_process_pid_by_name(const std::wstring& process_name)
//{
//	if (process_name.empty())
//		return 0;
//
//	DWORD pid = 0;
//	HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
//	if (hSnap == INVALID_HANDLE_VALUE)
//		return 0;
//
//	PROCESSENTRY32 pe{};
//	pe.dwSize = sizeof(pe);
//
//	if (Process32First(hSnap, &pe))
//	{
//		do
//		{
//			if (_wcsicmp(pe.szExeFile, process_name.c_str()) == 0)
//			{
//				pid = pe.th32ProcessID;
//				break;
//			}
//		} while (Process32Next(hSnap, &pe));
//	}
//
//	CloseHandle(hSnap);
//	return pid;
//}

 
//int main()
//{
//	// 测试读取和写入虚拟内存的操作
//
//// 假设我们在当前进程内做内存操作
//	DWORD process_id = GetCurrentProcessId();  // 获取当前进程 ID
//
//	// 分配一个内存区域来模拟目标进程内存
//	uint64_t test_data = 0x1234567890ABCDEF;
//	void* memory_address = &test_data; // 使用一个地址作为模拟内存
//
//	// 写入数据
//	uint64_t new_value = 0xFFFFFFFFFFFFFFFF;
//	size_t bytes_written = 0;
//	if (utils::hv_user_comm::handle_write_virt_mem(process_id, memory_address, &new_value, sizeof(new_value), &bytes_written)) {
//		std::cout << "Successfully wrote " << bytes_written << " bytes to virtual memory." << std::endl;
//	}
//	else {
//		std::cerr << "Failed to write to virtual memory." << std::endl;
//	}
//
//	// 读取数据
//	uint64_t read_value = 0;
//	size_t bytes_read = 0;
//	if (utils::hv_user_comm::handle_read_virt_mem(process_id, memory_address, &read_value, sizeof(read_value), &bytes_read)) {
//		std::cout << "Successfully read " << bytes_read << " bytes from virtual memory. Value: " << std::hex << read_value << std::dec << std::endl;
//	}
//	else {
//		std::cerr << "Failed to read from virtual memory." << std::endl;
//	}
//
//	std::cout << "Press any key to exit..." << std::endl;
//	std::cin.get();
//
//	return 0;
//	 
//}