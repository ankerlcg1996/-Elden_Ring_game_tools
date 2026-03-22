#include "PostureBars.hpp"

#include "../Game/Memory.hpp"
#include "../Main/Logger.hpp"
#include "../ThirdParty/MinHook/MinHook.h"

#include <algorithm>
#include <limits>

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

constexpr std::size_t kBossBarCount = 3;
constexpr std::size_t kEntityBarCount = 8;
constexpr float kPlayerBarX = 963.0f;
constexpr float kPlayerBarY = 100.0f;
constexpr float kBossBarX = 963.0f;
constexpr float kBossBarY = 945.0f;
constexpr float kBossBarSpacingY = 55.0f;
constexpr const char* kWorldChrPattern = "48 8B 05 ? ? ? ? 48 85 C0 74 0F 48 39 88";
constexpr const char* kCsFeManPattern =
    "48 8B 0D ? ? ? ? 8B DA 48 85 C9 75 ? 48 8D 0D ? ? ? ? E8 ? ? ? ? 4C 8B C8 4C 8D 05 ? ? ? ? BA B4 00 00 00 48 8D 0D ? ? ? ? E8 ? ? ? ? 48 8B 0D ? ? ? ? 8B D3 E8 ? ? ? ? 48 8B D8";
constexpr const char* kGetChrInsFromHandlePattern =
    "48 83 EC 28 E8 17 FF FF FF 48 85 C0 74 08 48 8B 00 48 83 C4 28 C3";
constexpr const char* kUpdateUiBarsPattern =
    "40 55 56 57 41 54 41 55 41 56 41 57 48 83 EC 60 48 C7 44 24 30 FE FF FF FF 48 89 9C 24 B0 00 00 00 48 8B 05 ? ? ? ? 48 33 C4 48 89 44 24 58 48";

struct BossHpBar {
    int display_id = -1;
    std::uint32_t padding_0x4 = 0;
    std::uint64_t boss_handle = 0;
    int current_display_damage = 0;
    int unknown_0x14 = 0;
    bool is_hit = false;
    std::uint8_t padding_0x19[0x3]{};
    float display_time = 0.0f;
};

struct EntityHpBar {
    std::uint64_t entity_handle = 0;
    float unknown[2]{};
    float screen_pos_x = 0.0f;
    float screen_pos_y = 0.0f;
    float distance_modifier = 1.0f;
    std::uint32_t unknown_0x18 = 0;
    char padding_0x1c = 0;
    char padding_0x1d = 0;
    std::uint16_t padding_0x1e = 0;
    int current_display_damage = -1;
    int previous_hp = 0;
    float time_displayed = 0.0f;
    float total_time_displayed = 0.0f;
    bool is_visible = false;
    char padding_0x35[11]{};
};

struct CSFeManImp {
    std::uint8_t padding[0x59F0]{};
    EntityHpBar entity_hp_bars[kEntityBarCount]{};
    BossHpBar boss_hp_bars[kBossBarCount]{};
};

struct StatModule {
    std::uint8_t padding[0x138]{};
    int health = 0;
    int health_max = 0;
    int health_base = 0;
    int unknown = 0;
    int focus = 0;
    int focus_max = 0;
    int focus_base = 0;
    int stamina = 0;
    int stamina_max = 0;
    int stamina_base = 0;
};

struct StaggerModule {
    std::uint8_t padding[0x10]{};
    float stagger = 0.0f;
    float stagger_max = 0.0f;
    std::uint8_t padding_0x18[0x4]{};
    float reset_timer = 0.0f;
};

struct ChrModuleBag {
    StatModule* stat_module = nullptr;
    std::uint8_t padding_0x8[0x10]{};
    void* anim_module = nullptr;
    void* resistance_module = nullptr;
    std::uint8_t padding_0x28[0x18]{};
    StaggerModule* stagger_module = nullptr;
};

struct ChrIns {
    std::uint8_t padding_0x0[0x8]{};    
    std::uint64_t handle = 0;
    std::uint8_t padding_0x10[0x50]{};  
    int npc_param = 0;
    int model_number = 0;
    int chr_type = 0;
    std::uint8_t team_type = 0;
    std::uint8_t padding_0x69[0x123]{};
    ChrModuleBag* chr_module_bag = nullptr;
    std::uint8_t padding_0x190[0x508]{};
    std::uint64_t target_handle = 0;
};

