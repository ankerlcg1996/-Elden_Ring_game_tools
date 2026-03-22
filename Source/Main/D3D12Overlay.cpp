#include "D3D12Overlay.hpp"

#include "../Common.hpp"
#include "../Features/InspectorTools.hpp"
#include "../Features/MassItemGibData.hpp"
#include "../Features/NpcDebug.hpp"
#include "../Features/PostureBars.hpp"
#include "FeatureStatus.hpp"
#include "Logger.hpp"

#include "../ThirdParty/ImGui/imgui_impl_dx12.h"
#include "../ThirdParty/ImGui/imgui_impl_win32.h"
#include "../ThirdParty/MinHook/MinHook.h"

#include <d3d12.h>
#include <dxgi1_4.h>
#include <algorithm>
#include <cctype>
#include <cmath>
#include <cstdio>
#include <fstream>
#include <limits>
#include <sstream>

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

namespace ERD::Main {
namespace {

inline constexpr std::uint16_t kExecuteCommandListsIndex = 54;
inline constexpr std::uint16_t kPresentIndex = 140;
inline constexpr std::uint16_t kResizeBuffersIndex = 145;
float g_MenuOpacity = 0.88f;
bool g_MenuMinimized = false;
bool g_MenuCollapseDirty = true;
RECT g_MenuRect{0, 0, 0, 0};
bool g_MenuRectValid = false;
std::unordered_map<std::uint64_t, int> g_PreviousBarHpByHandle;
std::unordered_map<std::uint64_t, int> g_FallbackDamageByHandle;
std::unordered_map<std::uint64_t, ULONGLONG> g_FallbackDamageTicksByHandle;

struct NpcHpTrackerState {
    std::uint64_t handle = 0;
    int previous_hp = -1;
};

struct DamageHistoryEntry {
    std::uint64_t handle = 0;
    int damage = 0;
    ULONGLONG tick = 0;
};

NpcHpTrackerState g_TargetedDamageTracker{};
NpcHpTrackerState g_LastHitDamageTracker{};
std::vector<DamageHistoryEntry> g_DamageHistory;

enum class CatalogKind {
    All,
    Weapon,
    Protector,
    Accessory,
    Goods,
};

struct CatalogItemEntry {
    std::int32_t id = 0;
    std::int32_t raw_id = 0;
    CatalogKind kind = CatalogKind::All;
    std::string name;
};

std::vector<CatalogItemEntry> g_ItemCatalog;
bool g_ItemCatalogLoaded = false;
std::string g_ItemCatalogError;
std::array<char, 128> g_CustomItemSearchBuffer{};
std::array<char, 128> g_EquipItemSearchBuffer{};
std::array<char, 128> g_WeaponInventorySearchBuffer{};
std::array<char, 128> g_ProtectorInventorySearchBuffer{};
std::array<char, 128> g_AccessoryInventorySearchBuffer{};
std::array<char, 128> g_GoodsInventorySearchBuffer{};
int g_CustomItemManualKind = static_cast<int>(CatalogKind::Goods);
fs::path g_ItemCatalogLoadedPath;
fs::file_time_type g_ItemCatalogLoadedWriteTime{};
bool g_MenuToggleRequested = false;

void LogProtectedException(const char* scope, const char* detail) {
    Logger::Instance().Error((std::string(scope) + " failed: " + detail).c_str());
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

HWND FindGameWindow() {
    HWND window = FindWindowW(nullptr, L"ELDEN RING");
    if (window == nullptr) {
        window = FindWindowW(nullptr, L"ELDEN RING");
    }
    if (window == nullptr) {
        window = GetForegroundWindow();
    }
    return window;
}

bool IsMouseMessage(UINT message) {
    switch (message) {
    case WM_MOUSEMOVE:
    case WM_LBUTTONDOWN:
    case WM_LBUTTONUP:
    case WM_LBUTTONDBLCLK:
    case WM_RBUTTONDOWN:
    case WM_RBUTTONUP:
    case WM_RBUTTONDBLCLK:
    case WM_MBUTTONDOWN:
    case WM_MBUTTONUP:
    case WM_MBUTTONDBLCLK:
    case WM_MOUSEWHEEL:
    case WM_MOUSEHWHEEL:
    case WM_XBUTTONDOWN:
    case WM_XBUTTONUP:
    case WM_XBUTTONDBLCLK:
        return true;
    default:
        return false;
    }
}

bool IsKeyboardMessage(UINT message) {
    switch (message) {
    case WM_KEYDOWN:
    case WM_KEYUP:
    case WM_SYSKEYDOWN:
    case WM_SYSKEYUP:
    case WM_CHAR:
        return true;
    default:
        return false;
    }
}

bool GetMouseClientPosition(HWND hwnd, UINT message, LPARAM l_param, POINT& point) {
    if (hwnd == nullptr) {
        return false;
    }

    switch (message) {
    case WM_MOUSEMOVE:
    case WM_LBUTTONDOWN:
    case WM_LBUTTONUP:
    case WM_LBUTTONDBLCLK:
    case WM_RBUTTONDOWN:
    case WM_RBUTTONUP:
    case WM_RBUTTONDBLCLK:
    case WM_MBUTTONDOWN:
    case WM_MBUTTONUP:
    case WM_MBUTTONDBLCLK:
    case WM_XBUTTONDOWN:
    case WM_XBUTTONUP:
    case WM_XBUTTONDBLCLK:
        point.x = static_cast<LONG>(static_cast<short>(LOWORD(l_param)));
        point.y = static_cast<LONG>(static_cast<short>(HIWORD(l_param)));
        return true;
    case WM_MOUSEWHEEL:
    case WM_MOUSEHWHEEL:
        point.x = static_cast<LONG>(static_cast<short>(LOWORD(l_param)));
        point.y = static_cast<LONG>(static_cast<short>(HIWORD(l_param)));
        return ScreenToClient(hwnd, &point) != FALSE;
    default:
        return false;
    }
}

bool IsPointInsideMenuRect(const POINT& point) {
    if (!g_MenuRectValid) {
        return false;
    }

    return point.x >= g_MenuRect.left && point.x < g_MenuRect.right &&
           point.y >= g_MenuRect.top && point.y < g_MenuRect.bottom;
}

bool IsToggleHotkeyPressed() {
    static bool was_down = false;
    const bool f1_down = (GetAsyncKeyState(VK_F1) & 0x8000) != 0;
    const bool down = f1_down;
    const bool pressed = down && !was_down;
    was_down = down;
    return pressed;
}

bool ConsumeMenuToggleRequest() {
    if (!g_MenuToggleRequested) {
        return false;
    }
    g_MenuToggleRequested = false;
    return true;
}

void DrawBooleanToggle(const char* label, const char* description, std::atomic_bool& value) {
    bool enabled = value.load();
    if (ImGui::Checkbox(label, &enabled)) {
        value.store(enabled);
    }
    ImGui::TextDisabled("%s", description);
    ImGui::Spacing();
}

void DrawIntSlider(const char* label,
                   const char* description,
                   std::atomic_int& value,
                   int min_value,
                   int max_value,
                   const char* format = "%d") {
    int current = std::clamp(value.load(), min_value, max_value);
    if (ImGui::SliderInt(label, &current, min_value, max_value, format)) {
        value.store(current);
    }
    ImGui::TextDisabled("%s", description);
    ImGui::Spacing();
}

void DrawIntInput(const char* label,
                  const char* description,
                  std::atomic_int& value,
                  int min_value,
                  int max_value,
                  int step = 1,
                  int step_fast = 10) {
    int current = std::clamp(value.load(), min_value, max_value);
    if (ImGui::InputInt(label, &current, step, step_fast)) {
        value.store(std::clamp(current, min_value, max_value));
    }
    ImGui::TextDisabled("%s", description);
    ImGui::Spacing();
}

void DrawFloatInput(const char* label,
                    const char* description,
                    std::atomic<float>& value,
                    float min_value,
                    float max_value,
                    float step,
                    const char* format = "%.2f") {
    float current = std::clamp(value.load(), min_value, max_value);
    if (ImGui::InputFloat(label, &current, step, step * 10.0f, format)) {
        value.store(std::clamp(current, min_value, max_value));
    }
    ImGui::TextDisabled("%s", description);
    ImGui::Spacing();
}

void DrawOneShotButton(const char* label,
                       const char* description,
                       std::atomic_bool& value,
                       const char* pending_text,
                       const char* applied_text) {
    const bool applied = value.load();
    if (applied) {
        ImGui::PushStyleVar(ImGuiStyleVar_Alpha, ImGui::GetStyle().Alpha * 0.6f);
    }

    if (ImGui::Button(label, ImVec2(-1.0f, 0.0f)) && !applied) {
        value.store(true);
    }

    if (applied) {
        ImGui::PopStyleVar();
    }

    ImGui::TextDisabled("%s", description);
    ImGui::TextColored(
        applied ? ImVec4(0.25f, 0.9f, 0.45f, 1.0f) : ImVec4(0.95f, 0.8f, 0.2f, 1.0f),
        "%s",
        applied ? applied_text : pending_text
    );
    ImGui::Spacing();
}

void DrawMassItemGibButton(const Features::MassItemGibPackage& package) {
    const bool dlc_only = package.requires_dlc;
    std::string button_label = package.label;
    if (dlc_only) {
        button_label += " [DLC]";
    }

    if (ImGui::Button(button_label.c_str(), ImVec2(-1.0f, 0.0f))) {
        g_FeatureStatus.mass_item_gib_request_id = package.id;
    }

    ImGui::TextDisabled("%s", package.description);
    ImGui::TextDisabled("Package size: %llu entries", static_cast<unsigned long long>(package.item_count));
    ImGui::Spacing();
}

const char* CharacterTypeLabel(int value) {
    switch (value) {
    case 0: return "Host";
    case 1: return "White Phantom";
    case 2: return "Dark Spirit?";
    case 3: return "Ghost";
    case 4: return "Ghost";
    case 5: return "NPC";
    case 16: return "Dark Spirit";
    case 17: return "Blue";
    default: return "Unknown";
    }
}

const char* TeamTypeLabel(int value) {
    switch (value) {
    case 0: return "None";
    case 1: return "Host";
    case 2: return "Phantom";
    case 6: return "Non-Hostile NPC";
    case 7: return "Hostile NPC";
    case 16: return "Dark Spirit";
    case 47: return "Spirit Summon";
    default: return "Unknown";
    }
}

void DrawNpcInfoSection(const char* title,
                        const char* empty_text,
                        std::atomic_bool& valid,
                        std::atomic<uintptr_t>& address,
                        std::atomic<uintptr_t>& handle,
                        std::atomic_int& npc_id,
                        std::atomic_int& character_type,
                        std::atomic_int& team_type,
                        std::atomic_int& hp,
                        std::atomic_int& max_hp,
                        std::atomic<float>& x,
                        std::atomic<float>& y,
                        std::atomic<float>& z,
                        std::atomic_bool& self_to_npc_requested,
                        std::atomic_bool& npc_to_self_requested) {
    ImGui::PushID(title);
    ImGui::TextUnformatted(title);
    ImGui::Separator();

    if (!valid.load()) {
        ImGui::TextDisabled("%s", empty_text);
        ImGui::Spacing();
        ImGui::PopID();
        return;
    }

    const uintptr_t current_address = address.load();
    const uintptr_t current_handle = handle.load();
    const int current_npc_id = npc_id.load();
    const int current_character_type = character_type.load();
    const int current_team_type = team_type.load();
    const int current_hp = hp.load();
    const int current_max_hp = max_hp.load();
    const float current_x = x.load();
    const float current_y = y.load();
    const float current_z = z.load();

    ImGui::Text("NPC ID: %d", current_npc_id);
    ImGui::Text("Character Type: %s (%d)", CharacterTypeLabel(current_character_type), current_character_type);
    ImGui::Text("Team Type: %s (%d)", TeamTypeLabel(current_team_type), current_team_type);
    ImGui::Text("HP: %d / %d", current_hp, current_max_hp);
    ImGui::Text("World Pos: X %.2f | Y %.2f | Z %.2f", current_x, current_y, current_z);
    ImGui::Text("Base: %p", reinterpret_cast<void*>(current_address));
    ImGui::Text("Handle: %p", reinterpret_cast<void*>(current_handle));

    if (ImGui::Button("传送自己到该 NPC", ImVec2(-1.0f, 0.0f))) {
        self_to_npc_requested = true;
    }
    if (ImGui::Button("传送该 NPC 到自己", ImVec2(-1.0f, 0.0f))) {
        npc_to_self_requested = true;
    }
    ImGui::Spacing();
    ImGui::PopID();
}

void DrawCoordReadout(const char* label, float x, float y, float z) {
    ImGui::Text("%s", label);
    ImGui::TextDisabled("X %.3f | Y %.3f | Z %.3f", x, y, z);
}

const char* const kEquipSlotLabels[] = {
    "0 主手左一",
    "1 主手右一",
    "2 主手左二",
    "3 主手右二",
    "4 主手左三",
    "5 主手右三",
    "6 箭矢一",
    "7 弩箭一",
    "8 箭矢二",
    "9 弩箭二",
    "10 箭矢三",
    "11 弩箭三",
    "12 头部",
    "13 胸甲",
    "14 手甲",
    "15 腿甲",
    "16 头发",
    "17 护符一",
    "18 护符二",
    "19 护符三",
    "20 护符四",
    "21 护符五",
    "22 快捷栏1",
    "23 快捷栏2",
    "24 快捷栏3",
    "25 快捷栏4",
    "26 快捷栏5",
    "27 快捷栏6",
    "28 快捷栏7",
    "29 快捷栏8",
    "30 快捷栏9",
    "31 快捷栏10",
    "32 随身包1",
    "33 随身包2",
    "34 随身包3",
    "35 随身包4",
    "36 随身包5",
    "37 随身包6",
    "38 大卢恩",
};

void DrawReadOnlyInt(const char* label, int value, const char* description) {
    ImGui::Text("%s: %d", label, value);
    ImGui::TextDisabled("%s", description);
    ImGui::Spacing();
}

std::string ToLowerCopy(std::string value) {
    std::transform(value.begin(), value.end(), value.begin(), [](unsigned char ch) {
        return static_cast<char>(std::tolower(ch));
    });
    return value;
}

fs::path GetChineseItemCatalogPath() {
    wchar_t buffer[MAX_PATH]{};
    const DWORD length = GetModuleFileNameW(g_Module, buffer, MAX_PATH);
    if (length == 0) {
        return {};
    }

    const fs::path dll_path(std::wstring(buffer, buffer + length));
    return dll_path.parent_path() / L"Resources" / L"Dropdowns" / L"ItemCatalog.zh.txt";
}

fs::path GetLegacyItemDropdownCatalogPath() {
    wchar_t buffer[MAX_PATH]{};
    const DWORD length = GetModuleFileNameW(g_Module, buffer, MAX_PATH);
    if (length == 0) {
        return {};
    }

    const fs::path dll_path(std::wstring(buffer, buffer + length));
    return dll_path.parent_path() / L"Resources" / L"Dropdowns" / L"ItemDropdownDLC.xml";
}

std::string TrimCopy(std::string value) {
    auto not_space = [](unsigned char ch) { return !std::isspace(ch); };
    value.erase(value.begin(), std::find_if(value.begin(), value.end(), not_space));
    value.erase(std::find_if(value.rbegin(), value.rend(), not_space).base(), value.end());
    return value;
}

CatalogKind ParseCatalogKindToken(const std::string& token) {
    const std::string lowered = ToLowerCopy(token);
    if (lowered == "weapon") {
        return CatalogKind::Weapon;
    }
    if (lowered == "protector" || lowered == "armor") {
        return CatalogKind::Protector;
    }
    if (lowered == "accessory" || lowered == "talisman" || lowered == "tailsman") {
        return CatalogKind::Accessory;
    }
    if (lowered == "goods" || lowered == "item") {
        return CatalogKind::Goods;
    }
    return CatalogKind::All;
}

std::uint32_t CatalogPrefixForKind(CatalogKind kind) {
    switch (kind) {
    case CatalogKind::Weapon:
        return 0x00000000u;
    case CatalogKind::Protector:
        return 0x10000000u;
    case CatalogKind::Accessory:
        return 0x20000000u;
    case CatalogKind::Goods:
        return 0x40000000u;
    default:
        return 0xF0000000u;
    }
}

CatalogKind DetectCatalogKindFromItemId(std::int32_t item_id) {
    const std::uint32_t prefix = static_cast<std::uint32_t>(item_id) & 0xF0000000u;
    if (prefix == 0x00000000u) {
        return CatalogKind::Weapon;
    }
    if (prefix == 0x10000000u) {
        return CatalogKind::Protector;
    }
    if (prefix == 0x20000000u) {
        return CatalogKind::Accessory;
    }
    if (prefix == 0x40000000u) {
        return CatalogKind::Goods;
    }
    return CatalogKind::All;
}

std::int32_t DecodeCatalogRawId(std::int32_t item_id) {
    const CatalogKind kind = DetectCatalogKindFromItemId(item_id);
    const std::uint32_t prefix = CatalogPrefixForKind(kind);
    if (prefix == 0xF0000000u) {
        return item_id;
    }
    return static_cast<std::int32_t>(static_cast<std::uint32_t>(item_id) - prefix);
}

std::int32_t EncodeCatalogId(std::int32_t raw_id, CatalogKind kind) {
    if (raw_id < 0) {
        return raw_id;
    }
    const std::uint32_t prefix = CatalogPrefixForKind(kind);
    if (prefix == 0xF0000000u) {
        return raw_id;
    }
    return static_cast<std::int32_t>(prefix + static_cast<std::uint32_t>(raw_id));
}

void LoadChineseTextCatalog(const fs::path& catalog_path) {
    std::ifstream input(catalog_path, std::ios::binary);
    if (!input.is_open()) {
        g_ItemCatalogError = "未找到中文物品表: " + catalog_path.string();
        return;
    }

    std::string line;
    bool first_line = true;
    while (std::getline(input, line)) {
        if (!line.empty() && line.back() == '\r') {
            line.pop_back();
        }
        if (first_line && line.size() >= 3 &&
            static_cast<unsigned char>(line[0]) == 0xEF &&
            static_cast<unsigned char>(line[1]) == 0xBB &&
            static_cast<unsigned char>(line[2]) == 0xBF) {
            line.erase(0, 3);
        }
        first_line = false;

        line = TrimCopy(line);
        if (line.empty() || line[0] == '#') {
            continue;
        }

        const std::size_t first_separator = line.find('|');
        const std::size_t second_separator = first_separator == std::string::npos ? std::string::npos : line.find('|', first_separator + 1);
        if (first_separator == std::string::npos || second_separator == std::string::npos) {
            continue;
        }

        const CatalogKind kind = ParseCatalogKindToken(TrimCopy(line.substr(0, first_separator)));
        const std::string raw_id_text = TrimCopy(line.substr(first_separator + 1, second_separator - first_separator - 1));
        const std::string name = TrimCopy(line.substr(second_separator + 1));
        if (kind == CatalogKind::All || raw_id_text.empty() || name.empty()) {
            continue;
        }

        try {
            const std::int32_t raw_id = static_cast<std::int32_t>(std::stoll(raw_id_text, nullptr, 0));
            g_ItemCatalog.push_back(CatalogItemEntry{
                EncodeCatalogId(raw_id, kind),
                raw_id,
                kind,
                name,
            });
        } catch (...) {
        }
    }

    if (g_ItemCatalog.empty()) {
        g_ItemCatalogError = "中文物品表解析后没有任何条目。";
    }
}

void LoadLegacyXmlCatalog(const fs::path& catalog_path) {
    std::ifstream input(catalog_path, std::ios::binary);
    if (!input.is_open()) {
        g_ItemCatalogError = "未找到物品枚举文件: " + catalog_path.string();
        return;
    }

    std::string content((std::istreambuf_iterator<char>(input)), std::istreambuf_iterator<char>());
    const std::string start_tag = "<DropDownList";
    const std::string end_tag = "</DropDownList>";
    const std::size_t start = content.find(start_tag);
    if (start == std::string::npos) {
        g_ItemCatalogError = "物品枚举文件格式无效。";
        return;
    }

    const std::size_t start_close = content.find('>', start);
    const std::size_t end = content.find(end_tag, start_close == std::string::npos ? start : start_close + 1);
    if (start_close == std::string::npos || end == std::string::npos || end <= start_close) {
        g_ItemCatalogError = "物品枚举文件内容为空。";
        return;
    }

    const std::string inner = content.substr(start_close + 1, end - start_close - 1);
    std::stringstream stream(inner);
    std::string line;
    while (std::getline(stream, line)) {
        if (!line.empty() && line.back() == '\r') {
            line.pop_back();
        }

        const std::size_t separator = line.find(':');
        if (separator == std::string::npos) {
            continue;
        }

        std::string id_text = line.substr(0, separator);
        std::string name = line.substr(separator + 1);
        if (id_text.empty() || name.empty()) {
            continue;
        }

        try {
            const std::uint32_t encoded_id = static_cast<std::uint32_t>(std::stoul(id_text, nullptr, 16));
            const std::int32_t item_id = static_cast<std::int32_t>(encoded_id);
            g_ItemCatalog.push_back(CatalogItemEntry{
                item_id,
                DecodeCatalogRawId(item_id),
                DetectCatalogKindFromItemId(item_id),
                name,
            });
        } catch (...) {
        }
    }

    if (g_ItemCatalog.empty()) {
        g_ItemCatalogError = "物品枚举文件解析后没有任何条目。";
    }
}

void EnsureItemCatalogLoaded() {
    const fs::path chinese_catalog_path = GetChineseItemCatalogPath();
    const fs::path legacy_catalog_path = GetLegacyItemDropdownCatalogPath();
    fs::path active_catalog_path = fs::exists(chinese_catalog_path) ? chinese_catalog_path : legacy_catalog_path;
    fs::file_time_type active_write_time{};
    if (!active_catalog_path.empty() && fs::exists(active_catalog_path)) {
        active_write_time = fs::last_write_time(active_catalog_path);
    }

    if (g_ItemCatalogLoaded &&
        g_ItemCatalogLoadedPath == active_catalog_path &&
        (!active_catalog_path.empty() && fs::exists(active_catalog_path)
             ? g_ItemCatalogLoadedWriteTime == active_write_time
             : g_ItemCatalogLoadedPath.empty())) {
        return;
    }

    g_ItemCatalogLoaded = true;
    g_ItemCatalog.clear();
    g_ItemCatalogError.clear();

    if (fs::exists(chinese_catalog_path)) {
        LoadChineseTextCatalog(chinese_catalog_path);
        active_catalog_path = chinese_catalog_path;
    }

    if (g_ItemCatalog.empty()) {
        LoadLegacyXmlCatalog(legacy_catalog_path);
        active_catalog_path = legacy_catalog_path;
    }

    g_ItemCatalogLoadedPath = active_catalog_path;
    if (!active_catalog_path.empty() && fs::exists(active_catalog_path)) {
        g_ItemCatalogLoadedWriteTime = fs::last_write_time(active_catalog_path);
    } else {
        g_ItemCatalogLoadedWriteTime = fs::file_time_type{};
    }
}

CatalogKind GetCatalogKindForEquipSlot(int slot) {
    if (slot >= 0 && slot <= 11) {
        return CatalogKind::Weapon;
    }
    if (slot >= 12 && slot <= 16) {
        return CatalogKind::Protector;
    }
    if (slot >= 17 && slot <= 21) {
        return CatalogKind::Accessory;
    }
    return CatalogKind::Goods;
}

bool CatalogItemMatchesKind(const CatalogItemEntry& item, CatalogKind kind) {
    if (kind == CatalogKind::All) {
        return true;
    }
    return item.kind == kind;
}

const char* CatalogKindLabel(CatalogKind kind) {
    switch (kind) {
    case CatalogKind::All:
        return "全部物品";
    case CatalogKind::Weapon:
        return "武器";
    case CatalogKind::Protector:
        return "防具";
    case CatalogKind::Accessory:
        return "护符";
    case CatalogKind::Goods:
        return "道具";
    default:
        return "未知分类";
    }
}

std::string BuildCatalogItemLabel(const CatalogItemEntry& item) {
    return std::to_string(item.raw_id) + " | " + item.name;
}

std::string BuildCatalogPreview(std::int32_t id) {
    EnsureItemCatalogLoaded();
    for (const CatalogItemEntry& item : g_ItemCatalog) {
        if (item.id == id) {
            return BuildCatalogItemLabel(item);
        }
    }

    return std::to_string(DecodeCatalogRawId(id));
}

void DrawCatalogCombo(const char* label,
                      const char* description,
                      std::atomic_int& target_id,
                      CatalogKind kind,
                      std::array<char, 128>& search_buffer) {
    EnsureItemCatalogLoaded();
    const std::string preview = BuildCatalogPreview(target_id.load());
    const std::string search = ToLowerCopy(search_buffer.data());

    int kind_total = 0;
    std::vector<int> filtered_indices;
    filtered_indices.reserve(g_ItemCatalog.size());
    for (std::size_t index = 0; index < g_ItemCatalog.size(); ++index) {
        const CatalogItemEntry& item = g_ItemCatalog[index];
        if (!CatalogItemMatchesKind(item, kind)) {
            continue;
        }

        ++kind_total;

        if (!search.empty()) {
            char id_buffer[32]{};
            sprintf_s(id_buffer, "%d", item.raw_id);
            char encoded_buffer[32]{};
            sprintf_s(encoded_buffer, "%08X", static_cast<unsigned>(item.id));
            const std::string item_name = ToLowerCopy(item.name);
            const std::string item_id = ToLowerCopy(id_buffer);
            const std::string encoded_id = ToLowerCopy(encoded_buffer);
            if (item_name.find(search) == std::string::npos &&
                item_id.find(search) == std::string::npos &&
                encoded_id.find(search) == std::string::npos) {
                continue;
            }
        }

        filtered_indices.push_back(static_cast<int>(index));
    }

    if (ImGui::BeginCombo(label, preview.c_str())) {
        ImGui::InputTextWithHint("##search", "搜索名称或 ID", search_buffer.data(), search_buffer.size());
        ImGui::SameLine();
        if (ImGui::Button("清空搜索")) {
            search_buffer.fill('\0');
        }
        ImGui::Separator();
        ImGui::TextDisabled(
            "分类：%s | 当前匹配：%d / %d",
            CatalogKindLabel(kind),
            static_cast<int>(filtered_indices.size()),
            kind_total
        );
        ImGui::Spacing();

        if (filtered_indices.empty()) {
            ImGui::TextDisabled("%s", g_ItemCatalogError.empty() ? "没有匹配的物品。" : g_ItemCatalogError.c_str());
        } else {
            const std::string child_id = std::string("##catalog_list_") + label;
            if (ImGui::BeginChild(child_id.c_str(), ImVec2(0.0f, ImGui::GetTextLineHeightWithSpacing() * 14.0f), true)) {
                ImGuiListClipper clipper;
                clipper.Begin(static_cast<int>(filtered_indices.size()));
                while (clipper.Step()) {
                    for (int row = clipper.DisplayStart; row < clipper.DisplayEnd; ++row) {
                        const CatalogItemEntry& item = g_ItemCatalog[filtered_indices[static_cast<std::size_t>(row)]];
                        const std::string option_label = BuildCatalogItemLabel(item);
                        const bool selected = target_id.load() == item.id;
                        if (ImGui::Selectable(option_label.c_str(), selected)) {
                            target_id = item.id;
                        }
                        if (selected) {
                            ImGui::SetItemDefaultFocus();
                        }
                    }
                }
            }
            ImGui::EndChild();
        }

        ImGui::EndCombo();
    }

    ImGui::TextDisabled("%s", description);
    ImGui::Spacing();
}

void DrawCatalogRawIdInput(const char* label,
                           const char* description,
                           std::atomic_int& target_id,
                           CatalogKind kind) {
    int raw_id = DecodeCatalogRawId(target_id.load());
    if (ImGui::InputInt(label, &raw_id, 1, 100)) {
        target_id = EncodeCatalogId(raw_id, kind);
    }
    ImGui::TextDisabled("%s", description);
}

const CatalogItemEntry* FindCatalogItem(std::int32_t item_id) {
    EnsureItemCatalogLoaded();
    const auto found = std::find_if(g_ItemCatalog.begin(), g_ItemCatalog.end(), [item_id](const CatalogItemEntry& item) {
        return item.id == item_id;
    });
    return found == g_ItemCatalog.end() ? nullptr : &(*found);
}

std::string BuildItemName(std::int32_t item_id) {
    if (const CatalogItemEntry* item = FindCatalogItem(item_id)) {
        return item->name;
    }
    return std::to_string(DecodeCatalogRawId(item_id));
}

bool MatchesInventorySearch(const Features::InventoryEntrySnapshot& entry, const std::string& search) {
    if (search.empty()) {
        return true;
    }

    const std::string name = ToLowerCopy(BuildItemName(entry.item_id));
    const std::string raw_id = std::to_string(DecodeCatalogRawId(entry.item_id));
    const std::string full_id = std::to_string(entry.item_id);
    return name.find(search) != std::string::npos ||
           raw_id.find(search) != std::string::npos ||
           full_id.find(search) != std::string::npos;
}

void DrawCompactItemLine(const char* label, std::int32_t item_id) {
    ImGui::BulletText("%s: %s (%d)", label, BuildItemName(item_id).c_str(), DecodeCatalogRawId(item_id));
}

void DrawInventoryList(const char* child_id,
                       const std::vector<Features::InventoryEntrySnapshot>& entries,
                       std::array<char, 128>& search_buffer) {
    const std::string search = ToLowerCopy(search_buffer.data());
    ImGui::InputTextWithHint("##inventory_search", "搜索名称或 ID", search_buffer.data(), search_buffer.size());
    ImGui::SameLine();
    if (ImGui::Button("清空")) {
        search_buffer.fill('\0');
    }
    ImGui::TextDisabled("条目数: %d", static_cast<int>(entries.size()));

    if (ImGui::BeginChild(child_id, ImVec2(0.0f, ImGui::GetTextLineHeightWithSpacing() * 14.0f), true)) {
        int shown = 0;
        for (const Features::InventoryEntrySnapshot& entry : entries) {
            if (!MatchesInventorySearch(entry, search)) {
                continue;
            }

            ++shown;
            const std::string name = BuildItemName(entry.item_id);
            ImGui::PushID(entry.item_id ^ (entry.repository ? 0x40000000 : 0));
            ImGui::Text("%s", name.c_str());
            ImGui::SameLine();
            ImGui::TextDisabled("[%d]", DecodeCatalogRawId(entry.item_id));
            ImGui::SameLine();
            ImGui::TextColored(
                entry.repository ? ImVec4(0.85f, 0.78f, 0.48f, 1.0f) : ImVec4(0.60f, 0.85f, 0.60f, 1.0f),
                "%s x%d",
                entry.repository ? "箱子" : "持有",
                entry.quantity
            );
            ImGui::PopID();
        }

        if (shown == 0) {
            ImGui::TextDisabled("没有匹配的条目。");
        }
    }
    ImGui::EndChild();
}

void DrawSessionPlayerCard(const char* title, const Features::SessionPlayerSnapshot& snapshot) {
    ImGui::PushID(title);
    ImGui::Separator();
    ImGui::TextUnformatted(title);
    if (!snapshot.valid) {
        ImGui::TextDisabled("当前没有可用数据。");
        ImGui::PopID();
        return;
    }

    ImGui::Text("名称: %s", snapshot.name.empty() ? "未知" : snapshot.name.c_str());
    ImGui::Text("HP: %d / %d", snapshot.hp, snapshot.max_hp);
    ImGui::Text("等级: %d | 强化级别: %d", snapshot.level, snapshot.reinforce_level);
    ImGui::Text("角色类型: %s (%d)", CharacterTypeLabel(snapshot.character_type), snapshot.character_type);
    ImGui::Text("队伍类型: %s (%d)", TeamTypeLabel(snapshot.team_type), snapshot.team_type);
    ImGui::PopID();
}

void DrawPartyMemberCard(int index, const Features::PartyMemberSlotSnapshot& snapshot) {
    ImGui::PushID(index);
    ImGui::Separator();
    ImGui::Text("队伍槽位 %d", index + 1);
    ImGui::Text("Handle: 0x%llX", static_cast<unsigned long long>(snapshot.handle));
    ImGui::Text("Type: %d | State: %d", snapshot.type, snapshot.state);
    ImGui::Text("InitFlag: %d | EndFlag: %d", snapshot.init_flag, snapshot.end_flag);
    ImGui::PopID();
}

struct GameViewportTransform {
    float scale = 1.0f;
    ImVec2 offset{};
};

GameViewportTransform BuildGameViewportTransform() {
    ImGuiViewport* viewport = ImGui::GetMainViewport();
    const float viewport_scale_x = viewport->Size.x / 1920.0f;
    const float viewport_scale_y = viewport->Size.y / 1080.0f;
    const float scale = std::max(0.01f, std::min(viewport_scale_x, viewport_scale_y));
    const float content_width = std::ceilf(1920.0f * scale);
    const float content_height = std::ceilf(1080.0f * scale);

    GameViewportTransform transform{};
    transform.scale = scale;
    transform.offset.x = viewport->Pos.x + (viewport->Size.x - content_width) * 0.5f;
    transform.offset.y = viewport->Pos.y + (viewport->Size.y - content_height) * 0.5f;
    return transform;
}

ImVec2 ToViewportPosition(const GameViewportTransform& transform, float x, float y) {
    return ImVec2(transform.offset.x + x * transform.scale, transform.offset.y + y * transform.scale);
}

void DrawCenteredText(ImDrawList* draw_list, const ImVec2& center_top, ImU32 color, const char* text) {
    if (draw_list == nullptr || text == nullptr || *text == '\0') {
        return;
    }

    const ImVec2 text_size = ImGui::CalcTextSize(text);
    draw_list->AddText(ImVec2(center_top.x - text_size.x * 0.5f, center_top.y), color, text);
}

ImU32 GetPostureBarOrange() {
    return IM_COL32(230, 126, 34, 235);
}

ImU32 GetSoulBorderOuter() {
    return IM_COL32(134, 92, 42, 240);
}

ImU32 GetSoulBorderInner() {
    return IM_COL32(52, 38, 20, 230);
}

ImU32 GetSoulPanelBackground() {
    return IM_COL32(16, 12, 8, 215);
}

void DrawSimplePostureBar(ImDrawList* draw_list,
                          const ImVec2& center_top,
                          const ImVec2& size,
                          float fill_ratio,
                          ImU32 fill_color) {
    if (draw_list == nullptr || size.x <= 1.0f || size.y <= 1.0f) {
        return;
    }

    const float clamped_ratio = std::clamp(fill_ratio, 0.0f, 1.0f);
    const ImVec2 min(center_top.x - size.x * 0.5f, center_top.y);
    const ImVec2 max(min.x + size.x, min.y + size.y);
    const float rounding = std::max(2.0f, size.y * 0.25f);

    draw_list->AddRectFilled(min, max, IM_COL32(12, 9, 7, 210), rounding);
    if (clamped_ratio > 0.0f) {
        draw_list->AddRectFilled(
            min,
            ImVec2(min.x + size.x * clamped_ratio, max.y),
            fill_color,
            rounding
        );
    }
    draw_list->AddRect(min, max, GetSoulBorderOuter(), rounding, 0, 1.8f);
    draw_list->AddRect(
        ImVec2(min.x + 1.0f, min.y + 1.0f),
        ImVec2(max.x - 1.0f, max.y - 1.0f),
        GetSoulBorderInner(),
        std::max(1.0f, rounding - 1.0f),
        0,
        1.0f
    );
    draw_list->AddLine(
        ImVec2(min.x + 2.0f, min.y + 1.0f),
        ImVec2(max.x - 2.0f, min.y + 1.0f),
        IM_COL32(255, 214, 166, 64),
        1.0f
    );
}

void DrawTargetedNpcSummary(ImDrawList* draw_list, const ImVec2& center_top) {
    const bool targeted_valid = g_FeatureStatus.targeted_npc_valid.load();
    const bool last_hit_valid = g_FeatureStatus.last_hit_npc_valid.load();
    const bool use_targeted = targeted_valid || !last_hit_valid;
    const std::string npc_name = use_targeted ? Features::GetTargetedNpcNameUtf8() : Features::GetLastHitNpcNameUtf8();
    const int hp = use_targeted ? g_FeatureStatus.targeted_npc_hp.load() : g_FeatureStatus.last_hit_npc_hp.load();
    const int max_hp =
        use_targeted ? g_FeatureStatus.targeted_npc_max_hp.load() : g_FeatureStatus.last_hit_npc_max_hp.load();

    float y = center_top.y;
    if (!npc_name.empty()) {
        DrawCenteredText(draw_list, ImVec2(center_top.x, y), IM_COL32(255, 255, 255, 245), npc_name.c_str());
        y += ImGui::GetTextLineHeight() + 2.0f;
    } else if (use_targeted && targeted_valid) {
        DrawCenteredText(draw_list, ImVec2(center_top.x, y), IM_COL32(220, 220, 220, 240), "Targeted NPC");
        y += ImGui::GetTextLineHeight() + 2.0f;
    } else if (last_hit_valid) {
        DrawCenteredText(draw_list, ImVec2(center_top.x, y), IM_COL32(220, 220, 220, 240), "Last Hit NPC");
        y += ImGui::GetTextLineHeight() + 2.0f;
    }

    char hp_text[64]{};
    sprintf_s(hp_text, "HP %d / %d", hp, max_hp);
    DrawCenteredText(draw_list, ImVec2(center_top.x, y), IM_COL32(255, 220, 140, 245), hp_text);
}

float GetDamageFontScale(int damage) {
    if (damage > 2000) {
        return 2.05f;
    }
    if (damage > 1500) {
        return 1.82f;
    }
    if (damage > 1000) {
        return 1.55f;
    }
    if (damage > 500) {
        return 1.28f;
    }
    return 1.0f;
}

ImU32 GetDamageTextColor(int damage) {
    if (damage > 2000) {
        return IM_COL32(255, 86, 60, 255);
    }
    if (damage > 1500) {
        return IM_COL32(255, 108, 70, 255);
    }
    if (damage > 1000) {
        return IM_COL32(255, 156, 92, 255);
    }
    if (damage > 500) {
        return IM_COL32(255, 180, 112, 255);
    }
    return IM_COL32(255, 196, 120, 245);
}

void PushDamageHistory(std::uint64_t handle, int damage) {
    if (handle == 0 || damage <= 0) {
        return;
    }

    const ULONGLONG now = GetTickCount64();
    if (!g_DamageHistory.empty()) {
        const DamageHistoryEntry& latest = g_DamageHistory.front();
        if (latest.handle == handle && latest.damage == damage && now - latest.tick < 1500ULL) {
            return;
        }
    }

    g_DamageHistory.insert(g_DamageHistory.begin(), DamageHistoryEntry{handle, damage, now});
    if (g_DamageHistory.size() > 48) {
        g_DamageHistory.resize(48);
    }
}

void DrawDamageHistory(ImDrawList* draw_list, const ImVec2& center_bottom, std::uint64_t handle) {
    if (draw_list == nullptr || handle == 0) {
        return;
    }

    constexpr ULONGLONG kHistoryLifetimeMs = 2600ULL;
    constexpr std::size_t kMaxEntries = 5;
    const ULONGLONG now = GetTickCount64();
    ImFont* font = ImGui::GetFont();
    const float base_font_size = 10.0f;
    float y_cursor = center_bottom.y;
    std::size_t drawn = 0;

    for (const DamageHistoryEntry& entry : g_DamageHistory) {
        if (entry.handle != handle || entry.damage <= 0 || now - entry.tick > kHistoryLifetimeMs) {
            continue;
        }
        if (drawn >= kMaxEntries) {
            break;
        }

        const float age_alpha = 1.0f - static_cast<float>(now - entry.tick) / static_cast<float>(kHistoryLifetimeMs);
        const float tier_scale = GetDamageFontScale(entry.damage);
        const float stack_scale = std::pow(0.8f, static_cast<float>(drawn));
        const float render_font_size = std::max(6.0f, base_font_size * tier_scale * stack_scale);
        const float vertical_gap = render_font_size + 5.0f;
        y_cursor -= vertical_gap;

        char damage_text[48]{};
        sprintf_s(damage_text, "-%d", entry.damage);
        const ImVec2 text_size = font->CalcTextSizeA(render_font_size, FLT_MAX, 0.0f, damage_text);
        const ImVec2 text_pos(center_bottom.x - text_size.x * 0.5f, y_cursor);
        const ImU32 base_color = GetDamageTextColor(entry.damage);
        const int alpha = static_cast<int>(std::clamp(age_alpha, 0.15f, 1.0f) * 255.0f);
        const ImU32 text_color = (base_color & 0x00FFFFFF) | (static_cast<ImU32>(alpha) << 24);
        const float outline_offset = entry.damage > 500 ? 2.0f : 1.0f;

        draw_list->AddText(
            font,
            render_font_size,
            ImVec2(text_pos.x + outline_offset, text_pos.y + outline_offset),
            IM_COL32(20, 12, 8, std::min(220, alpha)),
            damage_text
        );
        if (entry.damage > 500) {
            draw_list->AddText(
                font,
                render_font_size,
                ImVec2(text_pos.x, text_pos.y + 4.0f),
                IM_COL32(255, 120, 60, std::min(120, alpha / 2 + 32)),
                damage_text
            );
        }
        draw_list->AddText(font, render_font_size, text_pos, text_color, damage_text);
        ++drawn;
    }
}

void TrackFallbackDamage(std::uint64_t handle, int hp) {
    if (handle == 0 || hp <= 0) {
        return;
    }

    const auto previous = g_PreviousBarHpByHandle.find(handle);
    if (previous != g_PreviousBarHpByHandle.end() && hp < previous->second) {
        g_FallbackDamageByHandle[handle] = previous->second - hp;
        g_FallbackDamageTicksByHandle[handle] = GetTickCount64();
    }

    g_PreviousBarHpByHandle[handle] = hp;
}

int GetFallbackDamage(std::uint64_t handle) {
    const auto damage_it = g_FallbackDamageByHandle.find(handle);
    const auto tick_it = g_FallbackDamageTicksByHandle.find(handle);
    if (damage_it == g_FallbackDamageByHandle.end() || tick_it == g_FallbackDamageTicksByHandle.end()) {
        return 0;
    }

    if (GetTickCount64() - tick_it->second > 1400ULL) {
        g_FallbackDamageByHandle.erase(handle);
        g_FallbackDamageTicksByHandle.erase(handle);
        return 0;
    }

    return std::max(damage_it->second, 0);
}

void UpdateNpcDamageTracker(NpcHpTrackerState& state, bool valid, std::uint64_t handle, int hp) {
    if (!valid || handle == 0 || hp <= 0) {
        state = {};
        return;
    }

    if (state.handle != handle) {
        state.handle = handle;
        state.previous_hp = hp;
        return;
    }

    if (state.previous_hp > 0 && hp < state.previous_hp) {
        PushDamageHistory(handle, state.previous_hp - hp);
    }

    state.previous_hp = hp;
}

void UpdateTrackedNpcDamagePopups() {
    UpdateNpcDamageTracker(
        g_TargetedDamageTracker,
        g_FeatureStatus.targeted_npc_valid.load(),
        static_cast<std::uint64_t>(g_FeatureStatus.targeted_npc_handle.load()),
        g_FeatureStatus.targeted_npc_hp.load()
    );
    UpdateNpcDamageTracker(
        g_LastHitDamageTracker,
        g_FeatureStatus.last_hit_npc_valid.load(),
        static_cast<std::uint64_t>(g_FeatureStatus.last_hit_npc_handle.load()),
        g_FeatureStatus.last_hit_npc_hp.load()
    );
}

struct ResistanceBarDefinition {
    const char* label;
    int current;
    int maximum;
    ImU32 color;
};

bool HasTargetedResistanceSnapshot() {
    return g_FeatureStatus.targeted_npc_valid.load() &&
           (g_FeatureStatus.targeted_npc_max_hp.load() > 0 ||
            g_FeatureStatus.targeted_npc_poison_max.load() > 0 ||
            g_FeatureStatus.targeted_npc_rot_max.load() > 0 ||
            g_FeatureStatus.targeted_npc_bleed_max.load() > 0 ||
            g_FeatureStatus.targeted_npc_blight_max.load() > 0 ||
            g_FeatureStatus.targeted_npc_frost_max.load() > 0 ||
            g_FeatureStatus.targeted_npc_sleep_max.load() > 0 ||
            g_FeatureStatus.targeted_npc_madness_max.load() > 0);
}

bool HasLastHitResistanceSnapshot() {
    return g_FeatureStatus.last_hit_npc_valid.load() &&
           (g_FeatureStatus.last_hit_npc_max_hp.load() > 0 ||
            g_FeatureStatus.last_hit_npc_poison_max.load() > 0 ||
            g_FeatureStatus.last_hit_npc_rot_max.load() > 0 ||
            g_FeatureStatus.last_hit_npc_bleed_max.load() > 0 ||
            g_FeatureStatus.last_hit_npc_blight_max.load() > 0 ||
            g_FeatureStatus.last_hit_npc_frost_max.load() > 0 ||
            g_FeatureStatus.last_hit_npc_sleep_max.load() > 0 ||
            g_FeatureStatus.last_hit_npc_madness_max.load() > 0);
}

void DrawTargetedResistancePanel(ImDrawList* draw_list) {
    if (draw_list == nullptr) {
        return;
    }

    const bool targeted_valid = g_FeatureStatus.targeted_npc_valid.load();
    const bool last_hit_valid = g_FeatureStatus.last_hit_npc_valid.load();
    const bool targeted_ready = HasTargetedResistanceSnapshot();
    const bool last_hit_ready = HasLastHitResistanceSnapshot();
    if (!targeted_valid && !last_hit_valid) {
        return;
    }

    const bool use_targeted = targeted_valid && (targeted_ready || !last_hit_ready || !last_hit_valid);

    ImGuiViewport* viewport = ImGui::GetMainViewport();
    const float panel_width = 260.0f;
    const float panel_padding = 8.0f;
    const float bar_height = 6.0f;
    const float row_gap = 7.0f;
    const float section_gap = 8.0f;
    const float title_height = ImGui::GetTextLineHeight();
    const float line_height = ImGui::GetTextLineHeight();
    const float card_height = line_height + 4.0f + bar_height + panel_padding * 2.0f;

    const ResistanceBarDefinition defs[] = {
        {
            "Poison",
            use_targeted ? g_FeatureStatus.targeted_npc_poison.load() : g_FeatureStatus.last_hit_npc_poison.load(),
            use_targeted ? g_FeatureStatus.targeted_npc_poison_max.load() : g_FeatureStatus.last_hit_npc_poison_max.load(),
            IM_COL32(121, 199, 76, 235)
        },
        {
            "Scarlet Rot",
            use_targeted ? g_FeatureStatus.targeted_npc_rot.load() : g_FeatureStatus.last_hit_npc_rot.load(),
            use_targeted ? g_FeatureStatus.targeted_npc_rot_max.load() : g_FeatureStatus.last_hit_npc_rot_max.load(),
            IM_COL32(181, 72, 49, 235)
        },
        {
            "Bleed",
            use_targeted ? g_FeatureStatus.targeted_npc_bleed.load() : g_FeatureStatus.last_hit_npc_bleed.load(),
            use_targeted ? g_FeatureStatus.targeted_npc_bleed_max.load() : g_FeatureStatus.last_hit_npc_bleed_max.load(),
            IM_COL32(165, 44, 44, 235)
        },
        {
            "Death Blight",
            use_targeted ? g_FeatureStatus.targeted_npc_blight.load() : g_FeatureStatus.last_hit_npc_blight.load(),
            use_targeted ? g_FeatureStatus.targeted_npc_blight_max.load() : g_FeatureStatus.last_hit_npc_blight_max.load(),
            IM_COL32(160, 142, 84, 235)
        },
        {
            "Frostbite",
            use_targeted ? g_FeatureStatus.targeted_npc_frost.load() : g_FeatureStatus.last_hit_npc_frost.load(),
            use_targeted ? g_FeatureStatus.targeted_npc_frost_max.load() : g_FeatureStatus.last_hit_npc_frost_max.load(),
            IM_COL32(82, 174, 214, 235)
        },
        {
            "Sleep",
            use_targeted ? g_FeatureStatus.targeted_npc_sleep.load() : g_FeatureStatus.last_hit_npc_sleep.load(),
            use_targeted ? g_FeatureStatus.targeted_npc_sleep_max.load() : g_FeatureStatus.last_hit_npc_sleep_max.load(),
            IM_COL32(137, 98, 193, 235)
        },
        {
            "Madness",
            use_targeted ? g_FeatureStatus.targeted_npc_madness.load() : g_FeatureStatus.last_hit_npc_madness.load(),
            use_targeted ? g_FeatureStatus.targeted_npc_madness_max.load() : g_FeatureStatus.last_hit_npc_madness_max.load(),
            IM_COL32(214, 180, 49, 235)
        },
    };

    const float panel_height =
        title_height + line_height + section_gap + static_cast<float>(std::size(defs)) * card_height +
        static_cast<float>(std::size(defs) - 1) * row_gap;

    const ImVec2 panel_min(
        viewport->Pos.x + viewport->Size.x - panel_width - 24.0f,
        viewport->Pos.y + 22.0f
    );

    const std::string npc_name = use_targeted ? Features::GetTargetedNpcNameUtf8() : Features::GetLastHitNpcNameUtf8();
    const char* title = nullptr;
    if (!npc_name.empty()) {
        title = npc_name.c_str();
    } else {
        title = use_targeted ? "Target Resistances" : "Last Hit Resistances";
    }
    draw_list->AddText(
        ImVec2(panel_min.x + 2.0f, panel_min.y),
        IM_COL32(232, 206, 164, 245),
        title
    );
    draw_list->AddText(
        ImVec2(panel_min.x + 2.0f, panel_min.y + title_height + 1.0f),
        IM_COL32(168, 162, 150, 205),
        "Status buildup / Resistance"
    );

    float y = panel_min.y + title_height + section_gap + line_height;
    char value_text[96]{};
    for (const ResistanceBarDefinition& def : defs) {
        const ImVec2 card_min(panel_min.x, y);
        const ImVec2 card_max(panel_min.x + panel_width, y + card_height);
        draw_list->AddRectFilled(card_min, card_max, IM_COL32(10, 8, 7, 164), 6.0f);
        draw_list->AddRect(card_min, card_max, GetSoulBorderOuter(), 6.0f, 0, 1.4f);
        draw_list->AddRect(
            ImVec2(card_min.x + 1.0f, card_min.y + 1.0f),
            ImVec2(card_max.x - 1.0f, card_max.y - 1.0f),
            GetSoulBorderInner(),
            5.0f,
            0,
            1.0f
        );

        sprintf_s(value_text, "%s: %d / %d", def.label, def.current, def.maximum);
        draw_list->AddText(
            ImVec2(card_min.x + panel_padding, card_min.y + panel_padding - 1.0f),
            IM_COL32(228, 220, 206, 240),
            value_text
        );

        const ImVec2 bar_center(
            card_min.x + panel_width * 0.5f,
            card_min.y + panel_padding + line_height + 4.0f
        );
        DrawSimplePostureBar(
            draw_list,
            bar_center,
            ImVec2(panel_width - panel_padding * 2.0f, bar_height),
            def.maximum > 0 ? static_cast<float>(def.current) / static_cast<float>(def.maximum) : 0.0f,
            def.color
        );
        y += card_height + row_gap;
    }
}

void DrawIntegratedPostureBars() {
    const Features::PostureBarsSnapshot snapshot = Features::GetPostureBarsSnapshot();
    if (!snapshot.hook_installed) {
        return;
    }

    ImDrawList* draw_list = ImGui::GetBackgroundDrawList();
    if (draw_list == nullptr) {
        return;
    }

    UpdateTrackedNpcDamagePopups();

    const GameViewportTransform transform = BuildGameViewportTransform();
    const std::uint64_t targeted_handle = static_cast<std::uint64_t>(g_FeatureStatus.targeted_npc_handle.load());
    const bool targeted_valid = g_FeatureStatus.targeted_npc_valid.load();
    const std::uint64_t last_hit_handle = static_cast<std::uint64_t>(g_FeatureStatus.last_hit_npc_handle.load());
    const bool last_hit_valid = g_FeatureStatus.last_hit_npc_valid.load();
    const std::uint64_t focus_handle = targeted_valid ? targeted_handle : last_hit_handle;
    const bool focus_valid = targeted_valid || last_hit_valid;
    bool targeted_label_drawn = false;

    for (const Features::PostureBarEntry& boss_bar : snapshot.boss_bars) {
        if (!boss_bar.visible) {
            continue;
        }

        TrackFallbackDamage(boss_bar.handle, boss_bar.hp);

        const ImVec2 center = ToViewportPosition(transform, boss_bar.screen_x, boss_bar.screen_y);
        const ImVec2 size(998.0f * transform.scale, std::max(8.0f, 16.0f * transform.scale));
        DrawSimplePostureBar(
            draw_list,
            center,
            size,
            boss_bar.current / boss_bar.maximum,
            GetPostureBarOrange()
        );

        if (!targeted_label_drawn && focus_valid && boss_bar.handle == focus_handle) {
            const int display_damage = std::max(boss_bar.recent_damage, GetFallbackDamage(boss_bar.handle));
            if (display_damage > 0) {
                PushDamageHistory(boss_bar.handle, display_damage);
            }
            DrawDamageHistory(draw_list, ImVec2(center.x, center.y - 4.0f * transform.scale), boss_bar.handle);
            DrawTargetedNpcSummary(
                draw_list,
                ImVec2(center.x, center.y + size.y + 6.0f * transform.scale)
            );
            targeted_label_drawn = true;
        }
    }

    for (const Features::PostureBarEntry& entity_bar : snapshot.entity_bars) {
        if (!entity_bar.visible) {
            continue;
        }

        TrackFallbackDamage(entity_bar.handle, entity_bar.hp);

        const float clamped_game_x = std::clamp(entity_bar.screen_x, 130.0f, 1790.0f) - 1.0f;
        const float clamped_game_y = std::clamp(entity_bar.screen_y, 175.0f, 990.0f) - 10.0f;
        const ImVec2 center = ToViewportPosition(transform, clamped_game_x, clamped_game_y);
        const ImVec2 size(
            std::max(60.0f, 138.0f * entity_bar.distance_modifier) * transform.scale,
            std::max(4.0f, 5.0f * transform.scale)
        );

        DrawSimplePostureBar(
            draw_list,
            center,
            size,
            entity_bar.current / entity_bar.maximum,
            GetPostureBarOrange()
        );

        if (!targeted_label_drawn && focus_valid && entity_bar.handle == focus_handle) {
            const int display_damage = std::max(entity_bar.recent_damage, GetFallbackDamage(entity_bar.handle));
            if (display_damage > 0) {
                PushDamageHistory(entity_bar.handle, display_damage);
            }
            DrawDamageHistory(draw_list, ImVec2(center.x, center.y - 4.0f * transform.scale), entity_bar.handle);
            DrawTargetedNpcSummary(
                draw_list,
                ImVec2(center.x, center.y + size.y + 6.0f * transform.scale)
            );
            targeted_label_drawn = true;
        }
    }

    DrawTargetedResistancePanel(draw_list);
}

}  // namespace

float GetMenuOpacity() {
    return g_MenuOpacity;
}

bool IsMenuMinimized() {
    return g_MenuMinimized;
}

void SetMenuPreferences(float opacity, bool minimized) {
    g_MenuOpacity = std::clamp(opacity, 0.20f, 1.00f);
    g_MenuMinimized = minimized;
    g_MenuCollapseDirty = true;
    g_MenuRectValid = false;
}

D3D12Overlay::~D3D12Overlay() noexcept {
    Unhook();
}

bool D3D12Overlay::Hook() {
    if (hooked_) {
        return true;
    }

    if (!InitializeHook()) {
        Logger::Instance().Error("Failed to initialize DX12 overlay hook.");
        return false;
    }

    if (!CreateHook(
            kExecuteCommandListsIndex,
            reinterpret_cast<void**>(&original_execute_command_lists_),
            reinterpret_cast<void*>(&HookExecuteCommandLists)
        ) ||
        !CreateHook(
            kPresentIndex,
            reinterpret_cast<void**>(&original_present_),
            reinterpret_cast<void*>(&HookPresent)
        ) ||
        !CreateHook(
            kResizeBuffersIndex,
            reinterpret_cast<void**>(&original_resize_buffers_),
            reinterpret_cast<void*>(&HookResizeBuffers)
        )) {
        Logger::Instance().Error("Failed to create one or more DX12 hooks.");
        return false;
    }

    hooked_ = true;
    Logger::Instance().Info("DX12 overlay hook installed.");
    return true;
}

void D3D12Overlay::Unhook() {
    g_FeatureStatus.overlay_ready = false;

    if (hooked_) {
        MH_DisableHook(MH_ALL_HOOKS);
        MH_RemoveHook(MH_ALL_HOOKS);
        MH_Uninitialize();
        hooked_ = false;
    }

    ResetRenderState();

    if (methods_table_ != nullptr) {
        std::free(methods_table_);
        methods_table_ = nullptr;
    }

    if (dummy_window_ != nullptr) {
        DestroyWindow(dummy_window_);
        dummy_window_ = nullptr;
    }

    if (window_class_.lpszClassName != nullptr) {
        UnregisterClassW(window_class_.lpszClassName, window_class_.hInstance);
        window_class_ = {};
    }
}

HRESULT APIENTRY D3D12Overlay::HookPresent(IDXGISwapChain* swap_chain, UINT sync_interval, UINT flags) {
    if (g_D3D12Overlay != nullptr) {
        g_D3D12Overlay->Render(swap_chain);
        return g_D3D12Overlay->original_present_(swap_chain, sync_interval, flags);
    }

    return S_OK;
}

void APIENTRY D3D12Overlay::HookExecuteCommandLists(
    ID3D12CommandQueue* queue,
    UINT num_command_lists,
    ID3D12CommandList* const* command_lists) {
    if (g_D3D12Overlay != nullptr && g_D3D12Overlay->command_queue_ == nullptr) {
        if (queue->GetDesc().Type == D3D12_COMMAND_LIST_TYPE_DIRECT) {
            g_D3D12Overlay->command_queue_ = queue;
        }
    }

    if (g_D3D12Overlay != nullptr && g_D3D12Overlay->original_execute_command_lists_ != nullptr) {
        g_D3D12Overlay->original_execute_command_lists_(queue, num_command_lists, command_lists);
    }
}

HRESULT APIENTRY D3D12Overlay::HookResizeBuffers(IDXGISwapChain* swap_chain,
                                                 UINT buffer_count,
                                                 UINT width,
                                                 UINT height,
                                                 DXGI_FORMAT new_format,
                                                 UINT flags) {
    if (g_D3D12Overlay != nullptr) {
        g_D3D12Overlay->ResetRenderState();
        return g_D3D12Overlay->original_resize_buffers_(swap_chain, buffer_count, width, height, new_format, flags);
    }

    return S_OK;
}

LRESULT CALLBACK D3D12Overlay::HookWndProc(HWND hwnd, UINT msg, WPARAM w_param, LPARAM l_param) {
    if (g_D3D12Overlay == nullptr || g_D3D12Overlay->original_wnd_proc_ == nullptr) {
        return DefWindowProcW(hwnd, msg, w_param, l_param);
    }

    if (msg == WM_KEYUP && w_param == VK_F1) {
        g_MenuToggleRequested = true;
        return 0;
    }

    if (g_D3D12Overlay->initialized_ && g_FeatureStatus.menu_visible.load()) {
        ImGui_ImplWin32_WndProcHandler(hwnd, msg, w_param, l_param);

        const ImGuiIO& io = ImGui::GetIO();
        if (IsMouseMessage(msg)) {
            POINT point{};
            if (GetMouseClientPosition(hwnd, msg, l_param, point) && IsPointInsideMenuRect(point)) {
                return 0;
            }
        }

        if (io.WantCaptureKeyboard && IsKeyboardMessage(msg)) {
            return 0;
        }
    }

    return CallWindowProcW(g_D3D12Overlay->original_wnd_proc_, hwnd, msg, w_param, l_param);
}

bool D3D12Overlay::InitializeHook() {
    game_window_ = FindGameWindow();
    if (game_window_ == nullptr) {
        Logger::Instance().Error("Failed to find Elden Ring window for overlay.");
        return false;
    }

    window_class_.cbSize = sizeof(WNDCLASSEXW);
    window_class_.style = CS_HREDRAW | CS_VREDRAW;
    window_class_.lpfnWndProc = DefWindowProcW;
    window_class_.hInstance = GetModuleHandleW(nullptr);
    window_class_.lpszClassName = L"ERDModDX12DummyWindow";
    RegisterClassExW(&window_class_);

    dummy_window_ = CreateWindowW(
        window_class_.lpszClassName,
        L"ERDMod Dummy Window",
        WS_OVERLAPPEDWINDOW,
        0,
        0,
        100,
        100,
        nullptr,
        nullptr,
        window_class_.hInstance,
        nullptr
    );
    if (dummy_window_ == nullptr) {
        return false;
    }

    IDXGIFactory4* factory = nullptr;
    IDXGIAdapter1* adapter = nullptr;
    ID3D12Device* device = nullptr;
    ID3D12CommandQueue* command_queue = nullptr;
    ID3D12CommandAllocator* command_allocator = nullptr;
    ID3D12GraphicsCommandList* command_list = nullptr;
    IDXGISwapChain* swap_chain = nullptr;

    if (CreateDXGIFactory1(IID_PPV_ARGS(&factory)) != S_OK) {
        return false;
    }

    if (factory->EnumAdapters1(0, &adapter) == DXGI_ERROR_NOT_FOUND) {
        factory->Release();
        return false;
    }

    if (D3D12CreateDevice(adapter, D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&device)) != S_OK) {
        adapter->Release();
        factory->Release();
        return false;
    }

