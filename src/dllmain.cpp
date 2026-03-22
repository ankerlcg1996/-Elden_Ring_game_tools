#define WIN32_LEAN_AND_MEAN

#include "Common.hpp"
#include "grace_test_config.hpp"
#include "grace_test_localization.hpp"
#include "grace_test_messages.hpp"
#include "grace_test_runtime.hpp"
#include "grace_test_talkscript.hpp"

#include <elden-x/singletons.hpp>
#include <elden-x/utils/modutils.hpp>

#include <spdlog/sinks/daily_file_sink.h>
#include <spdlog/spdlog.h>

#include <windows.h>

#include <filesystem>
#include <memory>
#include <stdexcept>
#include <thread>

namespace fs = std::filesystem;

namespace {

std::thread mod_thread;

void setup_logger(const fs::path& folder) {
    fs::create_directories(folder / "logs");
    auto logger = std::make_shared<spdlog::logger>("erdGameTools");
    logger->sinks().push_back(std::make_shared<spdlog::sinks::daily_file_sink_st>(
        (folder / "logs" / "erdGameTools.log").string(), 0, 0, false, 5));
    logger->flush_on(spdlog::level::info);
    logger->set_pattern("[%Y-%m-%d %H:%M:%S.%e] [%n] [%l] %v");
    spdlog::set_default_logger(logger);
}

void setup_mod() {
    modutils::initialize();
    er::FD4::find_singletons();

    SPDLOG_INFO("Hooking native menu messages...");
    grace_test::msg::initialize();

    SPDLOG_INFO("Hooking native menu talkscript...");
    grace_test::talkscript::initialize();

    modutils::enable_hooks();
    SPDLOG_INFO("erdGameTools initialized, starting native feature runtime.");
}

}  // namespace

bool WINAPI DllMain(HINSTANCE dll_instance, unsigned int reason, void* reserved) {
    if (reason == DLL_PROCESS_ATTACH) {
        ERD::g_Module = dll_instance;
        ERD::g_Running = true;
        wchar_t dll_filename[MAX_PATH] = {0};
        GetModuleFileNameW(dll_instance, dll_filename, MAX_PATH);
        const fs::path folder = fs::path(dll_filename).parent_path();

        try {
            setup_logger(folder);
            grace_test::config::initialize(folder);
            grace_test::localization::initialize(folder);
            SPDLOG_INFO("erdGameTools version {}", PROJECT_VERSION);
        } catch (const std::exception& error) {
            OutputDebugStringA(("erdGameTools logger setup failed: " + std::string(error.what()) + "\n").c_str());
            return FALSE;
        }

        mod_thread = std::thread([folder]() {
            try {
                setup_mod();
                grace_test::runtime::run(folder);
            } catch (const std::runtime_error& error) {
                SPDLOG_ERROR("Initialization failed: {}", error.what());
                modutils::deinitialize();
            }
        });
    } else if (reason == DLL_PROCESS_DETACH && reserved != nullptr) {
        try {
            grace_test::runtime::request_stop();
            if (mod_thread.joinable()) {
                mod_thread.join();
            }
            modutils::deinitialize();
            SPDLOG_INFO("erdGameTools deinitialized.");
        } catch (const std::runtime_error& error) {
            SPDLOG_ERROR("Deinitialization failed: {}", error.what());
            spdlog::shutdown();
            return false;
        }
        spdlog::shutdown();
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
