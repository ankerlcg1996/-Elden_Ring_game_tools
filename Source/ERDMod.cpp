#include "ERDMod.hpp"

#include "Main/Config.hpp"
#include "Main/D3D12Overlay.hpp"
#include "Main/Logger.hpp"
#include "Main/Paths.hpp"
#include "Main/Runtime.hpp"

namespace ERD {

void MainThread() {
    // 先准备 DX12 + ImGui 叠加层对象，真正的 hook 会在 Runtime 里安装。
    Main::g_D3D12Overlay = std::make_unique<Main::D3D12Overlay>();

    // 解析当前 DLL 所在位置，后续配置、日志、资源都按 DLL 相对路径寻找。
    Main::ModPaths paths;
    if (!paths.Initialize(g_Module)) {
        return;
    }

    // 配置和日志都尽量早初始化，方便后面任何失败都能留下痕迹。
    const Main::ModConfig config = Main::LoadConfig(paths.FindExistingConfigPath());
    Main::Logger::Instance().Initialize(paths.LogFilePath(), config.enable_file_log);
    Main::Logger::Instance().Info("DLL attached successfully.");

    // 进入插件运行时主循环。
    Main::Runtime runtime(std::move(paths), config);
    runtime.Run();

    // 退出前主动拆掉 overlay hook，避免游戏继续调用已经失效的函数指针。
    if (Main::g_D3D12Overlay) {
        Main::g_D3D12Overlay->Unhook();
        Main::g_D3D12Overlay.reset();
    }

    FreeLibraryAndExitThread(g_Module, 0);
}

}  // namespace ERD