struct WorldChrMan {
    std::uint8_t padding[0x10EF8]{};
    ChrIns** player_array[4]{};
};

bool RegionFromMainModuleText(Region& region) {
    HMODULE module = GetModuleHandleW(nullptr);
    if (module == nullptr) {
        return false;
    }

    const auto* dos = reinterpret_cast<const IMAGE_DOS_HEADER*>(module);
    const auto* nt = reinterpret_cast<const IMAGE_NT_HEADERS*>(
        reinterpret_cast<uintptr_t>(module) + dos->e_lfanew
    );
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

uintptr_t ResolveRipAddress(uintptr_t instruction, std::size_t displacement_offset, std::size_t instruction_size) {
    std::int32_t displacement = 0;
    if (instruction == 0 || !Game::ReadValue(instruction + displacement_offset, displacement)) {
        return 0;
    }

    return instruction + instruction_size + displacement;
}

bool ExtractBarValues(const ChrIns* chr, PostureBarEntry& entry, bool allow_stamina) {
    if (chr == nullptr || chr->chr_module_bag == nullptr) {
        return false;
    }

    const ChrModuleBag* const modules = chr->chr_module_bag;
    if (modules->stat_module == nullptr || modules->stagger_module == nullptr) {
        return false;
    }

    const StatModule* const stat = modules->stat_module;
    const StaggerModule* const stagger = modules->stagger_module;

    const bool use_stamina = allow_stamina && chr->model_number == 0 && stat->stamina_max > 0;
    const float current = use_stamina ? static_cast<float>(stat->stamina) : stagger->stagger;
    const float maximum = use_stamina ? static_cast<float>(stat->stamina_max) : stagger->stagger_max;
    if (maximum <= 0.0f) {
        return false;
    }

    entry.handle = chr->handle;
    entry.current = current;
    entry.maximum = maximum;
    entry.hp = stat->health;
    entry.max_hp = stat->health_max;
    entry.use_stamina = use_stamina;
    return true;
}

}  // namespace

PostureBars* PostureBars::instance_ = nullptr;

PostureBars::PostureBars() {
    instance_ = this;
}

PostureBars::~PostureBars() noexcept {
    if (update_ui_bars_target_ != 0) {
        MH_DisableHook(reinterpret_cast<void*>(update_ui_bars_target_));
        MH_RemoveHook(reinterpret_cast<void*>(update_ui_bars_target_));
    }

    if (instance_ == this) {
        instance_ = nullptr;
    }
}

void PostureBars::Tick() {
    if (hook_installed_) {
        return;
    }

    if (!EnsureHookInstalled() && !install_error_logged_) {
        Main::Logger::Instance().Error("Failed to install integrated PostureBar hook.");
        install_error_logged_ = true;
    }
}

PostureBarsSnapshot PostureBars::Snapshot() const {
    std::scoped_lock lock(snapshot_mutex_);
    return snapshot_;
}

bool PostureBars::ResolveCurrentTarget(uintptr_t& base, std::uint64_t& handle) {
    base = 0;
    handle = 0;

    if (world_chr_storage_address_ == 0) {
        const uintptr_t match = FindPatternInText(kWorldChrPattern);
        world_chr_storage_address_ = ResolveRipAddress(match, 3, 7);
    }
    if (get_chr_ins_from_handle_ == nullptr) {
        const uintptr_t match = FindPatternInText(kGetChrInsFromHandlePattern);
        get_chr_ins_from_handle_ = reinterpret_cast<GetChrInsFromHandleFn>(match);
    }
    if (world_chr_storage_address_ == 0 || get_chr_ins_from_handle_ == nullptr) {
        return false;
    }

    uintptr_t world_chr_address = 0;
    if (!Game::ReadValue(world_chr_storage_address_, world_chr_address) || world_chr_address == 0) {
        return false;
    }

    auto* world_chr_man = reinterpret_cast<WorldChrMan*>(world_chr_address);
    if (world_chr_man->player_array[0] == nullptr || *world_chr_man->player_array[0] == nullptr) {
        return false;
    }

    const ChrIns* const player_chr = *world_chr_man->player_array[0];
    handle = player_chr->target_handle;
    if (handle == 0 || handle == std::numeric_limits<std::uint64_t>::max()) {
        handle = 0;
        return false;
    }

    base = reinterpret_cast<uintptr_t>(get_chr_ins_from_handle_(world_chr_man, &handle));
    if (base == 0) {
        handle = 0;
        return false;
    }

    return true;
}

