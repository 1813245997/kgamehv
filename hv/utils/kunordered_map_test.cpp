// 测试 kunordered_map 的修复
void test_kunordered_map_fix() {
    utils::kunordered_map<int, int> test_map;
    
    // 测试插入
    test_map[1] = 100;
    test_map[2] = 200;
    test_map[3] = 300;
    
    // 测试访问
    int value1 = test_map[1];  // 应该是 100
    int value2 = test_map[2];  // 应该是 200
    int value3 = test_map[3];  // 应该是 300
    
    // 测试查找
    auto it1 = test_map.find(1);
    auto it2 = test_map.find(2);
    auto it3 = test_map.find(3);
    
    // 验证结果
    if (value1 == 100 && value2 == 200 && value3 == 300) {
        // 成功
        DbgPrint("kunordered_map test PASSED: values are correct\n");
    } else {
        // 失败
        DbgPrint("kunordered_map test FAILED: value1=%d, value2=%d, value3=%d\n", 
                 value1, value2, value3);
    }
    
    // 测试不存在的键
    int value4 = test_map[4];  // 应该是 0（默认值）
    DbgPrint("Non-existent key test: value4=%d (should be 0)\n", value4);
    
    // 测试容器状态
    DbgPrint("Map size: %zu, bucket count: %zu, empty: %s\n", 
             test_map.size(), 
             test_map.bucket_count(),
             test_map.empty() ? "true" : "false");
}
