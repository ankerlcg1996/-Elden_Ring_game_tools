#include "Paths.hpp"

namespace ERD::Main {

bool ModPaths::Initialize(HMODULE module_handle) {
    wchar_t buffer[MAX_PATH]{};
    const DWORD length = GetModuleFileNameW(module_handle, buffer, MAX_PATH);
    if (length == 0) {
        return false;
    }

    dll_path_ = fs::path(std::wstring(buffer, buffer + length));
    dll_directory_ = dll_path_.parent_path();
    return !dll_directory_.empty();
}

const fs::path& ModPaths::DllPath() const {
    return dll_path_;
}

const fs::path& ModPaths::DllDirectory() const {
    return dll_directory_;
}

fs::path ModPaths::ConfigDirectory() const {
    return dll_directory_ / L"Config";
}

fs::path ModPaths::ResourcesDirectory() const {
    return dll_directory_ / L"Resources";
}

fs::path ModPaths::LogsDirectory() const {
    return dll_directory_ / L"Logs";
}

fs::path ModPaths::PreferredConfigPath() const {
    return ConfigDirectory() / L"ERDMod.ini";
}

// 兼容几种老路径，避免你改目录结构后插件直接找不到配置。
fs::path ModPaths::FindExistingConfigPath() const {
    const std::array<fs::path, 3> candidates{
        PreferredConfigPath(),
        dll_directory_ / L"ERDMod.ini",
        dll_directory_ / L"erdmod.ini",
    };

    for (const fs::path& candidate : candidates) {
        if (fs::exists(candidate)) {
            return candidate;
        }
    }

    return PreferredConfigPath();
}

fs::path ModPaths::LogFilePath() const {
    return LogsDirectory() / L"ERDMod.log";
}

}  // namespace ERD::Main
