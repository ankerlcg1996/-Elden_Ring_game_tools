#pragma once

#include "../Common.hpp"

namespace ERD::Main {

// 轻量日志器。
// 同时支持 OutputDebugString 和可选文件日志，方便本地调试。
class Logger {
public:
    static Logger& Instance();

    void Initialize(const fs::path& log_path, bool enable_file_log);
    void Info(std::string_view message);
    void Error(std::string_view message);

private:
    Logger() = default;

    void Write(std::string_view level, std::string_view message);

    std::mutex mutex_;
    std::ofstream file_;
    bool file_logging_enabled_ = false;
};

}  // namespace ERD::Main