PostureBars* PostureBars::Instance() {
    return instance_;
}

bool PostureBars::EnsureHookInstalled() {
    if (hook_installed_) {
        return true;
    }

    const MH_STATUS init_status = MH_Initialize();
    if (init_status != MH_OK && init_status != MH_ERROR_ALREADY_INITIALIZED) {
        return false;
    }

    if (world_chr_storage_address_ == 0) {
        const uintptr_t match = FindPatternInText(kWorldChrPattern);
        world_chr_storage_address_ = ResolveRipAddress(match, 3, 7);
    }
    if (cs_fe_man_storage_address_ == 0) {
        const uintptr_t match = FindPatternInText(kCsFeManPattern);
        cs_fe_man_storage_address_ = ResolveRipAddress(match, 3, 7);
    }
    if (get_chr_ins_from_handle_ == nullptr) {
        const uintptr_t match = FindPatternInText(kGetChrInsFromHandlePattern);
        get_chr_ins_from_handle_ = reinterpret_cast<GetChrInsFromHandleFn>(match);
    }
    if (update_ui_bars_target_ == 0) {
        update_ui_bars_target_ = FindPatternInText(kUpdateUiBarsPattern);
    }

    if (world_chr_storage_address_ == 0 ||
        cs_fe_man_storage_address_ == 0 ||
        get_chr_ins_from_handle_ == nullptr ||
        update_ui_bars_target_ == 0) {
        return false;
    }

    MH_STATUS create_status = MH_CreateHook(
        reinterpret_cast<void*>(update_ui_bars_target_),
        reinterpret_cast<void*>(&HookUpdateUiBars),
        reinterpret_cast<void**>(&original_update_ui_bars_)
    );
    if (create_status != MH_OK && create_status != MH_ERROR_ALREADY_CREATED) {
        return false;
    }

    const MH_STATUS enable_status = MH_EnableHook(reinterpret_cast<void*>(update_ui_bars_target_));
    if (enable_status != MH_OK && enable_status != MH_ERROR_ENABLED) {
        return false;
    }

    hook_installed_ = true;
    {
        std::scoped_lock lock(snapshot_mutex_);
        snapshot_.hook_installed = true;
    }
    Main::Logger::Instance().Info("Integrated PostureBar hook installed.");
    return true;
}

