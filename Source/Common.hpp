#pragma once

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#ifndef NOMINMAX
#define NOMINMAX
#endif

#include <Windows.h>

#include <array>
#include <atomic>
#include <chrono>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <filesystem>
#include <fstream>
#include <initializer_list>
#include <memory>
#include <mutex>
#include <optional>
#include <string>
#include <string_view>
#include <unordered_map>
#include <utility>
#include <vector>

namespace ERD {

namespace fs = std::filesystem;
using namespace std::chrono_literals;

// Elden Mod Loader 会把当前 DLL 直接加载进游戏进程。
// 这里保存插件全局生命周期里都会用到的几个共享状态。
inline HMODULE g_Module{};
inline std::atomic_bool g_Running = true;
inline std::atomic_bool g_KillSwitch = false;

// 主功能循环的轮询间隔。
// 当前功能大多是内存写值或一次性补丁，100ms 足够且不会太激进。
constexpr DWORD kFeaturePollIntervalMs = 20;

}  // namespace ERD