    D3D12_COMMAND_QUEUE_DESC queue_desc{};
    queue_desc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
    if (device->CreateCommandQueue(&queue_desc, IID_PPV_ARGS(&command_queue)) != S_OK) {
        device->Release();
        adapter->Release();
        factory->Release();
        return false;
    }

    if (device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&command_allocator)) != S_OK) {
        command_queue->Release();
        device->Release();
        adapter->Release();
        factory->Release();
        return false;
    }

    if (device->CreateCommandList(
            0,
            D3D12_COMMAND_LIST_TYPE_DIRECT,
            command_allocator,
            nullptr,
            IID_PPV_ARGS(&command_list)
        ) != S_OK) {
        command_allocator->Release();
        command_queue->Release();
        device->Release();
        adapter->Release();
        factory->Release();
        return false;
    }

    DXGI_SWAP_CHAIN_DESC swap_chain_desc{};
    swap_chain_desc.BufferCount = 2;
    swap_chain_desc.BufferDesc.Width = 100;
    swap_chain_desc.BufferDesc.Height = 100;
    swap_chain_desc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    swap_chain_desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    swap_chain_desc.OutputWindow = dummy_window_;
    swap_chain_desc.SampleDesc.Count = 1;
    swap_chain_desc.Windowed = TRUE;
    swap_chain_desc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;

    if (factory->CreateSwapChain(command_queue, &swap_chain_desc, &swap_chain) != S_OK) {
        command_list->Release();
        command_allocator->Release();
        command_queue->Release();
        device->Release();
        adapter->Release();
        factory->Release();
        return false;
    }

    methods_table_ = static_cast<std::uint64_t*>(std::calloc(150, sizeof(std::uint64_t)));
    std::memcpy(methods_table_, *reinterpret_cast<std::uint64_t**>(device), 44 * sizeof(std::uint64_t));
    std::memcpy(methods_table_ + 44, *reinterpret_cast<std::uint64_t**>(command_queue), 19 * sizeof(std::uint64_t));
    std::memcpy(methods_table_ + 44 + 19, *reinterpret_cast<std::uint64_t**>(command_allocator), 9 * sizeof(std::uint64_t));
    std::memcpy(methods_table_ + 44 + 19 + 9, *reinterpret_cast<std::uint64_t**>(command_list), 60 * sizeof(std::uint64_t));
    std::memcpy(methods_table_ + 44 + 19 + 9 + 60, *reinterpret_cast<std::uint64_t**>(swap_chain), 18 * sizeof(std::uint64_t));

    const MH_STATUS mh_status = MH_Initialize();
    if (mh_status != MH_OK && mh_status != MH_ERROR_ALREADY_INITIALIZED) {
        swap_chain->Release();
        command_list->Release();
        command_allocator->Release();
        command_queue->Release();
        device->Release();
        adapter->Release();
        factory->Release();
        return false;
    }

    swap_chain->Release();
    command_list->Release();
    command_allocator->Release();
    command_queue->Release();
    device->Release();
    adapter->Release();
    factory->Release();
    return true;
}

