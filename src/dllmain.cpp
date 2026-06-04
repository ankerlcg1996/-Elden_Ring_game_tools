#define WIN32_LEAN_AND_MEAN

#include "Common.hpp"
#include "grace_test_config.hpp"
#include "grace_test_localization.hpp"
#include "grace_test_messages.hpp"
#include "grace_test_plugins.hpp"
#include "grace_test_runtime.hpp"
#include "grace_test_talkscript.hpp"

#include <elden-x/singletons.hpp>
#include <elden-x/utils/modutils.hpp>

#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/spdlog.h>

#include <windows.h>

#include <filesystem>
#include <memory>
#include <stdexcept>

namespace fs = std::filesystem;

namespace {

constexpr DWORD kAltSavesCompatibilityDelayMs = 5000;

void setup_logger(const fs::path& folder) {
    const fs::path log_folder = folder / "logs";
    std::error_code ec;
    fs::create_directories(log_folder, ec);
    fs::remove(log_folder / "erdGameTools.log", ec);
    fs::remove(log_folder / "erd_game_tools.log", ec);

    auto logger = std::make_shared<spdlog::logger>(
        "erdGameTools",
        std::make_shared<spdlog::sinks::basic_file_sink_mt>((log_folder / "erdGameTools.log").string(), true));
    logger->flush_on(spdlog::level::info);
    logger->set_pattern("[%Y-%m-%d %H:%M:%S.%e] [%n] [%l] %v");
    spdlog::set_default_logger(logger);
}

void wait_for_game_startup() {
    if (ERD::g_Running) {
        Sleep(kAltSavesCompatibilityDelayMs);
    }
}

void setup_mod(const fs::path& folder) {
    modutils::initialize();
    er::FD4::find_singletons();

    SPDLOG_INFO("Hooking native menu messages...");
    grace_test::msg::initialize();

    SPDLOG_INFO("Hooking native menu talkscript...");
    grace_test::talkscript::initialize();

    modutils::enable_hooks();
    SPDLOG_INFO("erdGameTools initialized, starting native feature runtime.");
}

DWORD WINAPI run_mod(LPVOID parameter) {
    const HMODULE dll_instance = static_cast<HMODULE>(parameter);
    try {
        wchar_t dll_filename[MAX_PATH] = {0};
        GetModuleFileNameW(dll_instance, dll_filename, MAX_PATH);
        const fs::path folder = fs::path(dll_filename).parent_path();

        wait_for_game_startup();
        if (!ERD::g_Running) {
            return 0;
        }

        setup_logger(folder);
        grace_test::config::initialize(folder);
        grace_test::localization::initialize(folder);
        SPDLOG_INFO("Startup wait complete after {} ms compatibility delay.", kAltSavesCompatibilityDelayMs);
        SPDLOG_INFO("erdGameTools version {}", PROJECT_VERSION);
        setup_mod(folder);
        grace_test::runtime::run(folder);
    } catch (const std::exception& error) {
        const std::string message = "erdGameTools initialization failed:\n" + std::string(error.what());
        OutputDebugStringA((message + "\n").c_str());
        MessageBoxA(nullptr, message.c_str(), "erdGameTools", MB_OK | MB_ICONERROR | MB_SYSTEMMODAL);
        if (spdlog::default_logger() != nullptr) {
            SPDLOG_ERROR("Initialization failed: {}", error.what());
        }
        modutils::deinitialize();
        spdlog::shutdown();
        ERD::g_Running = false;
    }
    return 0;
}

}  // namespace

bool WINAPI DllMain(HINSTANCE dll_instance, unsigned int reason, void* reserved) {
    if (reason == DLL_PROCESS_ATTACH) {
        ERD::g_Module = dll_instance;
        ERD::g_Running = true;
        DisableThreadLibraryCalls(dll_instance);
        HANDLE thread = CreateThread(nullptr, 0, &run_mod, dll_instance, 0, nullptr);
        if (thread != nullptr) {
            CloseHandle(thread);
        } else {
            ERD::g_Running = false;
            return FALSE;
        }
    } else if (reason == DLL_PROCESS_DETACH && reserved != nullptr) {
        grace_test::runtime::request_stop();
    }
    return true;
}

static struct dummy_modengine_extension_st {
    virtual ~dummy_modengine_extension_st() = default;
    virtual void on_attach() {}
    virtual void on_detach() {}
    virtual const char* id() { return "erdGameTools"; }
} modengine_extension;

extern "C" __declspec(dllexport) bool modengine_ext_init(void* connector, void** extension) {
    *extension = &modengine_extension;
    return true;
}