void PostureBars::OnUpdateUiBars(uintptr_t move_map_step, uintptr_t time) {
    if (original_update_ui_bars_ != nullptr) {
        original_update_ui_bars_(move_map_step, time);
    }

    uintptr_t world_chr_address = 0;
    uintptr_t cs_fe_man_address = 0;
    if (!Game::ReadValue(world_chr_storage_address_, world_chr_address) ||
        !Game::ReadValue(cs_fe_man_storage_address_, cs_fe_man_address) ||
        world_chr_address == 0 ||
        cs_fe_man_address == 0 ||
        get_chr_ins_from_handle_ == nullptr) {
        std::scoped_lock lock(snapshot_mutex_);
        snapshot_ = {};
        snapshot_.hook_installed = hook_installed_;
        return;
    }

    auto* world_chr_man = reinterpret_cast<WorldChrMan*>(world_chr_address);
    auto* cs_fe_man = reinterpret_cast<CSFeManImp*>(cs_fe_man_address);

    PostureBarsSnapshot next_snapshot{};
    next_snapshot.hook_installed = hook_installed_;
    next_snapshot.boss_bars.reserve(kBossBarCount);
    next_snapshot.entity_bars.reserve(kEntityBarCount);

    if (world_chr_man->player_array[0] != nullptr && *world_chr_man->player_array[0] != nullptr) {
        const ChrIns* const player_chr_direct = *world_chr_man->player_array[0];
        std::uint64_t player_handle = player_chr_direct->handle;
        if (auto* player_chr = reinterpret_cast<ChrIns*>(get_chr_ins_from_handle_(world_chr_man, &player_handle));
            player_chr != nullptr) {
            PostureBarEntry player_entry{};
            if (ExtractBarValues(player_chr, player_entry, false)) {
                player_entry.visible = player_entry.hp > 0;
                player_entry.screen_x = kPlayerBarX;
                player_entry.screen_y = kPlayerBarY;
                next_snapshot.player_bar = player_entry;
            }
        }

        std::uint64_t locked_handle = player_chr_direct->target_handle;
        if (locked_handle != 0 && locked_handle != std::numeric_limits<std::uint64_t>::max()) {
            if (auto* locked_chr = reinterpret_cast<ChrIns*>(get_chr_ins_from_handle_(world_chr_man, &locked_handle));
                locked_chr != nullptr) {
                PostureBarEntry locked_entry{};
                if (ExtractBarValues(locked_chr, locked_entry, true)) {
                    locked_entry.visible = locked_entry.hp > 0;
                    locked_entry.screen_x = kBossBarX;
                    locked_entry.screen_y = 150.0f;
                    next_snapshot.locked_target_bar = locked_entry;
                }
            }
        }
    }

    for (std::size_t i = 0; i < kBossBarCount; ++i) {
        const BossHpBar& source = cs_fe_man->boss_hp_bars[i];
        if (source.boss_handle == std::numeric_limits<std::uint64_t>::max()) {
            continue;
        }

        std::uint64_t handle = source.boss_handle;
        auto* chr = reinterpret_cast<ChrIns*>(get_chr_ins_from_handle_(world_chr_man, &handle));
        if (chr == nullptr) {
            continue;
        }

        PostureBarEntry entry{};
        if (!ExtractBarValues(chr, entry, true)) {
            continue;
        }

        entry.recent_damage = std::max(source.current_display_damage, 0);
        entry.slot = i;
        entry.visible = entry.hp > 0;
        entry.screen_x = kBossBarX;
        entry.screen_y = kBossBarY - static_cast<float>(i) * kBossBarSpacingY;
        next_snapshot.boss_bars.push_back(entry);
    }

    for (std::size_t i = 0; i < kEntityBarCount; ++i) {
        const EntityHpBar& source = cs_fe_man->entity_hp_bars[i];
        if (source.entity_handle == std::numeric_limits<std::uint64_t>::max()) {
            continue;
        }

        std::uint64_t handle = source.entity_handle;
        auto* chr = reinterpret_cast<ChrIns*>(get_chr_ins_from_handle_(world_chr_man, &handle));
        if (chr == nullptr) {
            continue;
        }

        PostureBarEntry entry{};
        if (!ExtractBarValues(chr, entry, true)) {
            continue;
        }

        entry.recent_damage = std::max(source.current_display_damage, 0);
        entry.slot = i;
        entry.visible = source.is_visible && entry.hp > 0;
        entry.screen_x = source.screen_pos_x;
        entry.screen_y = source.screen_pos_y;
        entry.distance_modifier = std::clamp(source.distance_modifier, 0.35f, 3.0f);
        next_snapshot.entity_bars.push_back(entry);
    }

    static bool first_snapshot_logged = false;
    if (!first_snapshot_logged &&
        (next_snapshot.player_bar.has_value() || !next_snapshot.boss_bars.empty() || !next_snapshot.entity_bars.empty())) {
        first_snapshot_logged = true;
        Main::Logger::Instance().Info(
            "Integrated PostureBar snapshot ready. Player=" +
            std::to_string(next_snapshot.player_bar.has_value() ? 1 : 0) +
            ", Boss=" + std::to_string(next_snapshot.boss_bars.size()) +
            ", Entity=" + std::to_string(next_snapshot.entity_bars.size())
        );
    }

    std::scoped_lock lock(snapshot_mutex_);
    snapshot_ = std::move(next_snapshot);
}

void PostureBars::HookUpdateUiBars(uintptr_t move_map_step, uintptr_t time) {
    if (instance_ != nullptr) {
        instance_->OnUpdateUiBars(move_map_step, time);
    }
}

PostureBarsSnapshot GetPostureBarsSnapshot() {
    if (PostureBars::Instance() == nullptr) {
        return {};
    }
    return PostureBars::Instance()->Snapshot();
}

bool ResolveCurrentTargetFromPostureBars(uintptr_t& base, std::uint64_t& handle) {
    if (PostureBars::Instance() == nullptr) {
        base = 0;
        handle = 0;
        return false;
    }

    return PostureBars::Instance()->ResolveCurrentTarget(base, handle);
}

}  // namespace ERD::Features
