#pragma once

#include "../Common.hpp"

namespace ERD::Game {

// FD4 单例注册表。
// Cheat Table 里大量脚本依赖注册好的 WorldChrMan、CSRegulationManager、
// CSFD4VirtualMemoryFlag 等符号；这里对应 DLL 版自己的解析层。
class SingletonRegistry {
public:
    bool Initialize();
    uintptr_t GetStorage(std::string_view name) const;
    uintptr_t GetObjectPointer(std::string_view name) const;

private:
    // 保存“单例名 -> 单例静态存储地址”的映射。
    std::unordered_map<std::string, uintptr_t> storage_by_name_;
};

}  // namespace ERD::Game
