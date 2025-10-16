# kunordered_map 使用说明

## 基本用法

### 1. 声明和初始化
```cpp
#include "kunordered_map.h"

// 基本类型
utils::kunordered_map<int, int> int_map;
utils::kunordered_map<const char*, int> string_map;

// 自定义类型
utils::kunordered_map<ULONG, PEPROCESS> process_map;
```

### 2. 插入数据
```cpp
// 使用 operator[] 插入
int_map[1] = 100;
int_map[2] = 200;

// 使用 insert 方法
auto result = int_map.insert(3, 300);
if (result.second) {
    // 插入成功
}

// 插入键值对
utils::kpair<int, int> pair(4, 400);
int_map.insert(pair);
```

### 3. 访问数据
```cpp
// 使用 operator[] 访问（如果不存在会创建）
int value = int_map[1];  // value = 100

// 使用 at 方法（安全访问）
try {
    int value = int_map.at(1);  // 如果不存在会返回默认值
} catch (...) {
    // 内核环境下不会抛出异常
}
```

### 4. 查找数据
```cpp
// 查找元素
auto it = int_map.find(1);
if (it != int_map.end()) {
    int key = it->first;    // 键
    int val = it->second;   // 值
}
```

### 5. 删除数据
```cpp
// 删除指定键的元素
size_t erased = int_map.erase(1);  // 返回删除的元素数量
```

### 6. 遍历数据
```cpp
// 使用范围for循环
for (auto& pair : int_map) {
    int key = pair.first;
    int val = pair.second;
    // 处理每个键值对
}

// 使用迭代器
for (auto it = int_map.begin(); it != int_map.end(); ++it) {
    int key = it->first;
    int val = it->second;
}
```

### 7. 容器信息
```cpp
// 检查是否为空
bool empty = int_map.empty();

// 获取元素数量
size_t count = int_map.size();

// 获取桶数量
size_t buckets = int_map.bucket_count();
```

### 8. 清理
```cpp
// 清空所有元素
int_map.clear();
```

## 高级用法

### 1. 自定义哈希函数
```cpp
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
```

### 2. 在驱动中的实际应用
```cpp
// 进程管理
utils::kunordered_map<ULONG, PEPROCESS> process_map;

// 模块管理
struct ModuleInfo {
    ULONG64 base_address;
    ULONG size;
    char name[256];
    bool is_hidden;
};
utils::kunordered_map<ULONG64, ModuleInfo> module_map;

// 钩子管理
struct HookInfo {
    PVOID original_address;
    PVOID hook_address;
    bool is_active;
};
utils::kunordered_map<PVOID, HookInfo> hook_map;
```

### 3. 性能优化建议
- 预分配足够的桶数量以减少重新哈希
- 使用合适的键类型（整数比字符串更高效）
- 避免频繁的插入和删除操作
- 在驱动卸载时记得清理所有容器

## 注意事项

1. **内存管理**: 容器会自动管理内存，使用内核内存分配函数
2. **异常安全**: 内核环境下不会抛出异常，失败时返回默认值
3. **线程安全**: 需要外部同步机制保证线程安全
4. **性能**: 平均时间复杂度 O(1)，最坏情况 O(n)
5. **内存开销**: 每个元素需要额外的链表节点开销

## 错误处理

```cpp
// 检查插入是否成功
auto result = int_map.insert(key, value);
if (!result.second) {
    // 插入失败，可能是内存不足
}

// 检查查找结果
auto it = int_map.find(key);
if (it == int_map.end()) {
    // 未找到元素
}
```
