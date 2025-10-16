#include "global_defs.h"
#include "kunordered_map.h"

// 使用示例
void kunordered_map_usage_examples() {
    // 1. 基本使用 - 整数键值对
    utils::kunordered_map<int, int> int_map;
    
    // 插入数据
    int_map[1] = 100;
    int_map[2] = 200;
    int_map[3] = 300;
    
    // 访问数据
    int value = int_map[2];  // value = 200
    
    // 查找数据
    auto it = int_map.find(1);
    if (it != int_map.end()) {
        // 找到了，it->first 是键，it->second 是值
        int key = it->first;    // key = 1
        int val = it->second;   // val = 100
    }
    
    // 2. 字符串键值对
    utils::kunordered_map<const char*, int> string_map;
    
    string_map["apple"] = 10;
    string_map["banana"] = 20;
    string_map["orange"] = 30;
    
    // 查找字符串键
    auto str_it = string_map.find("banana");
    if (str_it != string_map.end()) {
        int count = str_it->second;  // count = 20
    }
    
    // 3. 自定义类型键值对
    struct ProcessInfo {
        ULONG pid;
        ULONG64 base_address;
        char name[64];
        
        ProcessInfo() : pid(0), base_address(0) {
            RtlZeroMemory(name, sizeof(name));
        }
        
        ProcessInfo(ULONG p, ULONG64 addr, const char* n) : pid(p), base_address(addr) {
            RtlZeroMemory(name, sizeof(name));
            if (n) {
                RtlCopyMemory(name, n, min(strlen(n), sizeof(name) - 1));
            }
        }
    };
    
    utils::kunordered_map<ULONG, ProcessInfo> process_map;
    
    // 插入进程信息
    ProcessInfo info1(1234, 0x400000, "notepad.exe");
    ProcessInfo info2(5678, 0x500000, "calc.exe");
    
    process_map[1234] = info1;
    process_map[5678] = info2;
    
    // 查找进程
    auto proc_it = process_map.find(1234);
    if (proc_it != process_map.end()) {
        ProcessInfo& proc = proc_it->second;
        // 使用 proc.pid, proc.base_address, proc.name
    }
    
    // 4. 遍历所有元素
    for (auto& pair : int_map) {
        int key = pair.first;
        int val = pair.second;
        // 处理每个键值对
    }
    
    // 5. 删除元素
    int_map.erase(2);  // 删除键为2的元素
    
    // 6. 检查容器状态
    bool is_empty = int_map.empty();
    size_t size = int_map.size();
    size_t buckets = int_map.bucket_count();
    
    // 7. 清空容器
    int_map.clear();
    
    // 8. 使用自定义哈希函数
    struct CustomHash {
        size_t operator()(const char* key) const {
            if (!key) return 0;
            size_t hash = 0;
            while (*key) {
                hash = hash * 31 + *key++;
            }
            return hash;
        }
    };
    
    utils::kunordered_map<const char*, int, CustomHash> custom_map;
    custom_map["test"] = 42;
}

// 在驱动中使用的高级示例
void driver_usage_example() {
    // 用于存储进程ID到进程对象的映射
    utils::kunordered_map<ULONG, PEPROCESS> process_map;
    
    // 用于存储模块基址到模块信息的映射
    struct ModuleInfo {
        ULONG64 base_address;
        ULONG size;
        char name[256];
        bool is_hidden;
    };
    
    utils::kunordered_map<ULONG64, ModuleInfo> module_map;
    
    // 用于存储钩子信息的映射
    struct HookInfo {
        PVOID original_address;
        PVOID hook_address;
        UCHAR original_bytes[16];
        bool is_active;
    };
    
    utils::kunordered_map<PVOID, HookInfo> hook_map;
    
    // 使用示例：
    // 1. 注册进程
    PEPROCESS process = nullptr;
    // ... 获取进程对象 ...
    process_map[1234] = process;
    
    // 2. 注册模块
    ModuleInfo mod_info;
    mod_info.base_address = 0x400000;
    mod_info.size = 0x1000;
    RtlCopyMemory(mod_info.name, "ntdll.dll", 10);
    mod_info.is_hidden = false;
    
    module_map[0x400000] = mod_info;
    
    // 3. 注册钩子
    HookInfo hook_info;
    hook_info.original_address = (PVOID)0x401000;
    hook_info.hook_address = (PVOID)0x500000;
    hook_info.is_active = true;
    // ... 保存原始字节 ...
    
    hook_map[(PVOID)0x401000] = hook_info;
    
    // 4. 查找和操作
    auto proc_it = process_map.find(1234);
    if (proc_it != process_map.end()) {
        PEPROCESS found_process = proc_it->second;
        // 使用找到的进程对象
    }
    
    // 5. 遍历所有钩子
    for (auto& hook_pair : hook_map) {
        PVOID addr = hook_pair.first;
        HookInfo& info = hook_pair.second;
        
        if (info.is_active) {
            // 处理活跃的钩子
        }
    }
    
    // 6. 清理（在驱动卸载时）
    process_map.clear();
    module_map.clear();
    hook_map.clear();
}

// 性能测试示例
void performance_test_example() {
    utils::kunordered_map<ULONG, ULONG> test_map;
    
    // 插入大量数据
    for (ULONG i = 0; i < 10000; ++i) {
        test_map[i] = i * 2;
    }
    
    // 查找测试
    for (ULONG i = 0; i < 1000; ++i) {
        auto it = test_map.find(i * 10);
        if (it != test_map.end()) {
            // 找到数据
        }
    }
    
    // 删除测试
    for (ULONG i = 0; i < 5000; ++i) {
        test_map.erase(i);
    }
}