bool D3D12Overlay::InitializeRenderer(IDXGISwapChain* swap_chain) {
    if (initialized_) {
        return true;
    }

    if (command_queue_ == nullptr) {
        return false;
    }

    if (swap_chain->QueryInterface(IID_PPV_ARGS(&swap_chain_)) != S_OK) {
        return false;
    }

    DXGI_SWAP_CHAIN_DESC swap_chain_desc{};
    if (swap_chain_->GetDesc(&swap_chain_desc) != S_OK) {
        return false;
    }

    if (swap_chain_desc.OutputWindow != nullptr) {
        game_window_ = swap_chain_desc.OutputWindow;
    }

    if (swap_chain_->GetDevice(IID_PPV_ARGS(&device_)) != S_OK) {
        return false;
    }

    D3D12_DESCRIPTOR_HEAP_DESC srv_desc{};
    srv_desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
    srv_desc.NumDescriptors = 1;
    srv_desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
    if (device_->CreateDescriptorHeap(&srv_desc, IID_PPV_ARGS(&srv_descriptor_heap_)) != S_OK) {
        return false;
    }

    D3D12_DESCRIPTOR_HEAP_DESC rtv_desc{};
    rtv_desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
    rtv_desc.NumDescriptors = swap_chain_desc.BufferCount;
    rtv_desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
    if (device_->CreateDescriptorHeap(&rtv_desc, IID_PPV_ARGS(&rtv_descriptor_heap_)) != S_OK) {
        return false;
    }

    const SIZE_T rtv_increment = device_->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
    D3D12_CPU_DESCRIPTOR_HANDLE rtv_handle = rtv_descriptor_heap_->GetCPUDescriptorHandleForHeapStart();

    frames_.resize(swap_chain_desc.BufferCount);
    for (UINT i = 0; i < swap_chain_desc.BufferCount; ++i) {
        if (device_->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&frames_[i].allocator)) != S_OK) {
            return false;
        }

        if (swap_chain_->GetBuffer(i, IID_PPV_ARGS(&frames_[i].back_buffer)) != S_OK) {
            return false;
        }

        frames_[i].rtv_handle = rtv_handle;
        device_->CreateRenderTargetView(frames_[i].back_buffer, nullptr, frames_[i].rtv_handle);
        rtv_handle.ptr += rtv_increment;
    }

    if (device_->CreateCommandList(
            0,
            D3D12_COMMAND_LIST_TYPE_DIRECT,
            frames_[0].allocator,
            nullptr,
            IID_PPV_ARGS(&command_list_)
        ) != S_OK) {
        return false;
    }
    command_list_->Close();

    if (!InitializeSyncObjects()) {
        return false;
    }

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui::StyleColorsDark();

    ImGuiIO& io = ImGui::GetIO();
    io.IniFilename = nullptr;
    io.LogFilename = nullptr;
    io.ConfigWindowsMoveFromTitleBarOnly = true;

    if (!LoadChineseFont()) {
        io.Fonts->AddFontDefault();
        Logger::Instance().Error("Failed to load a Chinese font for ImGui. Falling back to default font.");
    }

    if (!ImGui_ImplWin32_Init(game_window_)) {
        return false;
    }

    if (!ImGui_ImplDX12_Init(
            device_,
            static_cast<int>(swap_chain_desc.BufferCount),
            swap_chain_desc.BufferDesc.Format,
            srv_descriptor_heap_,
            srv_descriptor_heap_->GetCPUDescriptorHandleForHeapStart(),
            srv_descriptor_heap_->GetGPUDescriptorHandleForHeapStart()
        )) {
        return false;
    }

    if (!InstallWindowProcHook()) {
        Logger::Instance().Error("Failed to install window procedure hook for ImGui input. Overlay will continue without WndProc input hook.");
    }

    initialized_ = true;
    g_FeatureStatus.overlay_ready = true;
    Logger::Instance().Info("ImGui DX12 overlay initialized.");
    return true;
}

