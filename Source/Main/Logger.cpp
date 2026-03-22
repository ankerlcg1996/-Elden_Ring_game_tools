#include "Logger.hpp"

#include <iomanip>
#include <iostream>
#include <sstream>

namespace ERD::Main {
namespace {

std::string BuildPrefix(std::string_view level) {
    SYSTEMTIME local_time{};
    GetLocalTime(&local_time);

    std::ostringstream stream;
    stream << '['
           << std::setfill('0') << std::setw(2) << local_time.wHour << ':'
           << std::setfill('0') << std::setw(2) << local_time.wMinute << ':'
           << std::setfill('0') << std::setw(2) << local_time.wSecond << "] ["
           << level << "] ";
    return stream.str();
}

}  // namespace

Logger& Logger::Instance() {
    static Logger instance;
    return instance;
}

void Logger::Initialize(const fs::path& log_path, bool enable_file_log) {
    std::lock_guard<std::mutex> guard(mutex_);

    file_logging_enabled_ = enable_file_log;
    if (!file_logging_enabled_) {
        return;
    }

    std::error_code ec;
    fs::create_directories(log_path.parent_path(), ec);
    file_.open(log_path, std::ios::out | std::ios::app);
}

void Logger::Info(std::string_view message) {
    Write("INFO", message);
}

void Logger::Error(std::string_view message) {
    Write("ERROR", message);
}

void Logger::Write(std::string_view level, std::string_view message) {
    const std::string line = BuildPrefix(level) + std::string(message);

    std::lock_guard<std::mutex> guard(mutex_);

    OutputDebugStringA((line + "\n").c_str());

    if (file_logging_enabled_ && file_.is_open()) {
        file_ << line << '\n';
        file_.flush();
    }
}

}  // namespace ERD::Main
