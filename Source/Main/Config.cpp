#include "Config.hpp"

#include <algorithm>
#include <cctype>
#include <fstream>

namespace ERD::Main {
namespace {

std::string Trim(std::string value) {
    const auto not_space = [](unsigned char ch) { return !std::isspace(ch); };

    value.erase(value.begin(), std::find_if(value.begin(), value.end(), not_space));
    value.erase(std::find_if(value.rbegin(), value.rend(), not_space).base(), value.end());
    return value;
}

std::string ToLower(std::string value) {
    std::transform(value.begin(), value.end(), value.begin(), [](unsigned char ch) {
        return static_cast<char>(std::tolower(ch));
    });
    return value;
}

bool ParseBool(const std::string& value, bool fallback) {
    const std::string lowered = ToLower(Trim(value));
    if (lowered == "1" || lowered == "true" || lowered == "yes" || lowered == "on") {
        return true;
    }
    if (lowered == "0" || lowered == "false" || lowered == "no" || lowered == "off") {
        return false;
    }
    return fallback;
}

float ParseFloat(const std::string& value, float fallback) {
    try {
        return std::stof(Trim(value));
    } catch (...) {
        return fallback;
    }
}

int ParseInt(const std::string& value, int fallback) {
    try {
        return std::stoi(Trim(value));
    } catch (...) {
        return fallback;
    }
}

std::string WideToUtf8(const std::wstring& value) {
    if (value.empty()) {
        return {};
    }

    const int required = WideCharToMultiByte(
        CP_UTF8,
        0,
        value.c_str(),
        static_cast<int>(value.size()),
        nullptr,
        0,
        nullptr,
        nullptr
    );
    if (required <= 0) {
        return {};
    }

    std::string utf8(static_cast<std::size_t>(required), '\0');
    WideCharToMultiByte(
        CP_UTF8,
        0,
        value.c_str(),
        static_cast<int>(value.size()),
        utf8.data(),
        required,
        nullptr,
        nullptr
    );
    return utf8;
}

void ApplyBool(ModConfig& config, const std::string& key, const std::string& value) {
    if (key == "general.create_console") {
        config.create_console = ParseBool(value, config.create_console);
    } else if (key == "general.enable_file_log") {
        config.enable_file_log = ParseBool(value, config.enable_file_log);
    } else if (key == "general.show_message_box") {
        config.show_message_box = ParseBool(value, config.show_message_box);
    } else if (key == "overlay.menu_minimized" || key == "menu_minimized") {
        config.menu_minimized = ParseBool(value, config.menu_minimized);
    } else if (key == "characterflags.infinite_fp" || key == "infinite_fp" || key == "enable_no_fp_consumption") {
        config.infinite_fp = ParseBool(value, config.infinite_fp);
    } else if (key == "characterflags.infinite_items" || key == "infinite_items" || key == "enable_no_goods_consume") {
        config.infinite_items = ParseBool(value, config.infinite_items);
    } else if (key == "characterflags.no_stamina_consumption" || key == "no_stamina_consumption") {
        config.no_stamina_consumption = ParseBool(value, config.no_stamina_consumption);
    } else if (key == "worldunlocks.unlock_all_maps" || key == "unlock_all_maps") {
        config.unlock_all_maps = ParseBool(value, config.unlock_all_maps);
    } else if (key == "worldunlocks.unlock_all_cookbooks" || key == "unlock_all_cookbooks") {
        config.unlock_all_cookbooks = ParseBool(value, config.unlock_all_cookbooks);
    } else if (key == "worldunlocks.unlock_all_whetblades" || key == "unlock_all_whetblades") {
        config.unlock_all_whetblades = ParseBool(value, config.unlock_all_whetblades);
    } else if (key == "worldunlocks.unlock_all_graces" || key == "unlock_all_graces") {
        config.unlock_all_graces = ParseBool(value, config.unlock_all_graces);
    } else if (key == "worldunlocks.unlock_all_summoning_pools" || key == "unlock_all_summoning_pools") {
        config.unlock_all_summoning_pools = ParseBool(value, config.unlock_all_summoning_pools);
    } else if (key == "worldunlocks.unlock_all_colosseums" || key == "unlock_all_colosseums") {
        config.unlock_all_colosseums = ParseBool(value, config.unlock_all_colosseums);
    } else if (key == "parampatches.faster_respawn" || key == "faster_respawn") {
        config.faster_respawn = ParseBool(value, config.faster_respawn);
    } else if (key == "parampatches.warp_out_of_uncleared_minidungeons" || key == "warp_out_of_uncleared_minidungeons") {
        config.warp_out_of_uncleared_minidungeons =
            ParseBool(value, config.warp_out_of_uncleared_minidungeons);
    } else if (key == "parampatches.free_purchase" || key == "free_purchase") {
        config.free_purchase = ParseBool(value, config.free_purchase);
    } else if (key == "parampatches.no_crafting_material_cost" || key == "no_crafting_material_cost") {
        config.no_crafting_material_cost = ParseBool(value, config.no_crafting_material_cost);
    } else if (key == "parampatches.no_upgrade_material_cost" || key == "no_upgrade_material_cost") {
        config.no_upgrade_material_cost = ParseBool(value, config.no_upgrade_material_cost);
    } else if (key == "parampatches.no_magic_requirements" || key == "no_magic_requirements") {
        config.no_magic_requirements = ParseBool(value, config.no_magic_requirements);
    } else if (key == "parampatches.all_magic_one_slot" || key == "all_magic_one_slot") {
        config.all_magic_one_slot = ParseBool(value, config.all_magic_one_slot);
    } else if (key == "parampatches.weightless_equipment" || key == "weightless_equipment") {
        config.weightless_equipment = ParseBool(value, config.weightless_equipment);
    } else if (key == "parampatches.mount_anywhere" || key == "mount_anywhere") {
        config.mount_anywhere = ParseBool(value, config.mount_anywhere);
    } else if (key == "parampatches.spirit_ashes_anywhere" || key == "spirit_ashes_anywhere") {
        config.spirit_ashes_anywhere = ParseBool(value, config.spirit_ashes_anywhere);
    } else if (key == "parampatches.item_discovery_x10" || key == "item_discovery_x10") {
        config.item_discovery_multiplier = ParseBool(value, false) ? 10 : 1;
    } else if (key == "parampatches.permanent_lantern" || key == "permanent_lantern") {
        config.permanent_lantern = ParseBool(value, config.permanent_lantern);
    } else if (key == "parampatches.invisible_helmets" || key == "invisible_helmets") {
        config.invisible_helmets = ParseBool(value, config.invisible_helmets);
    } else if (key == "gameactions.start_next_cycle" || key == "start_next_cycle") {
        config.start_next_cycle = ParseBool(value, config.start_next_cycle);
    }
}

void ApplyInt(ModConfig& config, const std::string& key, const std::string& value) {
    if (key == "parampatches.item_discovery_multiplier" || key == "item_discovery_multiplier") {
        config.item_discovery_multiplier = std::clamp(ParseInt(value, config.item_discovery_multiplier), 1, 100);
    }
}

}  // namespace

ModConfig LoadConfig(const fs::path& config_path) {
    ModConfig config;

    std::ifstream input(config_path);
    if (!input.is_open()) {
        return config;
    }

    std::string current_section;
    std::string line;
    while (std::getline(input, line)) {
        const std::string trimmed = Trim(line);
        if (trimmed.empty() || trimmed[0] == '#' || trimmed[0] == ';') {
            continue;
        }

        if (trimmed.front() == '[' && trimmed.back() == ']') {
            current_section = ToLower(Trim(trimmed.substr(1, trimmed.size() - 2)));
            continue;
        }

        const std::size_t equals = trimmed.find('=');
        if (equals == std::string::npos) {
            continue;
        }

        const std::string key_name = ToLower(Trim(trimmed.substr(0, equals)));
        const std::string value = Trim(trimmed.substr(equals + 1));
        const std::string full_key = current_section.empty() ? key_name : current_section + "." + key_name;

        ApplyBool(config, full_key, value);
        ApplyInt(config, full_key, value);
        if (full_key == "general.message_box_text" || key_name == "message_box_text") {
            config.message_box_text.assign(value.begin(), value.end());
        } else if (full_key == "overlay.menu_opacity" || key_name == "menu_opacity") {
            config.menu_opacity = ParseFloat(value, config.menu_opacity);
        }
    }

    return config;
}

bool SaveConfig(const fs::path& config_path, const ModConfig& config) {
    std::error_code error;
    const fs::path parent = config_path.parent_path();
    if (!parent.empty()) {
        fs::create_directories(parent, error);
        if (error) {
            return false;
        }
    }

    std::ofstream output(config_path, std::ios::binary | std::ios::trunc);
    if (!output.is_open()) {
        return false;
    }

    output <<
        "; ERDMod 配置文件\n"
        "; 说明：持续开关会在游戏内菜单改动后自动保存，下次启动会自动恢复上次选择。\n"
        "; 但真正写入游戏内存会等角色已经进档后才开始，避免标题界面或加载途中误改。\n\n"
        "[General]\n"
        "; 是否创建调试控制台窗口\n"
        "create_console=" << (config.create_console ? "true" : "false") << "\n\n"
        "; 是否把日志写到 Logs/ERDMod.log\n"
        "enable_file_log=" << (config.enable_file_log ? "true" : "false") << "\n\n"
        "; 插件加载时是否弹出提示框\n"
        "show_message_box=" << (config.show_message_box ? "true" : "false") << "\n\n"
        "; 提示框显示的文字\n"
        "message_box_text=" << WideToUtf8(config.message_box_text) << "\n\n"
        "[Overlay]\n"
        "; 菜单默认是否以最小化状态打开\n"
        "menu_minimized=" << (config.menu_minimized ? "true" : "false") << "\n\n"
        "; 菜单透明度，范围 0.20 - 1.00\n"
        "menu_opacity=" << config.menu_opacity << "\n\n"
        "[CharacterFlags]\n"
        "; 无限 FP / 不耗蓝\n"
        "infinite_fp=" << (config.infinite_fp ? "true" : "false") << "\n\n"
        "; 道具不消耗\n"
        "infinite_items=" << (config.infinite_items ? "true" : "false") << "\n\n"
        "; 不耗精力\n"
        "no_stamina_consumption=" << (config.no_stamina_consumption ? "true" : "false") << "\n\n"
        "[WorldUnlocks]\n"
        "; 以下是一类一次性动作，默认不会因为菜单点击而自动回写\n"
        "unlock_all_maps=" << (config.unlock_all_maps ? "true" : "false") << "\n"
        "unlock_all_cookbooks=" << (config.unlock_all_cookbooks ? "true" : "false") << "\n"
        "unlock_all_whetblades=" << (config.unlock_all_whetblades ? "true" : "false") << "\n"
        "unlock_all_graces=" << (config.unlock_all_graces ? "true" : "false") << "\n"
        "unlock_all_summoning_pools=" << (config.unlock_all_summoning_pools ? "true" : "false") << "\n"
        "unlock_all_colosseums=" << (config.unlock_all_colosseums ? "true" : "false") << "\n\n"
        "[ParamPatches]\n"
        "faster_respawn=" << (config.faster_respawn ? "true" : "false") << "\n"
        "warp_out_of_uncleared_minidungeons=" << (config.warp_out_of_uncleared_minidungeons ? "true" : "false") << "\n"
        "free_purchase=" << (config.free_purchase ? "true" : "false") << "\n"
        "no_crafting_material_cost=" << (config.no_crafting_material_cost ? "true" : "false") << "\n"
        "no_upgrade_material_cost=" << (config.no_upgrade_material_cost ? "true" : "false") << "\n"
        "no_magic_requirements=" << (config.no_magic_requirements ? "true" : "false") << "\n"
        "all_magic_one_slot=" << (config.all_magic_one_slot ? "true" : "false") << "\n"
        "weightless_equipment=" << (config.weightless_equipment ? "true" : "false") << "\n"
        "mount_anywhere=" << (config.mount_anywhere ? "true" : "false") << "\n"
        "spirit_ashes_anywhere=" << (config.spirit_ashes_anywhere ? "true" : "false") << "\n"
        "item_discovery_multiplier=" << std::clamp(config.item_discovery_multiplier, 1, 100) << "\n"
        "permanent_lantern=" << (config.permanent_lantern ? "true" : "false") << "\n"
        "invisible_helmets=" << (config.invisible_helmets ? "true" : "false") << "\n\n"
        "[GameActions]\n"
        "start_next_cycle=" << (config.start_next_cycle ? "true" : "false") << "\n";

    return output.good();
}

}  // namespace ERD::Main
