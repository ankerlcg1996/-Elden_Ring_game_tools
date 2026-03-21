#pragma once

#include "../Common.hpp"

namespace ERD::Main {

// 统一管理 DLL 相对路径，让配置、资源、日志都跟随插件目录走。
class ModPaths {
public:
    bool Initialize(HMODULE module_handle);

    const fs::path& DllPath() const;
    const fs::path& DllDirectory() const;

    fs::path ConfigDirectory() const;
    fs::path ResourcesDirectory() const;
    fs::path LogsDirectory() const;
    fs::path PreferredConfigPath() const;
    fs::path FindExistingConfigPath() const;
    fs::path LogFilePath() const;

private:
    fs::path dll_path_;
    fs::path dll_directory_;
};

}  // namespace ERD::Main
