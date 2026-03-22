#include "MovementMods.hpp"

#include "../Game/Memory.hpp"
#include "../Main/FeatureStatus.hpp"
#include "../Main/Logger.hpp"

#include <cmath>

namespace ERD::Features {
namespace {

struct Region {
    uintptr_t base = 0;
    std::size_t size = 0;
};

struct Pattern {
    std::vector<std::uint8_t> bytes;
    std::vector<std::uint8_t> masks;
};

constexpr std::size_t kInlinePatchSize = 5;
constexpr uintptr_t kNetPlayersOffset = 0x10EF8;
constexpr const char* kFollowCamPattern = "F3 0F 10 93 60 01 00 00 F3 0F 10 9B";
constexpr const char* kFreecamPositionPattern = "0F 29 37 80 BB 15 03 00 00 00";
constexpr const char* kFreecamRotationPattern = "0F 29 A6 50 01 00 00 41 0F 28";
constexpr const char* kNoFallCameraPattern =
    "0F 87 ? ? ? ? 41 8B 8C 85 ? ? ? ? 49 03 CD FF E1 48 8B 4F 60 4D 8B CE 4C 8B C6 41 0F 28 C9 E8";

std::atomic<uintptr_t> g_FollowCamAnglesPtr = 0;
std::atomic<uintptr_t> g_FreecamTransformPtr = 0;

void LogProtectedException(const char* scope, const char* detail) {
    Main::Logger::Instance().Error((std::string(scope) + " failed: " + detail).c_str());
}

#define ERD_PROTECTED_STEP(SCOPE, CALL)                           \
    do {                                                          \
        try {                                                     \
            CALL;                                                 \
        } catch (const std::exception& ex) {                      \
            LogProtectedException(SCOPE, ex.what());              \
        } catch (...) {                                           \
            LogProtectedException(SCOPE, "unknown exception");     \
        }                                                         \
    } while (false)

bool RegionFromMainModuleText(Region& region) {
    HMODULE module = GetModuleHandleW(nullptr);
    if (module == nullptr) {
        return false;
    }

    const auto* dos = reinterpret_cast<const IMAGE_DOS_HEADER*>(module);
    const auto* nt = reinterpret_cast<const IMAGE_NT_HEADERS*>(reinterpret_cast<uintptr_t>(module) + dos->e_lfanew);
    const IMAGE_SECTION_HEADER* sections = IMAGE_FIRST_SECTION(nt);
    for (unsigned i = 0; i < nt->FileHeader.NumberOfSections; ++i) {
        const IMAGE_SECTION_HEADER* section = sections + i;
        if (_stricmp(reinterpret_cast<const char*>(section->Name), ".text") == 0) {
            region.base = reinterpret_cast<uintptr_t>(module) + section->VirtualAddress;
            region.size = section->Misc.VirtualSize;
            return true;
        }
    }

    return false;
}

int HexValue(char c) {
    if (c >= '0' && c <= '9') {
        return c - '0';
    }
    if (c >= 'a' && c <= 'f') {
        return 10 + (c - 'a');
    }
    if (c >= 'A' && c <= 'F') {
        return 10 + (c - 'A');
    }
    return -1;
}

bool ParsePattern(const char* aob, Pattern& pattern) {
    pattern.bytes.clear();
    pattern.masks.clear();

    std::string token;
    for (const char* cursor = aob;; ++cursor) {
        const char ch = *cursor;
        if (ch == ' ' || ch == '\0') {
            if (!token.empty()) {
                std::uint8_t byte = 0;
                std::uint8_t mask = 0;

                if (token.size() == 1) {
                    const int hi = HexValue(token[0]);
                    if (hi >= 0) {
                        byte = static_cast<std::uint8_t>(hi);
                        mask = 0x0F;
                    } else if (token[0] != '?') {
                        return false;
                    }
                } else if (token.size() == 2) {
                    const int hi = HexValue(token[0]);
                    const int lo = HexValue(token[1]);
                    if (hi >= 0) {
                        byte |= static_cast<std::uint8_t>(hi << 4);
                        mask |= 0xF0;
                    } else if (token[0] != '?') {
                        return false;
                    }
                    if (lo >= 0) {
                        byte |= static_cast<std::uint8_t>(lo);
                        mask |= 0x0F;
                    } else if (token[1] != '?') {
                        return false;
                    }
                } else {
                    return false;
                }

                pattern.bytes.push_back(byte);
                pattern.masks.push_back(mask);
                token.clear();
            }

            if (ch == '\0') {
                break;
            }
        } else {
            token.push_back(ch);
        }
    }

    return !pattern.bytes.empty();
}

bool MatchesPattern(const std::uint8_t* address, const Pattern& pattern) {
    for (std::size_t i = 0; i < pattern.bytes.size(); ++i) {
        if (((address[i] ^ pattern.bytes[i]) & pattern.masks[i]) != 0) {
            return false;
        }
    }
    return true;
}

uintptr_t FindPatternInText(const char* aob) {
    Pattern pattern{};
    Region text{};
    if (!ParsePattern(aob, pattern) || !RegionFromMainModuleText(text) || text.size < pattern.bytes.size()) {
        return 0;
    }

    const auto* address = reinterpret_cast<const std::uint8_t*>(text.base);
    const auto* end = address + text.size - pattern.bytes.size();
    for (; address <= end; ++address) {
        if (MatchesPattern(address, pattern)) {
            return reinterpret_cast<uintptr_t>(address);
        }
    }

    return 0;
}

bool WriteProtectedMemory(uintptr_t address, const void* data, std::size_t size) {
    DWORD old_protect = 0;
    if (!VirtualProtect(reinterpret_cast<void*>(address), size, PAGE_EXECUTE_READWRITE, &old_protect)) {
        return false;
    }

    std::memcpy(reinterpret_cast<void*>(address), data, size);
    FlushInstructionCache(GetCurrentProcess(), reinterpret_cast<void*>(address), size);

    DWORD ignored = 0;
    VirtualProtect(reinterpret_cast<void*>(address), size, old_protect, &ignored);
    return true;
}

void* AllocateNearbyExecutableMemory(uintptr_t target, std::size_t size) {
    SYSTEM_INFO info{};
    GetSystemInfo(&info);

    const uintptr_t granularity = static_cast<uintptr_t>(info.dwAllocationGranularity);
    const uintptr_t min_address = reinterpret_cast<uintptr_t>(info.lpMinimumApplicationAddress);
    const uintptr_t max_address = reinterpret_cast<uintptr_t>(info.lpMaximumApplicationAddress);
    const uintptr_t max_distance = 0x7FFF0000;
    const uintptr_t aligned_target = target & ~(granularity - 1);

    for (uintptr_t distance = 0; distance <= max_distance; distance += granularity) {
        const uintptr_t high = aligned_target + distance;
        if (high >= min_address && high <= max_address) {
            if (void* memory = VirtualAlloc(reinterpret_cast<void*>(high), size, MEM_RESERVE | MEM_COMMIT, PAGE_EXECUTE_READWRITE)) {
                return memory;
            }
        }

        if (distance == 0 || aligned_target < distance) {
            continue;
        }

        const uintptr_t low = aligned_target - distance;
        if (low >= min_address && low <= max_address) {
            if (void* memory = VirtualAlloc(reinterpret_cast<void*>(low), size, MEM_RESERVE | MEM_COMMIT, PAGE_EXECUTE_READWRITE)) {
                return memory;
            }
        }
    }

    return nullptr;
}

bool MakeRelativeJump(std::array<std::uint8_t, kInlinePatchSize>& patch, uintptr_t source, uintptr_t destination) {
    const std::int64_t delta =
        static_cast<std::int64_t>(destination) - static_cast<std::int64_t>(source + kInlinePatchSize);
    if (delta < INT32_MIN || delta > INT32_MAX) {
        return false;
    }

    patch[0] = 0xE9;
    const auto rel32 = static_cast<std::int32_t>(delta);
    std::memcpy(patch.data() + 1, &rel32, sizeof(rel32));
    return true;
}

bool CapturePatchTarget(MovementMods::CodePatchState& state, uintptr_t target, std::size_t patch_size) {
    if (patch_size == 0 || patch_size > state.original_bytes.size() || target == 0) {
        return false;
    }

    if (state.captured) {
        return true;
    }

    state.target = target;
    state.patch_size = patch_size;
    if (!Game::ReadMemory(state.target, state.original_bytes.data(), state.patch_size)) {
        state.target = 0;
        state.patch_size = 0;
        return false;
    }

    state.captured = true;
    return true;
}

bool InstallFollowCamHook(MovementMods::CodePatchState& state) {
    if (!state.captured && !CapturePatchTarget(state, FindPatternInText(kFollowCamPattern), 8)) {
        return false;
    }
    if (state.active) {
        return true;
    }

    state.cave = AllocateNearbyExecutableMemory(state.target, 0x80);
    if (state.cave == nullptr) {
        return false;
    }

    auto* cave = reinterpret_cast<std::uint8_t*>(state.cave);
    std::size_t cursor = 0;

    const std::uint8_t mov_store[] = {0x48, 0xB8};
    std::memcpy(cave + cursor, mov_store, sizeof(mov_store));
    cursor += sizeof(mov_store);
    const uintptr_t storage = reinterpret_cast<uintptr_t>(&g_FollowCamAnglesPtr);
    std::memcpy(cave + cursor, &storage, sizeof(storage));
    cursor += sizeof(storage);
    const std::uint8_t write_ptr[] = {0x48, 0x89, 0x18};
    std::memcpy(cave + cursor, write_ptr, sizeof(write_ptr));
    cursor += sizeof(write_ptr);

    std::memcpy(cave + cursor, state.original_bytes.data(), state.patch_size);
    cursor += state.patch_size;

    std::array<std::uint8_t, kInlinePatchSize> cave_jump{};
    if (!MakeRelativeJump(cave_jump, reinterpret_cast<uintptr_t>(cave + cursor), state.target + state.patch_size)) {
        VirtualFree(state.cave, 0, MEM_RELEASE);
        state.cave = nullptr;
        return false;
    }
    std::memcpy(cave + cursor, cave_jump.data(), cave_jump.size());

    std::array<std::uint8_t, 8> target_patch{};
    std::array<std::uint8_t, kInlinePatchSize> target_jump{};
    if (!MakeRelativeJump(target_jump, state.target, reinterpret_cast<uintptr_t>(state.cave))) {
        VirtualFree(state.cave, 0, MEM_RELEASE);
        state.cave = nullptr;
        return false;
    }
    std::memcpy(target_patch.data(), target_jump.data(), target_jump.size());
    std::memset(target_patch.data() + kInlinePatchSize, 0x90, target_patch.size() - kInlinePatchSize);

    if (!WriteProtectedMemory(state.target, target_patch.data(), state.patch_size)) {
        VirtualFree(state.cave, 0, MEM_RELEASE);
        state.cave = nullptr;
        return false;
    }

    state.active = true;
    return true;
}

bool InstallFreecamPositionHook(MovementMods::CodePatchState& state) {
    if (!state.captured && !CapturePatchTarget(state, FindPatternInText(kFreecamPositionPattern), 10)) {
        return false;
    }
    if (state.active) {
        return true;
    }

    state.cave = AllocateNearbyExecutableMemory(state.target, 0x100);
    if (state.cave == nullptr) {
        return false;
    }

    auto* cave = reinterpret_cast<std::uint8_t*>(state.cave);
    std::size_t cursor = 0;

    const std::uint8_t store_ptr[] = {0x48, 0xB8};
    std::memcpy(cave + cursor, store_ptr, sizeof(store_ptr));
    cursor += sizeof(store_ptr);
    const uintptr_t storage = reinterpret_cast<uintptr_t>(&g_FreecamTransformPtr);
    std::memcpy(cave + cursor, &storage, sizeof(storage));
    cursor += sizeof(storage);
    const std::uint8_t mov_store[] = {0x48, 0x89, 0x38};
    std::memcpy(cave + cursor, mov_store, sizeof(mov_store));
    cursor += sizeof(mov_store);

    const std::uint8_t cmp_toggle[] = {
        0x48, 0xB8
    };
    std::memcpy(cave + cursor, cmp_toggle, sizeof(cmp_toggle));
    cursor += sizeof(cmp_toggle);
    const uintptr_t toggle_address = reinterpret_cast<uintptr_t>(&Main::g_FeatureStatus.freecam_enabled);
    std::memcpy(cave + cursor, &toggle_address, sizeof(toggle_address));
    cursor += sizeof(toggle_address);
    const std::uint8_t cmp_bytes[] = {
        0x80, 0x38, 0x00,
        0x75, 0x03,
        0x0F, 0x29, 0x37,
        0x80, 0xBB, 0x15, 0x03, 0x00, 0x00, 0x00
    };
    std::memcpy(cave + cursor, cmp_bytes, sizeof(cmp_bytes));
    cursor += sizeof(cmp_bytes);

    std::array<std::uint8_t, kInlinePatchSize> cave_jump{};
    if (!MakeRelativeJump(cave_jump, reinterpret_cast<uintptr_t>(cave + cursor), state.target + state.patch_size)) {
        VirtualFree(state.cave, 0, MEM_RELEASE);
        state.cave = nullptr;
        return false;
    }
    std::memcpy(cave + cursor, cave_jump.data(), cave_jump.size());

    std::array<std::uint8_t, 10> target_patch{};
    std::array<std::uint8_t, kInlinePatchSize> target_jump{};
    if (!MakeRelativeJump(target_jump, state.target, reinterpret_cast<uintptr_t>(state.cave))) {
        VirtualFree(state.cave, 0, MEM_RELEASE);
        state.cave = nullptr;
        return false;
    }
    std::memcpy(target_patch.data(), target_jump.data(), target_jump.size());
    std::memset(target_patch.data() + kInlinePatchSize, 0x90, target_patch.size() - kInlinePatchSize);

    if (!WriteProtectedMemory(state.target, target_patch.data(), state.patch_size)) {
        VirtualFree(state.cave, 0, MEM_RELEASE);
        state.cave = nullptr;
        return false;
    }

    state.active = true;
    return true;
}

bool InstallFreecamRotationHook(MovementMods::CodePatchState& state) {
    if (!state.captured && !CapturePatchTarget(state, FindPatternInText(kFreecamRotationPattern), 7)) {
        return false;
    }
    if (state.active) {
        return true;
    }

    state.cave = AllocateNearbyExecutableMemory(state.target, 0x80);
    if (state.cave == nullptr) {
        return false;
    }

    auto* cave = reinterpret_cast<std::uint8_t*>(state.cave);
    std::size_t cursor = 0;
    const std::uint8_t cmp_toggle[] = {0x48, 0xB8};
    std::memcpy(cave + cursor, cmp_toggle, sizeof(cmp_toggle));
    cursor += sizeof(cmp_toggle);
    const uintptr_t toggle_address = reinterpret_cast<uintptr_t>(&Main::g_FeatureStatus.freecam_enabled);
    std::memcpy(cave + cursor, &toggle_address, sizeof(toggle_address));
    cursor += sizeof(toggle_address);
    const std::uint8_t branch_and_copy[] = {
        0x80, 0x38, 0x00,
        0x75, 0x07,
        0x0F, 0x29, 0xA6, 0x50, 0x01, 0x00, 0x00
    };
    std::memcpy(cave + cursor, branch_and_copy, sizeof(branch_and_copy));
    cursor += sizeof(branch_and_copy);

    std::array<std::uint8_t, kInlinePatchSize> cave_jump{};
    if (!MakeRelativeJump(cave_jump, reinterpret_cast<uintptr_t>(cave + cursor), state.target + state.patch_size)) {
        VirtualFree(state.cave, 0, MEM_RELEASE);
        state.cave = nullptr;
        return false;
    }
    std::memcpy(cave + cursor, cave_jump.data(), cave_jump.size());

    std::array<std::uint8_t, 7> target_patch{};
    std::array<std::uint8_t, kInlinePatchSize> target_jump{};
    if (!MakeRelativeJump(target_jump, state.target, reinterpret_cast<uintptr_t>(state.cave))) {
        VirtualFree(state.cave, 0, MEM_RELEASE);
        state.cave = nullptr;
        return false;
    }
    std::memcpy(target_patch.data(), target_jump.data(), target_jump.size());
    std::memset(target_patch.data() + kInlinePatchSize, 0x90, target_patch.size() - kInlinePatchSize);

    if (!WriteProtectedMemory(state.target, target_patch.data(), state.patch_size)) {
        VirtualFree(state.cave, 0, MEM_RELEASE);
        state.cave = nullptr;
        return false;
    }

    state.active = true;
    return true;
}

float ReadAngleValue(uintptr_t base, uintptr_t offset) {
    float value = 0.0f;
    Game::ReadValue(base + offset, value);
    return value;
}

bool ReadTransformVector(uintptr_t address, float& x, float& y, float& z) {
    return Game::ReadValue(address + 0x0, x) &&
           Game::ReadValue(address + 0x4, y) &&
           Game::ReadValue(address + 0x8, z);
}

bool WriteTransformVector(uintptr_t address, float x, float y, float z) {
    return Game::WriteValue(address + 0x0, x) &&
           Game::WriteValue(address + 0x4, y) &&
           Game::WriteValue(address + 0x8, z);
}

void ApplyDirectionalMovement(float& x,
                              float& y,
                              float& z,
                              float horizontal,
                              float vertical,
                              float speed,
                              uintptr_t forward_key,
                              uintptr_t back_key,
                              uintptr_t left_key,
                              uintptr_t right_key,
                              uintptr_t up_key,
                              uintptr_t down_key) {
    const float sin_h = std::sin(horizontal);
    const float cos_h = std::cos(horizontal);
    const float sin_v = std::sin(vertical);
    const float right_h = horizontal - (3.14159265359f / 2.0f);
    const float sin_r = std::sin(right_h);
    const float cos_r = std::cos(right_h);

    if (GetAsyncKeyState(static_cast<int>(forward_key)) & 0x8000) {
        x += sin_h * speed;
        y -= sin_v * speed;
        z += cos_h * speed;
    }
    if (GetAsyncKeyState(static_cast<int>(back_key)) & 0x8000) {
        x -= sin_h * speed;
        y += sin_v * speed;
        z -= cos_h * speed;
    }
    if (GetAsyncKeyState(static_cast<int>(left_key)) & 0x8000) {
        x += sin_r * speed;
        z += cos_r * speed;
    }
    if (GetAsyncKeyState(static_cast<int>(right_key)) & 0x8000) {
        x -= sin_r * speed;
        z -= cos_r * speed;
    }
    if (GetAsyncKeyState(static_cast<int>(up_key)) & 0x8000) {
        y += speed;
    }
    if (GetAsyncKeyState(static_cast<int>(down_key)) & 0x8000) {
        y -= speed;
    }
}

bool GetPlayerMovementNode(uintptr_t world_chr_man, uintptr_t& movement_node) {
    return world_chr_man != 0 &&
           Game::ResolvePointerChain(world_chr_man, {kNetPlayersOffset, 0x0, 0x190}, 0x68, movement_node);
}

}  // namespace

MovementMods::~MovementMods() noexcept {
    StopWorker();
}

void MovementMods::Tick(const Game::SingletonRegistry& singletons) {
    ERD_PROTECTED_STEP("MovementMods.EnsureHooksInstalled", EnsureHooksInstalled());
    ERD_PROTECTED_STEP("MovementMods.SyncNoFallCamera", SyncNoFallCamera());
    ERD_PROTECTED_STEP("MovementMods.SyncWorker", SyncWorker(singletons));
}

void MovementMods::EnsureHooksInstalled() {
    if (!follow_cam_hook_.active && !InstallFollowCamHook(follow_cam_hook_) && !follow_cam_error_logged_) {
        Main::Logger::Instance().Error("Failed to install NoClip follow-cam hook.");
        follow_cam_error_logged_ = true;
    }
    if (!freecam_position_hook_.active && !InstallFreecamPositionHook(freecam_position_hook_) && !freecam_position_error_logged_) {
        Main::Logger::Instance().Error("Failed to install FreeCam position hook.");
        freecam_position_error_logged_ = true;
    }
    if (!freecam_rotation_hook_.active && !InstallFreecamRotationHook(freecam_rotation_hook_) && !freecam_rotation_error_logged_) {
        Main::Logger::Instance().Error("Failed to install FreeCam rotation hook.");
        freecam_rotation_error_logged_ = true;
    }
}

void MovementMods::SyncNoFallCamera() {
    if (!no_fall_camera_patch_.captured) {
        const uintptr_t pattern = FindPatternInText(kNoFallCameraPattern);
        if (pattern == 0 || !CapturePatchTarget(no_fall_camera_patch_, pattern + 0x11, 2)) {
            if (!no_fall_camera_error_logged_) {
                Main::Logger::Instance().Error("Failed to locate No Fall Camera patch target.");
                no_fall_camera_error_logged_ = true;
            }
            return;
        }
    }

    const bool enabled = Main::g_FeatureStatus.no_fall_camera.load();
    if (enabled == no_fall_camera_patch_.active) {
        return;
    }

    if (enabled) {
        const std::uint8_t patch[2] = {0x90, 0x90};
        if (WriteProtectedMemory(no_fall_camera_patch_.target, patch, sizeof(patch))) {
            no_fall_camera_patch_.active = true;
        }
    } else if (WriteProtectedMemory(
                   no_fall_camera_patch_.target,
                   no_fall_camera_patch_.original_bytes.data(),
                   no_fall_camera_patch_.patch_size)) {
        no_fall_camera_patch_.active = false;
    }
}

void MovementMods::SyncWorker(const Game::SingletonRegistry& singletons) {
    world_chr_storage_address_ = singletons.GetStorage("WorldChrMan");
    const bool should_run =
        Main::g_FeatureStatus.no_clip_enabled.load() || Main::g_FeatureStatus.freecam_enabled.load();

    if (should_run && !worker_running_.load()) {
        StartWorker();
    } else if (!should_run && worker_running_.load()) {
        StopWorker();
    }
}

void MovementMods::StartWorker() {
    if (worker_running_.exchange(true)) {
        return;
    }

    worker_thread_ = std::thread([this]() { WorkerLoop(); });
    Main::Logger::Instance().Info("Movement mods worker started.");
}

void MovementMods::StopWorker() {
    if (!worker_running_.exchange(false)) {
        return;
    }

    if (worker_thread_.joinable()) {
        worker_thread_.join();
    }
    Main::Logger::Instance().Info("Movement mods worker stopped.");
}

void MovementMods::WorkerLoop() {
    while (worker_running_.load() && g_Running) {
        Sleep(10);

        const float speed = std::clamp(Main::g_FeatureStatus.movement_speed.load(), 0.1f, 20.0f) / 10.0f;

        uintptr_t world_chr_man = 0;
        if (world_chr_storage_address_ == 0 || !Game::ReadValue(world_chr_storage_address_, world_chr_man) || world_chr_man == 0) {
            continue;
        }

        if (Main::g_FeatureStatus.no_clip_enabled.load()) {
            uintptr_t movement_node = 0;
            if (GetPlayerMovementNode(world_chr_man, movement_node) && movement_node != 0) {
                const std::uint16_t move_flags = 0x0101;
                Game::WriteValue(movement_node + 0x92, move_flags);

                float x = 0.0f;
                float z = 0.0f;
                float y = 0.0f;
                if (ReadTransformVector(movement_node + 0x70, x, z, y)) {
                    const uintptr_t follow_cam = g_FollowCamAnglesPtr.load();
                    const float horizontal = follow_cam != 0 ? ReadAngleValue(follow_cam, 0x15C) : 0.0f;
                    const float vertical = follow_cam != 0 ? ReadAngleValue(follow_cam, 0x160) : 0.0f;
                    ApplyDirectionalMovement(x, y, z, horizontal, vertical, speed, 'W', 'S', 'A', 'D', VK_SPACE, VK_CONTROL);
                    WriteTransformVector(movement_node + 0x70, x, z, y);
                }
            }
        }

        if (Main::g_FeatureStatus.freecam_enabled.load()) {
            const uintptr_t freecam_ptr = g_FreecamTransformPtr.load();
            if (freecam_ptr != 0) {
                float x = 0.0f;
                float y = 0.0f;
                float z = 0.0f;
                if (ReadTransformVector(freecam_ptr, x, y, z)) {
                    const float horizontal = ReadAngleValue(freecam_ptr, 0xB4);
                    const float vertical = ReadAngleValue(freecam_ptr, 0xB8);
                    ApplyDirectionalMovement(x, y, z, horizontal, vertical, speed, 'W', 'S', 'A', 'D', VK_SPACE, VK_CONTROL);
                    WriteTransformVector(freecam_ptr, x, y, z);
                }
            }
        }
    }
}

}  // namespace ERD::Features