bool D3D12Overlay::InitializeSyncObjects() {
    if (device_->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&fence_)) != S_OK) {
        return false;
    }

    fence_event_ = CreateEventW(nullptr, FALSE, FALSE, nullptr);
    if (fence_event_ == nullptr) {
        return false;
    }

    next_fence_value_ = 1;
    return true;
}

bool D3D12Overlay::LoadChineseFont() {
    ImGuiIO& io = ImGui::GetIO();
    const ImWchar* ranges = io.Fonts->GetGlyphRangesChineseFull();

    constexpr const char* kFontCandidates[] = {
        "C:\\Windows\\Fonts\\msyh.ttc",
        "C:\\Windows\\Fonts\\simhei.ttf",
        "C:\\Windows\\Fonts\\simsun.ttc",
    };

    for (const char* font_path : kFontCandidates) {
        if (GetFileAttributesA(font_path) == INVALID_FILE_ATTRIBUTES) {
            continue;
        }

        if (io.Fonts->AddFontFromFileTTF(font_path, 20.0f, nullptr, ranges) != nullptr) {
            return true;
        }
    }

    return false;
}

bool D3D12Overlay::InstallWindowProcHook() {
    if (original_wnd_proc_ != nullptr || game_window_ == nullptr) {
        return game_window_ != nullptr;
    }

    SetLastError(0);
    const LONG_PTR previous = SetWindowLongPtrW(game_window_, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(&HookWndProc));
    if (previous == 0 && GetLastError() != 0) {
        return false;
    }

    original_wnd_proc_ = reinterpret_cast<WNDPROC>(previous);
    return true;
}

