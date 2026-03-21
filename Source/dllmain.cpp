#include "ERDMod.hpp"

BOOL APIENTRY DllMain(HMODULE module_handle, DWORD reason, LPVOID reserved) {
    (void)reserved;

    if (!ERD::g_Module) {
        ERD::g_Module = module_handle;
    }

    switch (reason) {
    case DLL_PROCESS_ATTACH:
        // 不关心线程级 attach/detach，关闭后能少进很多无意义回调。
        DisableThreadLibraryCalls(module_handle);

        // DllMain 内不做重活，只拉起工作线程，避免 loader lock 风险。
        CreateThread(
            nullptr,
            0,
            reinterpret_cast<LPTHREAD_START_ROUTINE>(&ERD::MainThread),
            module_handle,
            0,
            nullptr
        );
        break;

    case DLL_PROCESS_DETACH:
        // 通知主循环尽快停下，避免 DLL 卸载后后台逻辑还在跑。
        ERD::g_KillSwitch = true;
        ERD::g_Running = false;
        break;

    default:
        break;
    }

    return TRUE;
}
