#pragma once

#include "Common.hpp"

namespace ERD {

// 由 DllMain 创建的工作线程入口。
// 这里会负责路径初始化、配置读取、日志启动和运行时主循环。
void MainThread();

}  // namespace ERD