void D3D12Overlay::Render(IDXGISwapChain* swap_chain) {
    if (!InitializeRenderer(swap_chain)) {
        return;
    }

    const bool use_wndproc_hotkey = original_wnd_proc_ != nullptr;
    if (ConsumeMenuToggleRequest() || (!use_wndproc_hotkey && IsToggleHotkeyPressed())) {
        ToggleMenuVisibility();
    }

    const UINT buffer_index = swap_chain_->GetCurrentBackBufferIndex();
    FrameContext& frame = frames_[buffer_index];
    WaitForFrame(frame);

    const bool menu_visible = g_FeatureStatus.menu_visible.load();
    ImGuiIO& io = ImGui::GetIO();
    io.MouseDrawCursor = menu_visible;

    ImGui_ImplDX12_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();
    ERD_PROTECTED_STEP("Overlay.DrawIntegratedPostureBars", DrawIntegratedPostureBars());
    if (menu_visible) {
        ERD_PROTECTED_STEP("Overlay.DrawMenu", DrawMenu());
    }
    ImGui::Render();

    if (frame.allocator->Reset() != S_OK) {
        return;
    }

    if (command_list_->Reset(frame.allocator, nullptr) != S_OK) {
        return;
    }

    D3D12_RESOURCE_BARRIER barrier{};
    barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
    barrier.Transition.pResource = frame.back_buffer;
    barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
    barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;
    barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;
    command_list_->ResourceBarrier(1, &barrier);

    command_list_->OMSetRenderTargets(1, &frame.rtv_handle, FALSE, nullptr);
    command_list_->SetDescriptorHeaps(1, &srv_descriptor_heap_);

    ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), command_list_);

    barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
    barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;
    command_list_->ResourceBarrier(1, &barrier);
    command_list_->Close();

    ID3D12CommandList* command_lists[] = {command_list_};
    command_queue_->ExecuteCommandLists(1, command_lists);

    const UINT64 fence_value = next_fence_value_++;
    if (command_queue_->Signal(fence_, fence_value) == S_OK) {
        frame.fence_value = fence_value;
    }
}

void D3D12Overlay::ResetRenderState() {
    if (original_wnd_proc_ != nullptr && game_window_ != nullptr && IsWindow(game_window_)) {
        SetWindowLongPtrW(game_window_, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(original_wnd_proc_));
        original_wnd_proc_ = nullptr;
    }

    if (initialized_) {
        WaitForGpu();

        ImGui_ImplDX12_Shutdown();
        ImGui_ImplWin32_Shutdown();
        ImGui::DestroyContext();

        initialized_ = false;
        g_FeatureStatus.overlay_ready = false;
    }

    if (command_list_ != nullptr) {
        command_list_->Release();
        command_list_ = nullptr;
    }

    for (FrameContext& frame : frames_) {
        if (frame.allocator != nullptr) {
            frame.allocator->Release();
            frame.allocator = nullptr;
        }
        if (frame.back_buffer != nullptr) {
            frame.back_buffer->Release();
            frame.back_buffer = nullptr;
        }
        frame.fence_value = 0;
    }
    frames_.clear();

    if (fence_ != nullptr) {
        fence_->Release();
        fence_ = nullptr;
    }
    if (fence_event_ != nullptr) {
        CloseHandle(fence_event_);
        fence_event_ = nullptr;
    }
    next_fence_value_ = 0;

    if (rtv_descriptor_heap_ != nullptr) {
        rtv_descriptor_heap_->Release();
        rtv_descriptor_heap_ = nullptr;
    }
    if (srv_descriptor_heap_ != nullptr) {
        srv_descriptor_heap_->Release();
        srv_descriptor_heap_ = nullptr;
    }
    if (swap_chain_ != nullptr) {
        swap_chain_->Release();
        swap_chain_ = nullptr;
    }
    if (device_ != nullptr) {
        device_->Release();
        device_ = nullptr;
    }
}

void D3D12Overlay::WaitForFrame(FrameContext& frame) {
    if (fence_ == nullptr || fence_event_ == nullptr || frame.fence_value == 0) {
        return;
    }

    if (fence_->GetCompletedValue() < frame.fence_value) {
        if (fence_->SetEventOnCompletion(frame.fence_value, fence_event_) == S_OK) {
            WaitForSingleObject(fence_event_, INFINITE);
        }
    }

    frame.fence_value = 0;
}

void D3D12Overlay::WaitForGpu() {
    if (command_queue_ == nullptr || fence_ == nullptr || fence_event_ == nullptr || next_fence_value_ == 0) {
        return;
    }

    const UINT64 fence_value = next_fence_value_++;
    if (command_queue_->Signal(fence_, fence_value) != S_OK) {
        return;
    }

    if (fence_->GetCompletedValue() < fence_value &&
        fence_->SetEventOnCompletion(fence_value, fence_event_) == S_OK) {
        WaitForSingleObject(fence_event_, INFINITE);
    }
}

bool D3D12Overlay::CreateHook(std::uint16_t index, void** original, void* detour) {
    void* target = reinterpret_cast<void*>(methods_table_[index]);
    return MH_CreateHook(target, detour, original) == MH_OK &&
           MH_EnableHook(target) == MH_OK;
}

void D3D12Overlay::ToggleMenuVisibility() {
    const bool visible = !g_FeatureStatus.menu_visible.load();
    g_FeatureStatus.menu_visible.store(visible);

    if (initialized_) {
        ImGui::GetIO().MouseDrawCursor = visible;
    }
}

void D3D12Overlay::DrawMenu() {
    ImGuiViewport* viewport = ImGui::GetMainViewport();
    const ImVec2 viewport_center(
        viewport->Pos.x + viewport->Size.x * 0.5f,
        viewport->Pos.y + viewport->Size.y * 0.5f
    );

    ImGui::SetNextWindowSize(ImVec2(560.0f, 0.0f), ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowPos(viewport_center, ImGuiCond_FirstUseEver, ImVec2(0.5f, 0.5f));
    ImGui::SetNextWindowBgAlpha(g_MenuOpacity);
    ImGui::SetNextWindowCollapsed(
        g_MenuMinimized,
        g_MenuCollapseDirty ? ImGuiCond_Always : ImGuiCond_FirstUseEver
    );

    bool menu_visible = true;
    constexpr ImGuiWindowFlags flags = ImGuiWindowFlags_NoSavedSettings;

    const bool window_open = ImGui::Begin("ERDMod 中文菜单", &menu_visible, flags);
    const ImVec2 window_pos = ImGui::GetWindowPos();
    const ImVec2 window_size = ImGui::GetWindowSize();
    POINT top_left{
        static_cast<LONG>(window_pos.x),
        static_cast<LONG>(window_pos.y),
    };
    POINT bottom_right{
        static_cast<LONG>(window_pos.x + window_size.x),
        static_cast<LONG>(window_pos.y + window_size.y),
    };
    if (g_D3D12Overlay != nullptr && g_D3D12Overlay->game_window_ != nullptr) {
        ScreenToClient(g_D3D12Overlay->game_window_, &top_left);
        ScreenToClient(g_D3D12Overlay->game_window_, &bottom_right);
    }
    g_MenuRect.left = top_left.x;
    g_MenuRect.top = top_left.y;
    g_MenuRect.right = bottom_right.x;
    g_MenuRect.bottom = bottom_right.y;
    g_MenuRectValid = menu_visible;
    if (!menu_visible) {
        g_MenuMinimized = true;
        g_MenuCollapseDirty = true;
        menu_visible = true;
    }

    g_MenuMinimized = ImGui::IsWindowCollapsed();
    g_MenuCollapseDirty = false;
    if (!window_open || g_MenuMinimized) {
        ImGui::End();
        g_FeatureStatus.menu_visible.store(menu_visible);
        return;
    }

    if (window_open) {
        if (!menu_visible) {
            g_MenuMinimized = true;
            menu_visible = true;
        }

        g_MenuMinimized = ImGui::IsWindowCollapsed();
        if (g_MenuMinimized) {
            ImGui::End();
            g_FeatureStatus.menu_visible.store(menu_visible);
            return;
        }

        ImGui::TextUnformatted("ERDMod");
        ImGui::SameLine();
        ImGui::TextDisabled("| F1 显示 / 隐藏菜单");
        ImGui::SameLine();
        if (ImGui::Button(g_MenuMinimized ? "展开" : "最小化")) {
            g_MenuMinimized = !g_MenuMinimized;
            g_MenuCollapseDirty = true;
        }
        ImGui::SameLine();
        ImGui::SetNextItemWidth(140.0f);
        ImGui::SliderFloat("透明度", &g_MenuOpacity, 0.20f, 1.00f, "%.2f");
        ImGui::TextWrapped("菜单已按 CT 功能分类拆成多个页签。持续型功能可以随时开关；带输入值的功能会直接读取下方输入框；一次性动作请谨慎使用。");

        ImGui::Separator();
        const auto inspector = Features::GetInspectorSnapshot();
        if (ImGui::BeginTabBar("CTCategories")) {
            if (ImGui::BeginTabItem("角色")) {
                DrawBooleanToggle("不死", "对应 CT: NoDead。阻止角色进入死亡状态。", g_FeatureStatus.no_dead);
                DrawBooleanToggle("无伤", "对应 CT: NoDamage。角色不会受到伤害。", g_FeatureStatus.no_damage);
                DrawBooleanToggle("无限 FP / 不耗蓝", "对应 CT: NoFPConsumption。", g_FeatureStatus.infinite_fp);
                DrawBooleanToggle("道具不消耗", "对应 CT: NoGoodsConsume。", g_FeatureStatus.infinite_items);
                DrawBooleanToggle("不耗精力", "对应 CT: NoStaminaConsumption。", g_FeatureStatus.no_stamina_consumption);
                DrawBooleanToggle("魔法没有使用需求", "把全部魔法的智力、信仰和感应需求归零。", g_FeatureStatus.no_magic_requirements);
                DrawBooleanToggle("所有魔法只占一个栏位", "把全部魔法记忆栏位需求改成单栏位。", g_FeatureStatus.all_magic_one_slot);
                DrawBooleanToggle("装备无重量", "按 CT 原版逻辑把装备重量计算结果直接清零。", g_FeatureStatus.weightless_equipment);
                DrawIntSlider("掉宝率倍率", "对应 CT: Item discovery x10，可扩展为 1-100。", g_FeatureStatus.item_discovery_multiplier, 1, 100, "%d 倍");
                DrawBooleanToggle("常亮提灯", "提灯效果在切图和死亡后也会继续保存。", g_FeatureStatus.permanent_lantern);
                DrawBooleanToggle("头盔隐藏", "隐藏头盔模型，通常需要重新装备头盔后生效。", g_FeatureStatus.invisible_helmets);
                ImGui::EndTabItem();
            }

            if (ImGui::BeginTabItem("补丁 / 相机")) {
                DrawBooleanToggle("更快复活", "对应 CT: Faster respawn。", g_FeatureStatus.faster_respawn);
                DrawBooleanToggle("未清小地牢强制传出", "对应 CT: Warp out of uncleared mini-dungeons。", g_FeatureStatus.warp_out_of_uncleared_minidungeons);
                DrawBooleanToggle("商店免费购买", "把商店价格字段和 sellValue 归零，等价于 CT 的免费购买。", g_FeatureStatus.free_purchase);
                DrawBooleanToggle("制作不消耗材料", "迁移 CT 的制作不耗材料脚本，制作道具时不会扣除材料。", g_FeatureStatus.no_crafting_material_cost);
                DrawBooleanToggle("强化不消耗材料", "强化武器时不检查首项强化材料和数量。", g_FeatureStatus.no_upgrade_material_cost);
                DrawBooleanToggle("随处骑马", "解除大部分区域的灵马呼唤限制；切图后若失效可再切一次开关。", g_FeatureStatus.mount_anywhere);
                DrawBooleanToggle("随处召唤骨灰", "解除骨灰召唤限制，并同时扩大可召唤判定范围。", g_FeatureStatus.spirit_ashes_anywhere);
                DrawBooleanToggle("自定义 FOV", "对应 CT: Custom FOV。启用后会把 LockCamParam 的 camFovY 写成下方值。", g_FeatureStatus.custom_fov_enabled);
                DrawFloatInput("FOV 数值", "建议范围 20-120，CT 默认值为 60。", g_FeatureStatus.custom_fov_value, 20.0f, 120.0f, 1.0f, "%.1f");
                DrawBooleanToggle("自定义镜头距离", "对应 CT: Custom Camera Distance。", g_FeatureStatus.custom_camera_distance_enabled);
                DrawFloatInput("镜头距离", "建议范围 1-20，CT 默认值为 8。", g_FeatureStatus.custom_camera_distance_value, 1.0f, 20.0f, 0.1f, "%.2f");
                ImGui::EndTabItem();
            }

            if (ImGui::BeginTabItem("解锁")) {
                DrawOneShotButton("解锁全部地图", "解锁世界地图和 DLC 地图显示。", g_FeatureStatus.unlock_all_maps, "待执行", "已执行");
                DrawOneShotButton("解锁全部食谱", "一次性解锁全部制作食谱。", g_FeatureStatus.unlock_all_cookbooks, "待执行", "已执行");
                DrawOneShotButton("解锁全部砥石刀", "一次性解锁全部战灰质变用砥石刀。", g_FeatureStatus.unlock_all_whetblades, "待执行", "已执行");
                DrawOneShotButton("解锁全部赐福", "一次性点亮全部赐福，DLC 已购买时也会一起解锁。", g_FeatureStatus.unlock_all_graces, "待执行", "已执行");
                DrawOneShotButton("解锁全部召唤池", "一次性开启全部召唤池，适合联机测试。", g_FeatureStatus.unlock_all_summoning_pools, "待执行", "已执行");
                DrawOneShotButton("解锁全部斗技场", "一次性开放宁姆格福、盖利德和王城斗技场。", g_FeatureStatus.unlock_all_colosseums, "待执行", "已执行");
                DrawOneShotButton("解锁全部动作", "对应 CT: Unlock All Gestures。默认包含基础动作和已拥有 DLC 的动作。", g_FeatureStatus.unlock_all_gestures, "待执行", "已执行");
                ImGui::EndTabItem();
            }

            if (ImGui::BeginTabItem("Coords / Teleport")) {
                ImGui::TextDisabled("CT migration: Local / Global / Chunk / InitPos / LastGoodGlobal coordinate readout.");
                if (ImGui::Button("Read Player Coords", ImVec2(-1.0f, 0.0f))) {
                    g_FeatureStatus.teleport_read_player_coords_requested = true;
                }
                if (ImGui::Button("Use Global Coords", ImVec2(-1.0f, 0.0f))) {
                    g_FeatureStatus.teleport_use_global_coords_requested = true;
                }
                if (ImGui::Button("Use Last Good Global Coords", ImVec2(-1.0f, 0.0f))) {
                    g_FeatureStatus.teleport_use_last_good_global_coords_requested = true;
                }
                if (ImGui::Button("Use InitPos Coords", ImVec2(-1.0f, 0.0f))) {
                    g_FeatureStatus.teleport_use_init_pos_coords_requested = true;
                }
                if (ImGui::Button("Use Targeted NPC Coords", ImVec2(-1.0f, 0.0f))) {
                    g_FeatureStatus.teleport_use_targeted_npc_coords_requested = true;
                }
                if (ImGui::Button("Use Last Hit NPC Coords", ImVec2(-1.0f, 0.0f))) {
                    g_FeatureStatus.teleport_use_last_hit_npc_coords_requested = true;
                }
                ImGui::Spacing();
                DrawCoordReadout(
                    "Local Coords",
                    g_FeatureStatus.local_coord_x.load(),
                    g_FeatureStatus.local_coord_y.load(),
                    g_FeatureStatus.local_coord_z.load()
                );
                DrawCoordReadout(
                    "Global Coords",
                    g_FeatureStatus.global_coord_x.load(),
                    g_FeatureStatus.global_coord_y.load(),
                    g_FeatureStatus.global_coord_z.load()
                );
                DrawCoordReadout(
                    "Chunk Coords",
                    g_FeatureStatus.chunk_coord_x.load(),
                    g_FeatureStatus.chunk_coord_y.load(),
                    g_FeatureStatus.chunk_coord_z.load()
                );
                DrawCoordReadout(
                    "InitPos Coords",
                    g_FeatureStatus.init_pos_x.load(),
                    g_FeatureStatus.init_pos_y.load(),
                    g_FeatureStatus.init_pos_z.load()
                );
                DrawCoordReadout(
                    "LastGoodGlobal Coords",
                    g_FeatureStatus.last_good_global_x.load(),
                    g_FeatureStatus.last_good_global_y.load(),
                    g_FeatureStatus.last_good_global_z.load()
                );
                ImGui::TextDisabled(
                    "Player Rad %.3f | MapID 0x%08X",
                    g_FeatureStatus.player_rad.load(),
                    static_cast<unsigned>(g_FeatureStatus.player_map_id.load())
                );
                ImGui::Spacing();
                DrawFloatInput("Teleport X", "World X coordinate.", g_FeatureStatus.teleport_coord_x, -100000.0f, 100000.0f, 1.0f, "%.3f");
                DrawFloatInput("Teleport Y", "World Y coordinate.", g_FeatureStatus.teleport_coord_y, -100000.0f, 100000.0f, 1.0f, "%.3f");
                DrawFloatInput("Teleport Z", "World Z coordinate.", g_FeatureStatus.teleport_coord_z, -100000.0f, 100000.0f, 1.0f, "%.3f");
                if (ImGui::Button("Teleport Player To Input Coords", ImVec2(-1.0f, 0.0f))) {
                    g_FeatureStatus.teleport_player_to_input_coords_requested = true;
                }
                ImGui::TextDisabled("Y uses the game's vertical axis. This page now mirrors the core CT coordinate blocks.");
                ImGui::EndTabItem();
            }

            if (ImGui::BeginTabItem("Movement / Camera")) {
                DrawBooleanToggle(
                    "NoClip",
                    "CT: NoClip/Freecam -> Activate NoClip. Uses WASD + Space + Ctrl and current follow-cam direction.",
                    g_FeatureStatus.no_clip_enabled
                );
                DrawBooleanToggle(
                    "FreeCam",
                    "CT: NoClip/Freecam -> Activate Freecam. Detaches camera position and moves it with WASD + Space + Ctrl.",
                    g_FeatureStatus.freecam_enabled
                );
                DrawBooleanToggle(
                    "No Fall Camera",
                    "CT: No Fall Camera. Prevents the usual fall camera transition patch from triggering.",
                    g_FeatureStatus.no_fall_camera
                );
                DrawFloatInput(
                    "Movement Speed",
                    "Shared speed scalar for NoClip and FreeCam. CT default is 1.0.",
                    g_FeatureStatus.movement_speed,
                    0.1f,
                    20.0f,
                    0.1f,
                    "%.2f"
                );
                ImGui::TextDisabled("Keys: W / A / S / D / Space / Ctrl");
                ImGui::EndTabItem();
            }

            if (ImGui::BeginTabItem("Build / ItemGib")) {
                ImGui::TextDisabled("CT migration: MassItemGib batches are now callable directly from the DLL.");
                const int last_package_id = g_FeatureStatus.mass_item_gib_last_package_id.load();
                const int last_result = g_FeatureStatus.mass_item_gib_last_result.load();
                if (const Features::MassItemGibPackage* last_package = Features::FindMassItemGibPackage(last_package_id)) {
                    const ImVec4 status_color = (last_result > 0)
                                                    ? ImVec4(0.25f, 0.9f, 0.45f, 1.0f)
                                                    : ImVec4(0.95f, 0.35f, 0.35f, 1.0f);
                    if (last_result != 0) {
                        ImGui::TextColored(
                            status_color,
                            "Last batch: %s (%s)",
                            last_package->label,
                            last_result > 0 ? "Success" : "Failed"
                        );
                    }
                } else {
                    ImGui::TextDisabled("Large CT build-creation batches live here. Each button fires a full package.");
                }

                const char* current_category = nullptr;
                for (std::size_t index = 0; index < Features::kMassItemGibPackageCount; ++index) {
                    const Features::MassItemGibPackage& package = Features::kMassItemGibPackages[index];
                    if (current_category == nullptr || std::strcmp(current_category, package.category) != 0) {
                        current_category = package.category;
                        ImGui::Separator();
                        ImGui::TextUnformatted(current_category);
                    }
                    DrawMassItemGibButton(package);
                }

                ImGui::TextDisabled("These are batch actions. If a DLC package fails, check whether Shadow of the Erdtree is owned on the current save/profile.");
                ImGui::EndTabItem();
            }

            if (ImGui::BeginTabItem("添加物品")) {
                ImGui::TextDisabled("优先读取外置中文 CT 物品表；你可以直接改资源文件来新增或修正条目。");
                DrawCatalogCombo(
                    "物品枚举选择",
                    "从中文 CT 物品表中搜索名称或原始 ID，选中后会自动写入下面的 CT 原始 ID。",
                    g_FeatureStatus.custom_item_id,
                    CatalogKind::All,
                    g_CustomItemSearchBuffer
                );
                const CatalogKind detected_kind = DetectCatalogKindFromItemId(g_FeatureStatus.custom_item_id.load());
                if (detected_kind != CatalogKind::All) {
                    g_CustomItemManualKind = static_cast<int>(detected_kind) - 1;
                }
                const char* custom_item_kind_labels[] = {"武器", "防具", "护符", "道具"};
                ImGui::SetNextItemWidth(-1.0f);
                if (ImGui::Combo("手动输入类别", &g_CustomItemManualKind, custom_item_kind_labels, IM_ARRAYSIZE(custom_item_kind_labels))) {
                    const CatalogKind selected_kind = static_cast<CatalogKind>(g_CustomItemManualKind + 1);
                    g_FeatureStatus.custom_item_id = EncodeCatalogId(DecodeCatalogRawId(g_FeatureStatus.custom_item_id.load()), selected_kind);
                }
                DrawCatalogRawIdInput(
                    "CT 原始物品 ID",
                    "这里输入的是中文版 CT 里的原始十进制 ID，不是内部前缀编码。",
                    g_FeatureStatus.custom_item_id,
                    static_cast<CatalogKind>(g_CustomItemManualKind + 1)
                );
                DrawIntInput(
                    "数量",
                    "下一次发放的目标数量。",
                    g_FeatureStatus.custom_item_quantity,
                    1,
                    9999
                );
                DrawIntInput(
                    "强化等级",
                    "主要用于武器发放时的强化等级。",
                    g_FeatureStatus.custom_item_reinforce_level,
                    0,
                    25
                );
                DrawIntInput(
                    "Upgrade 值",
                    "对应 CT 里的 upgrade 偏移，通常保持 0 即可。",
                    g_FeatureStatus.custom_item_upgrade_value,
                    0,
                    1200
                );
                DrawIntInput(
                    "Gem ID",
                    "镶嵌/战灰相关值。普通物品保持 -1 即可。",
                    g_FeatureStatus.custom_item_gem_id,
                    -1,
                    999999999
                );
                if (ImGui::Button("发放物品", ImVec2(-1.0f, 0.0f))) {
                    g_FeatureStatus.custom_item_give_requested = true;
                }
                const int custom_item_result = g_FeatureStatus.custom_item_last_result.load();
                if (custom_item_result > 0) {
                    ImGui::TextColored(ImVec4(0.25f, 0.9f, 0.45f, 1.0f), "上一次发放结果：成功");
                } else if (custom_item_result < 0) {
                    ImGui::TextColored(ImVec4(0.95f, 0.35f, 0.35f, 1.0f), "上一次发放结果：失败");
                } else {
                    ImGui::TextDisabled("还没有发放过物品。");
                }
                ImGui::EndTabItem();
            }

            if (ImGui::BeginTabItem("角色编辑")) {
                ImGui::TextDisabled("角色主属性和资源值编辑。");
                if (ImGui::Button("读取角色数值", ImVec2(-1.0f, 0.0f))) {
                    g_FeatureStatus.character_read_requested = true;
                }
                if (ImGui::Button("应用角色数值", ImVec2(-1.0f, 0.0f))) {
                    g_FeatureStatus.character_apply_requested = true;
                }
                ImGui::TextDisabled(
                    "%s",
                    g_FeatureStatus.character_editor_valid.load()
                        ? "当前编辑框已经和角色实时数值同步。"
                        : "建议先读取一次，再开始修改。"
                );
                ImGui::Spacing();
                DrawIntInput("等级", "CT: Soul Level", g_FeatureStatus.character_level, 1, 713);
                DrawIntInput("Vigor", "CT: Vigor", g_FeatureStatus.character_vigor, 1, 99);
                DrawIntInput("Mind", "CT: Mind", g_FeatureStatus.character_mind, 1, 99);
                DrawIntInput("Endurance", "CT: Endurance", g_FeatureStatus.character_endurance, 1, 99);
                DrawIntInput("Strength", "CT: Strength", g_FeatureStatus.character_strength, 1, 99);
                DrawIntInput("Dexterity", "CT: Dexterity", g_FeatureStatus.character_dexterity, 1, 99);
                DrawIntInput("Intelligence", "CT: Intelligence", g_FeatureStatus.character_intelligence, 1, 99);
                DrawIntInput("Faith", "CT: Faith", g_FeatureStatus.character_faith, 1, 99);
                DrawIntInput("Arcane", "CT: Arcane", g_FeatureStatus.character_arcane, 1, 99);
                ImGui::Separator();
                DrawReadOnlyInt("当前 HP", g_FeatureStatus.current_hp.load(), "实时生命值。");
                DrawReadOnlyInt("最大 HP", g_FeatureStatus.max_hp.load(), "实时生命上限。");
                DrawReadOnlyInt("当前 FP", g_FeatureStatus.current_mp.load(), "实时专注值。");
                DrawReadOnlyInt("最大 FP", g_FeatureStatus.max_mp.load(), "实时专注上限。");
                DrawReadOnlyInt("当前 SP", g_FeatureStatus.current_sp.load(), "实时精力值。");
                DrawReadOnlyInt("最大 SP", g_FeatureStatus.max_sp.load(), "实时精力上限。");
                if (ImGui::Button("读取 HP / FP / SP", ImVec2(-1.0f, 0.0f))) {
                    g_FeatureStatus.resource_read_requested = true;
                }
                if (ImGui::Button("应用 HP / FP / SP", ImVec2(-1.0f, 0.0f))) {
                    g_FeatureStatus.resource_apply_requested = true;
                }
                DrawIntInput("编辑 HP", "写入当前生命值。", g_FeatureStatus.edit_hp, 0, 999999);
                DrawIntInput("编辑 FP", "写入当前专注值。", g_FeatureStatus.edit_mp, 0, 999999);
                DrawIntInput("编辑 SP", "写入当前精力值。", g_FeatureStatus.edit_sp, 0, 999999);
                ImGui::EndTabItem();
            }

            if (ImGui::BeginTabItem("装备编辑")) {
                ImGui::TextDisabled("当前装备、快捷栏、随身包和大卢恩都可以在这里搜索后直接套用。");
                int equip_slot =
                    std::clamp(g_FeatureStatus.equip_slot.load(), 0, static_cast<int>(IM_ARRAYSIZE(kEquipSlotLabels)) - 1);
                ImGui::SetNextItemWidth(-1.0f);
                if (ImGui::Combo("装备槽位", &equip_slot, kEquipSlotLabels, IM_ARRAYSIZE(kEquipSlotLabels))) {
                    g_FeatureStatus.equip_slot = equip_slot;
                }
                const CatalogKind equip_kind = GetCatalogKindForEquipSlot(equip_slot);
                DrawCatalogCombo(
                    "装备枚举选择",
                    "会根据当前槽位自动过滤成对应的中文 CT 分类。",
                    g_FeatureStatus.equip_item_id,
                    equip_kind,
                    g_EquipItemSearchBuffer
                );
                DrawReadOnlyInt(
                    "当前 CT 物品 ID",
                    DecodeCatalogRawId(g_FeatureStatus.equip_current_item_id.load()),
                    "当前槽位里实时装备物品对应的 CT 原始十进制 ID。"
                );
                DrawCatalogRawIdInput(
                    "目标 CT 物品 ID",
                    "这里输入的是中文版 CT 里的原始十进制 ID，会按槽位自动换算成内部编码。",
                    g_FeatureStatus.equip_item_id,
                    equip_kind
                );
                if (ImGui::Button("按 ID 装备到槽位", ImVec2(-1.0f, 0.0f))) {
                    g_FeatureStatus.equip_apply_requested = true;
                }
                if (ImGui::Button("卸下当前槽位", ImVec2(-1.0f, 0.0f))) {
                    g_FeatureStatus.equip_unequip_requested = true;
                }
                ImGui::TextDisabled("22-38 槽位对应快捷栏、随身包和大卢恩，也支持同样的搜索选择。");
                ImGui::EndTabItem();
            }

            if (ImGui::BeginTabItem("魔法编辑")) {
                ImGui::TextDisabled("记忆魔法编辑。当前仍以手输 ID 为主。");
                DrawReadOnlyInt(
                    "已解锁栏位数",
                    g_FeatureStatus.magic_slot_count.load(),
                    "来自 PlayerGameData + 0xA74。"
                );
                DrawReadOnlyInt(
                    "当前栏位物品 ID",
                    g_FeatureStatus.magic_current_item_id.load(),
                    "当前栏位里已记忆的魔法 ID。"
                );
                DrawIntInput("魔法栏位", "目标记忆栏位。", g_FeatureStatus.magic_slot, 0, 13);
                DrawIntInput(
                    "魔法物品 ID",
                    "完整 magic goods ID。若背包没有，会先自动发放再记忆。",
                    g_FeatureStatus.magic_item_id,
                    std::numeric_limits<int>::min(),
                    std::numeric_limits<int>::max()
                );
                if (ImGui::Button("记忆魔法", ImVec2(-1.0f, 0.0f))) {
                    g_FeatureStatus.magic_attune_requested = true;
                }
                if (ImGui::Button("移除魔法", ImVec2(-1.0f, 0.0f))) {
                    g_FeatureStatus.magic_remove_requested = true;
                }
                ImGui::EndTabItem();
            }

            if (ImGui::BeginTabItem("动作编辑")) {
                ImGui::TextDisabled("动作槽位编辑。");
                DrawReadOnlyInt(
                    "当前动作 ID",
                    g_FeatureStatus.gesture_current_item_id.load(),
                    "当前动作槽里实时存放的 gesture ID。"
                );
                DrawIntInput("动作槽位", "界面里 0-6 对应手势 1-7。", g_FeatureStatus.gesture_slot, 0, 6);
                DrawIntInput(
                    "动作 ID",
                    "把选中的动作槽改成这个 gesture ID。",
                    g_FeatureStatus.gesture_item_id,
                    0,
                    999999999
                );
                if (ImGui::Button("应用动作槽位", ImVec2(-1.0f, 0.0f))) {
                    g_FeatureStatus.gesture_apply_requested = true;
                }
                ImGui::TextDisabled("这里的槽位 0 对应存档里的 Gesture 1。");
                ImGui::EndTabItem();
            }

            if (ImGui::BeginTabItem("NPC")) {
                ImGui::TextDisabled("对应 CT: Targeted Npc Info / Last Hit Npc Info。用于快速确认目标 NPC 的 ID、阵营、血量和坐标。");
                ImGui::Spacing();
                DrawNpcInfoSection(
                    "Targeted Npc",
                    "当前没有捕获到锁定中的 NPC。先锁定目标，或等到 HUD 里出现锁定框后再看这里。",
                    g_FeatureStatus.targeted_npc_valid,
                    g_FeatureStatus.targeted_npc_address,
                    g_FeatureStatus.targeted_npc_handle,
                    g_FeatureStatus.targeted_npc_id,
                    g_FeatureStatus.targeted_npc_character_type,
                    g_FeatureStatus.targeted_npc_team_type,
                    g_FeatureStatus.targeted_npc_hp,
                    g_FeatureStatus.targeted_npc_max_hp,
                    g_FeatureStatus.targeted_npc_x,
                    g_FeatureStatus.targeted_npc_y,
                    g_FeatureStatus.targeted_npc_z,
                    g_FeatureStatus.teleport_self_to_targeted_npc_requested,
                    g_FeatureStatus.teleport_targeted_npc_to_self_requested
                );
                DrawNpcInfoSection(
                    "Last Hit Npc",
                    "当前没有捕获到最近一次命中的 NPC。先造成一次命中，再回到这里查看。",
                    g_FeatureStatus.last_hit_npc_valid,
                    g_FeatureStatus.last_hit_npc_address,
                    g_FeatureStatus.last_hit_npc_handle,
                    g_FeatureStatus.last_hit_npc_id,
                    g_FeatureStatus.last_hit_npc_character_type,
                    g_FeatureStatus.last_hit_npc_team_type,
                    g_FeatureStatus.last_hit_npc_hp,
                    g_FeatureStatus.last_hit_npc_max_hp,
                    g_FeatureStatus.last_hit_npc_x,
                    g_FeatureStatus.last_hit_npc_y,
                    g_FeatureStatus.last_hit_npc_z,
                    g_FeatureStatus.teleport_self_to_last_hit_npc_requested,
                    g_FeatureStatus.teleport_last_hit_npc_to_self_requested
                );
                ImGui::EndTabItem();
            }

            if (ImGui::BeginTabItem("动作 / 调试")) {
                DrawOneShotButton("开始下一周目", "向 GameMan 写入 NG+ 请求。建议只在主线通关后手动点击。", g_FeatureStatus.start_next_cycle, "未触发", "已触发");
                ImGui::Separator();
                DrawIntInput("卢恩数量", "对应 CT: Give Runes。输入后点击下方按钮，建议按存档进度谨慎发放。", g_FeatureStatus.give_runes_amount, 1, 999999999);
                if (ImGui::Button("发放卢恩", ImVec2(-1.0f, 0.0f))) {
                    g_FeatureStatus.give_runes_requested = true;
                }
                ImGui::TextDisabled("一次性动作：会直接调用 PlayerGameData 的加魂函数。");
                ImGui::Spacing();

                int flask_type = std::clamp(g_FeatureStatus.add_flask_charge_type.load(), 0, 1);
                const char* flask_type_labels[] = {"红露滴圣杯瓶", "蓝露滴圣杯瓶"};
                ImGui::SetNextItemWidth(-1.0f);
                if (ImGui::Combo("补充圣杯瓶类型", &flask_type, flask_type_labels, IM_ARRAYSIZE(flask_type_labels))) {
                    g_FeatureStatus.add_flask_charge_type = flask_type;
                }
                ImGui::TextDisabled("对应 CT: Add charge to flask。总次数到 14 以上时游戏会拒绝。");
                if (ImGui::Button("增加一次圣杯瓶充能", ImVec2(-1.0f, 0.0f))) {
                    g_FeatureStatus.add_flask_charge_requested = true;
                }
                ImGui::Spacing();

                DrawIntInput("圣杯瓶强化等级", "对应 CT: Set flask level。范围 0-12，输入的是 + 后面的数值。", g_FeatureStatus.set_flask_level_value, 0, 12);
                if (ImGui::Button("设置圣杯瓶强化等级", ImVec2(-1.0f, 0.0f))) {
                    g_FeatureStatus.set_flask_level_requested = true;
                }
                ImGui::TextDisabled("会同步替换红蓝瓶道具 ID，并补一个保存请求。");
                ImGui::Spacing();

                ImGui::Separator();
                ImGui::TextUnformatted("Event Flag by ID");
                DrawIntInput("Event Flag ID", "对应 CT: Event Flags by ID。输入 flag ID 后可读取或写入。", g_FeatureStatus.debug_event_flag_id, 0, 2000000000);
                DrawBooleanToggle("目标状态", "勾选 = ON，取消勾选 = OFF。点击“写入”时会把这个状态应用到上方 ID。", g_FeatureStatus.debug_event_flag_desired_state);

                const int flag_state = g_FeatureStatus.debug_event_flag_current_state.load();
                const char* flag_state_text = "未读取";
                ImVec4 flag_state_color = ImVec4(0.95f, 0.8f, 0.2f, 1.0f);
                if (flag_state == 0) {
                    flag_state_text = "当前状态: OFF";
                    flag_state_color = ImVec4(0.95f, 0.35f, 0.35f, 1.0f);
                } else if (flag_state == 1) {
                    flag_state_text = "当前状态: ON";
                    flag_state_color = ImVec4(0.25f, 0.9f, 0.45f, 1.0f);
                }
                ImGui::TextColored(flag_state_color, "%s", flag_state_text);

                if (ImGui::Button("读取 Event Flag", ImVec2(-1.0f, 0.0f))) {
                    g_FeatureStatus.debug_event_flag_read_requested = true;
                }
                if (ImGui::Button("写入 Event Flag", ImVec2(-1.0f, 0.0f))) {
                    g_FeatureStatus.debug_event_flag_write_requested = true;
                }
                ImGui::TextDisabled("适合用来补流程、调试分支或对照 CT 里的 flag 页。");
                ImGui::Separator();
                ImGui::Text("叠加层状态：%s", g_FeatureStatus.overlay_ready.load() ? "已就绪" : "初始化中");
                ImGui::Text("游戏状态：%s", g_FeatureStatus.game_ready.load() ? "角色已加载" : "等待进档");
                ImGui::TextDisabled("资源动作和 Event Flag 调试已经接入；NPC 定位和更多 CT 页面还会继续补。");
                ImGui::EndTabItem();
            }

            if (ImGui::BeginTabItem("Hero / 库存")) {
                ImGui::Text("角色名: %s", inspector.character_name.empty() ? "未知" : inspector.character_name.c_str());
                ImGui::Text("等级: %d | 周目: %d | 卢恩弧: %d", inspector.level, inspector.clear_count, inspector.rune_arc);
                ImGui::Text("当前武器槽: 左 %d | 右 %d", inspector.current_weapon_slot_left, inspector.current_weapon_slot_right);
                ImGui::Text("幽影树庇佑: %d | 灵灰庇佑: %d", inspector.scadutree_blessing, inspector.revered_spirit_ash_blessing);
                ImGui::Separator();
                ImGui::TextUnformatted("当前装备与快捷栏");
                DrawCompactItemLine("左手槽 1", inspector.equipped_weapons[0]);
                DrawCompactItemLine("右手槽 1", inspector.equipped_weapons[1]);
                DrawCompactItemLine("左手槽 2", inspector.equipped_weapons[2]);
                DrawCompactItemLine("右手槽 2", inspector.equipped_weapons[3]);
                DrawCompactItemLine("左手槽 3", inspector.equipped_weapons[4]);
                DrawCompactItemLine("右手槽 3", inspector.equipped_weapons[5]);
                ImGui::Separator();
                if (ImGui::BeginTabBar("HeroInventoryTabs")) {
                    if (ImGui::BeginTabItem("快捷栏")) {
                        for (std::size_t index = 0; index < inspector.quick_items.size(); ++index) {
                            const std::string label = "快捷栏 " + std::to_string(index + 1);
                            DrawCompactItemLine(label.c_str(), inspector.quick_items[index]);
                        }
                        ImGui::Separator();
                        for (std::size_t index = 0; index < inspector.pouch_items.size(); ++index) {
                            const std::string label = "随身包 " + std::to_string(index + 1);
                            DrawCompactItemLine(label.c_str(), inspector.pouch_items[index]);
                        }
                        ImGui::EndTabItem();
                    }
                    if (ImGui::BeginTabItem("武器库存")) {
                        DrawInventoryList("hero_weapon_inventory", inspector.weapon_inventory, g_WeaponInventorySearchBuffer);
                        ImGui::EndTabItem();
                    }
                    if (ImGui::BeginTabItem("防具库存")) {
                        DrawInventoryList("hero_protector_inventory", inspector.protector_inventory, g_ProtectorInventorySearchBuffer);
                        ImGui::EndTabItem();
                    }
                    if (ImGui::BeginTabItem("护符库存")) {
                        DrawInventoryList("hero_accessory_inventory", inspector.accessory_inventory, g_AccessoryInventorySearchBuffer);
                        ImGui::EndTabItem();
                    }
                    if (ImGui::BeginTabItem("道具库存")) {
                        DrawInventoryList("hero_goods_inventory", inspector.goods_inventory, g_GoodsInventorySearchBuffer);
                        ImGui::EndTabItem();
                    }
                    ImGui::EndTabBar();
                }
                ImGui::EndTabItem();
            }

            if (ImGui::BeginTabItem("统计 / 会话")) {
                ImGui::Text("HP: %d / %d | FP: %d / %d | SP: %d / %d",
                            inspector.current_hp,
                            inspector.max_hp,
                            inspector.current_fp,
                            inspector.max_fp,
                            inspector.current_sp,
                            inspector.max_sp);
                ImGui::Text("角色类型: %s (%d) | 队伍类型: %s (%d)",
                            CharacterTypeLabel(inspector.character_type),
                            inspector.character_type,
                            TeamTypeLabel(inspector.team_type),
                            inspector.team_type);
                ImGui::Text("当前存档槽: %d | 待载入槽: %d", inspector.save_slot, inspector.pending_load_save_slot);
                ImGui::Text("最后赐福 ID: %d | 大卢恩 ID: %d", inspector.last_grace, inspector.great_rune);
                ImGui::Separator();
                DrawIntInput("载入存档槽", "写入 GameMan 的待载入槽位。", g_FeatureStatus.statistics_load_save_slot, 0, 9);
                if (ImGui::Button("请求载入该存档槽", ImVec2(-1.0f, 0.0f))) {
                    g_FeatureStatus.statistics_load_save_slot_requested = true;
                }
                if (ImGui::Button("请求保存游戏", ImVec2(-1.0f, 0.0f))) {
                    g_FeatureStatus.statistics_save_game_requested = true;
                }
                bool disable_auto_save = g_FeatureStatus.statistics_disable_auto_save.load();
                if (ImGui::Checkbox("禁用自动保存", &disable_auto_save)) {
                    g_FeatureStatus.statistics_disable_auto_save = disable_auto_save;
                    g_FeatureStatus.statistics_auto_save_dirty = true;
                }
                ImGui::TextDisabled("当前自动保存状态: %s", inspector.disable_auto_save ? "已禁用" : "启用");
                ImGui::Separator();
                DrawSessionPlayerCard("本地玩家", inspector.local_player);
                for (std::size_t index = 0; index < inspector.net_players.size(); ++index) {
                    const std::string title = "联机槽位 " + std::to_string(index + 1);
                    DrawSessionPlayerCard(title.c_str(), inspector.net_players[index]);
                }
                ImGui::Separator();
                ImGui::Text("白灵: %d | 红灵: %d | 侦探灵: %d",
                            inspector.party_white_ghosts,
                            inspector.party_black_ghosts,
                            inspector.party_detective_ghosts);
                ImGui::Text("NPC 队友: %d | 已创建成员: %d | 已连接成员: %d",
                            inspector.party_npc_members,
                            inspector.party_members_created,
                            inspector.party_connected_members);
                for (std::size_t index = 0; index < inspector.party_slots.size(); ++index) {
                    DrawPartyMemberCard(static_cast<int>(index), inspector.party_slots[index]);
                }
                ImGui::EndTabItem();
            }

            if (ImGui::BeginTabItem("参数工具")) {
                if (inspector.param_tables.empty()) {
                    ImGui::TextDisabled("暂时没有拿到 Param 表。");
                } else {
                    int selected_table = std::clamp(
                        g_FeatureStatus.param_tool_selected_table.load(),
                        0,
                        static_cast<int>(inspector.param_tables.size()) - 1
                    );
                    const char* preview = inspector.param_tables[static_cast<std::size_t>(selected_table)].name_utf8.c_str();
                    if (ImGui::BeginCombo("Param 表", preview)) {
                        for (std::size_t index = 0; index < inspector.param_tables.size(); ++index) {
                            const bool selected = static_cast<int>(index) == selected_table;
                            const std::string label =
                                inspector.param_tables[index].name_utf8 +
                                " (" + std::to_string(inspector.param_tables[index].row_count) + ")";
                            if (ImGui::Selectable(label.c_str(), selected)) {
                                g_FeatureStatus.param_tool_selected_table = static_cast<int>(index);
                            }
                            if (selected) {
                                ImGui::SetItemDefaultFocus();
                            }
                        }
                        ImGui::EndCombo();
                    }

                    DrawIntInput("Row ID", "目标 param row id。", g_FeatureStatus.param_tool_row_id, 0, std::numeric_limits<int>::max());
                    DrawIntInput("字段偏移", "目标字段 offset，单位字节。", g_FeatureStatus.param_tool_field_offset, 0, 0x7FFFFFFF);
                    int value_type = std::clamp(g_FeatureStatus.param_tool_value_type.load(), 0, 6);
                    const char* value_type_labels[] = {"s8", "u8", "s16", "u16", "s32", "u32", "f32"};
                    if (ImGui::Combo("字段类型", &value_type, value_type_labels, IM_ARRAYSIZE(value_type_labels))) {
                        g_FeatureStatus.param_tool_value_type = value_type;
                    }

                    if (inspector.param_preview.valid) {
                        ImGui::Text("当前值: %s", inspector.param_preview.value_text.c_str());
                        ImGui::TextDisabled("预览类型: %s | RowAddr: 0x%p",
                                            inspector.param_preview.type_text.c_str(),
                                            reinterpret_cast<void*>(inspector.param_preview.row_address));
                    } else {
                        ImGui::TextDisabled("当前 row / offset 还没有取到可读值。");
                    }

                    DrawIntInput("整数写入值", "当字段类型是整数时使用。", g_FeatureStatus.param_tool_int_value, std::numeric_limits<int>::min(), std::numeric_limits<int>::max());
                    DrawFloatInput("浮点写入值", "当字段类型是 f32 时使用。", g_FeatureStatus.param_tool_float_value, -1000000.0f, 1000000.0f, 0.1f, "%.4f");
                    if (ImGui::Button("写入参数值", ImVec2(-1.0f, 0.0f))) {
                        g_FeatureStatus.param_tool_write_requested = true;
                    }
                }

                ImGui::Separator();
                ImGui::TextUnformatted("脚本化改参");
                if (!inspector.param_script_error.empty()) {
                    ImGui::TextDisabled("%s", inspector.param_script_error.c_str());
                }
                std::string current_group;
                for (const Features::ParamScriptSnapshot& script : inspector.param_scripts) {
                    if (current_group != script.group) {
                        current_group = script.group;
                        ImGui::Separator();
                        ImGui::TextUnformatted(current_group.c_str());
                    }
                    if (ImGui::Button(script.label.c_str(), ImVec2(-1.0f, 0.0f))) {
                        g_FeatureStatus.param_tool_script_request_id = script.id;
                    }
                    if (!script.description.empty()) {
                        ImGui::TextDisabled("%s", script.description.c_str());
                    }
                }
                ImGui::EndTabItem();
            }

            if (ImGui::BeginTabItem("NPC 菜单 / 商店")) {
                const int last_menu_result = g_FeatureStatus.npc_menu_last_result.load();
                if (last_menu_result > 0) {
                    ImGui::TextColored(ImVec4(0.25f, 0.9f, 0.45f, 1.0f), "上一次菜单调用结果: 成功");
                } else if (last_menu_result < 0) {
                    ImGui::TextColored(ImVec4(0.95f, 0.35f, 0.35f, 1.0f), "上一次菜单调用结果: 失败");
                } else {
                    ImGui::TextDisabled("还没有触发过 NPC 菜单。");
                }
                if (!inspector.npc_menu_error.empty()) {
                    ImGui::TextDisabled("%s", inspector.npc_menu_error.c_str());
                }

                std::string current_category;
                for (const Features::NpcMenuEntrySnapshot& entry : inspector.npc_menus) {
                    if (current_category != entry.category) {
                        current_category = entry.category;
                        ImGui::Separator();
                        ImGui::TextUnformatted(current_category.c_str());
                    }
                    if (ImGui::Button(entry.label.c_str(), ImVec2(-1.0f, 0.0f))) {
                        g_FeatureStatus.npc_menu_request_id = entry.id;
                    }
                    if (!entry.description.empty()) {
                        ImGui::TextDisabled("%s", entry.description.c_str());
                    }
                }
                ImGui::EndTabItem();
            }

            if (ImGui::BeginTabItem("系统调试")) {
                ImGui::Text("Overlay: %s | GameReady: %s",
                            g_FeatureStatus.overlay_ready.load() ? "Ready" : "Init",
                            inspector.game_ready ? "Yes" : "No");
                ImGui::Separator();
                ImGui::Text("WorldChrMan: 0x%p", reinterpret_cast<void*>(inspector.world_chr_man));
                ImGui::Text("GameMan: 0x%p", reinterpret_cast<void*>(inspector.game_man));
                ImGui::Text("GameDataMan: 0x%p", reinterpret_cast<void*>(inspector.game_data_man));
                ImGui::Text("CSMenuMan: 0x%p", reinterpret_cast<void*>(inspector.cs_menu_man));
                ImGui::Text("CSRegulationManager: 0x%p", reinterpret_cast<void*>(inspector.cs_regulation_manager));
                ImGui::Separator();
                ImGui::Text("Targeted NPC: %s | Last Hit NPC: %s",
                            g_FeatureStatus.targeted_npc_valid.load() ? "有效" : "无",
                            g_FeatureStatus.last_hit_npc_valid.load() ? "有效" : "无");
                if (g_FeatureStatus.targeted_npc_valid.load()) {
                    ImGui::Text("Targeted ID: %d | HP: %d / %d",
                                g_FeatureStatus.targeted_npc_id.load(),
                                g_FeatureStatus.targeted_npc_hp.load(),
                                g_FeatureStatus.targeted_npc_max_hp.load());
                }
                if (g_FeatureStatus.last_hit_npc_valid.load()) {
                    ImGui::Text("LastHit ID: %d | HP: %d / %d",
                                g_FeatureStatus.last_hit_npc_id.load(),
                                g_FeatureStatus.last_hit_npc_hp.load(),
                                g_FeatureStatus.last_hit_npc_max_hp.load());
                }
                ImGui::Separator();
                ImGui::TextDisabled("这里汇总了汉化 CT 里的 Statistics / Session Info / Party Member Info / Debug Stuff。");
                ImGui::EndTabItem();
            }

            ImGui::EndTabBar();
        }
    }
    ImGui::End();

    g_FeatureStatus.menu_visible.store(menu_visible);
}

}  // namespace ERD::Main
