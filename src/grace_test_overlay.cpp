#include "grace_test_overlay.hpp"

#include "Common.hpp"
#include "Features/PostureBars.hpp"
#include "Main/FeatureStatus.hpp"

#include "ThirdParty/ImGui/imgui.h"
#include "ThirdParty/ImGui/imgui_impl_dx12.h"
#include "ThirdParty/ImGui/imgui_impl_win32.h"
#include "ThirdParty/MinHook/MinHook.h"

#include <d3d12.h>
#include <dxgi1_4.h>

#include <algorithm>
#include <cmath>
#include <cstdlib>
#include <cstdio>
#include <memory>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include <spdlog/spdlog.h>

namespace grace_test::overlay {
namespace {

class D3D12PostureOverlay;
std::unique_ptr<D3D12PostureOverlay> g_overlay;

inline constexpr std::uint16_t kExecuteCommandListsIndex = 54;
inline constexpr std::uint16_t kPresentIndex = 140;
inline constexpr std::uint16_t kResizeBuffersIndex = 145;

struct GameViewportTransform {
    float scale = 1.0f;
    ImVec2 offset{};
};

struct DamagePopupEntry {
    std::uint64_t handle = 0;
    int damage = 0;
    ULONGLONG spawn_tick = 0;
};

struct AnchorState {
    ImVec2 position{};
    ULONGLONG tick = 0;
};

struct TrackedNpcState {
    std::uint64_t handle = 0;
    int previous_hp = -1;
};

std::unordered_map<std::uint64_t, int> g_PreviousHpByHandle;
std::unordered_map<std::uint64_t, AnchorState> g_LastAnchorByHandle;
std::vector<DamagePopupEntry> g_DamagePopups;
TrackedNpcState g_TargetedTracker{};
TrackedNpcState g_LastHitTracker{};

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

ImU32 GetPostureBarOrange() {
    return IM_COL32(230, 126, 34, 235);
}

ImU32 GetSoulBorderOuter() {
    return IM_COL32(134, 92, 42, 240);
}

ImU32 GetSoulBorderInner() {
    return IM_COL32(52, 38, 20, 230);
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
        draw_list->AddRectFilled(min, ImVec2(min.x + size.x * clamped_ratio, max.y), fill_color, rounding);
    }
    draw_list->AddRect(min, max, GetSoulBorderOuter(), rounding, 0, 1.8f);
    draw_list->AddRect(
        ImVec2(min.x + 1.0f, min.y + 1.0f),
        ImVec2(max.x - 1.0f, max.y - 1.0f),
        GetSoulBorderInner(),
        std::max(1.0f, rounding - 1.0f),
        0,
        1.0f);
}

int GetDamageTier(int damage) {
    if (damage <= 0) {
        return 0;
    }
    if (damage < 100) {
        return 1;
    }
    if (damage < 200) {
        return 2;
    }
    if (damage < 350) {
        return 3;
    }
    if (damage < 550) {
        return 4;
    }
    if (damage < 800) {
        return 5;
    }
    if (damage < 1200) {
        return 6;
    }
    if (damage < 1700) {
        return 7;
    }
    if (damage < 3000) {
        return 8;
    }
    if (damage < 5000) {
        return 9;
    }
    return 10;
}

float GetDamageFontSize(int tier, float scale) {
    static constexpr float kFontSizes[] = {
        0.0f,
        18.0f,
        20.0f,
        22.0f,
        24.0f,
        26.0f,
        28.0f,
        31.0f,
        35.0f,
        40.0f,
        46.0f,
    };
    return kFontSizes[std::clamp(tier, 1, 10)] * scale;
}

ImU32 GetDamageTextColor(int tier, int alpha) {
    static constexpr ImU32 kColors[] = {
        0,
        IM_COL32(255, 225, 155, 255),
        IM_COL32(255, 212, 122, 255),
        IM_COL32(255, 196, 102, 255),
        IM_COL32(255, 176, 88, 255),
        IM_COL32(255, 154, 74, 255),
        IM_COL32(255, 128, 62, 255),
        IM_COL32(255, 102, 56, 255),
        IM_COL32(255, 88, 54, 255),
        IM_COL32(255, 120, 92, 255),
        IM_COL32(255, 238, 214, 255),
    };

    const ImU32 rgb = kColors[std::clamp(tier, 1, 10)] & 0x00FFFFFF;
    return rgb | (static_cast<ImU32>(std::clamp(alpha, 0, 255)) << 24);
}

void PushDamagePopup(std::uint64_t handle, int damage) {
    if (handle == 0 || damage <= 0) {
        return;
    }

    g_DamagePopups.push_back(DamagePopupEntry{
        handle,
        damage,
        GetTickCount64(),
    });

    if (g_DamagePopups.size() > 96) {
        g_DamagePopups.erase(g_DamagePopups.begin(), g_DamagePopups.begin() + (g_DamagePopups.size() - 96));
    }
}

void TrackDamageForHandle(std::uint64_t handle, int hp) {
    if (handle == 0 || hp <= 0) {
        return;
    }

    const auto found = g_PreviousHpByHandle.find(handle);
    if (found != g_PreviousHpByHandle.end() && hp < found->second) {
        PushDamagePopup(handle, found->second - hp);
    }

    g_PreviousHpByHandle[handle] = hp;
}

void UpdateTrackedNpcState(TrackedNpcState& tracker, bool valid, std::uint64_t handle, int hp) {
    if (!valid || handle == 0) {
        if (tracker.handle != 0 && tracker.previous_hp > 0) {
            PushDamagePopup(tracker.handle, tracker.previous_hp);
        }
        tracker = {};
        return;
    }

    if (tracker.handle != handle) {
        tracker.handle = handle;
        tracker.previous_hp = hp;
        return;
    }

    if (hp <= 0 && tracker.previous_hp > 0) {
        PushDamagePopup(handle, tracker.previous_hp);
        tracker.previous_hp = 0;
        return;
    }

    tracker.previous_hp = hp;
}

void PruneExpiredDamagePopups() {
    constexpr ULONGLONG kLifetimeMs = 5000ULL;
    const ULONGLONG now = GetTickCount64();

    g_DamagePopups.erase(
        std::remove_if(
            g_DamagePopups.begin(),
            g_DamagePopups.end(),
            [now](const DamagePopupEntry& entry) {
                return now - entry.spawn_tick > kLifetimeMs;
            }),
        g_DamagePopups.end());

    for (auto it = g_LastAnchorByHandle.begin(); it != g_LastAnchorByHandle.end();) {
        if (now - it->second.tick > 6000ULL) {
            it = g_LastAnchorByHandle.erase(it);
        } else {
            ++it;
        }
    }
}

void DrawDamagePopups(ImDrawList* draw_list, const ImVec2& anchor, std::uint64_t handle, float scale) {
    if (draw_list == nullptr || handle == 0) {
        return;
    }

    constexpr ULONGLONG kLifetimeMs = 5000ULL;
    const ULONGLONG now = GetTickCount64();
    std::size_t stack_index = 0;

    for (auto it = g_DamagePopups.rbegin(); it != g_DamagePopups.rend(); ++it) {
        if (it->handle != handle) {
            continue;
        }

        const ULONGLONG age_ms = now - it->spawn_tick;
        if (age_ms > kLifetimeMs) {
            continue;
        }

        const float progress = static_cast<float>(age_ms) / static_cast<float>(kLifetimeMs);
        const float eased = 1.0f - std::pow(1.0f - progress, 2.0f);
        const int tier = GetDamageTier(it->damage);
        const float font_size = GetDamageFontSize(tier, scale);
        const float rise = (30.0f + tier * 5.0f) * scale * eased;
        const float stack_offset = static_cast<float>(stack_index) * (font_size * 0.78f);
        const int alpha = static_cast<int>(255.0f * (1.0f - progress));
        const float glow_alpha = std::clamp((1.0f - progress) * 0.75f, 0.0f, 1.0f);

        char text[32]{};
        sprintf_s(text, "-%d", it->damage);

        ImFont* font = ImGui::GetFont();
        const ImVec2 text_size = font->CalcTextSizeA(font_size, FLT_MAX, 0.0f, text);
        const ImVec2 pos(
            anchor.x - text_size.x * 0.5f,
            anchor.y - rise - stack_offset - font_size);

        const ImU32 text_color = GetDamageTextColor(tier, alpha);
        const ImU32 outline_color = IM_COL32(18, 10, 6, std::min(235, alpha));
        const ImU32 glow_color = IM_COL32(255, 155, 80, static_cast<int>(140.0f * glow_alpha));
        const float outline_offset = 1.2f + static_cast<float>(tier) * 0.12f;

        draw_list->AddText(font, font_size, ImVec2(pos.x + outline_offset, pos.y + outline_offset), outline_color, text);
        draw_list->AddText(font, font_size, ImVec2(pos.x - outline_offset, pos.y), outline_color, text);
        draw_list->AddText(font, font_size, ImVec2(pos.x, pos.y - outline_offset), outline_color, text);
        draw_list->AddText(font, font_size, ImVec2(pos.x, pos.y + 3.0f * scale), glow_color, text);
        draw_list->AddText(font, font_size, pos, text_color, text);

        ++stack_index;
        if (stack_index >= 5) {
            break;
        }
    }
}

void DrawIntegratedPostureBars() {
    const bool show_posture_bars = ERD::Main::g_FeatureStatus.posture_bar_overlay_enabled.load();
    const bool show_damage_popups = ERD::Main::g_FeatureStatus.damage_popup_overlay_enabled.load();
    if (!show_posture_bars && !show_damage_popups) {
        return;
    }

    const ERD::Features::PostureBarsSnapshot snapshot = ERD::Features::GetPostureBarsSnapshot();
    if (!snapshot.hook_installed) {
        return;
    }

    ImDrawList* draw_list = ImGui::GetBackgroundDrawList();
    if (draw_list == nullptr) {
        return;
    }

    PruneExpiredDamagePopups();
    UpdateTrackedNpcState(
        g_TargetedTracker,
        ERD::Main::g_FeatureStatus.targeted_npc_valid.load(),
        static_cast<std::uint64_t>(ERD::Main::g_FeatureStatus.targeted_npc_handle.load()),
        ERD::Main::g_FeatureStatus.targeted_npc_hp.load());
    UpdateTrackedNpcState(
        g_LastHitTracker,
        ERD::Main::g_FeatureStatus.last_hit_npc_valid.load(),
        static_cast<std::uint64_t>(ERD::Main::g_FeatureStatus.last_hit_npc_handle.load()),
        ERD::Main::g_FeatureStatus.last_hit_npc_hp.load());
    const GameViewportTransform transform = BuildGameViewportTransform();
    std::unordered_set<std::uint64_t> drawn_popup_handles;

    for (const ERD::Features::PostureBarEntry& boss_bar : snapshot.boss_bars) {
        if (!boss_bar.visible || boss_bar.maximum <= 0.0f) {
            continue;
        }

        const ImVec2 center = ToViewportPosition(transform, boss_bar.screen_x, boss_bar.screen_y);
        const ImVec2 size(998.0f * transform.scale, std::max(8.0f, 16.0f * transform.scale));
        TrackDamageForHandle(boss_bar.handle, boss_bar.hp);
        g_LastAnchorByHandle[boss_bar.handle] = AnchorState{ImVec2(center.x, center.y - 8.0f * transform.scale), GetTickCount64()};
        if (show_posture_bars) {
            DrawSimplePostureBar(draw_list, center, size, boss_bar.current / boss_bar.maximum, GetPostureBarOrange());
        }
        if (show_damage_popups) {
            DrawDamagePopups(draw_list, ImVec2(center.x, center.y - 8.0f * transform.scale), boss_bar.handle, transform.scale);
        }
        drawn_popup_handles.insert(boss_bar.handle);
    }

    for (const ERD::Features::PostureBarEntry& entity_bar : snapshot.entity_bars) {
        if (!entity_bar.visible || entity_bar.maximum <= 0.0f) {
            continue;
        }

        const float clamped_game_x = std::clamp(entity_bar.screen_x, 130.0f, 1790.0f) - 1.0f;
        const float clamped_game_y = std::clamp(entity_bar.screen_y, 175.0f, 990.0f) - 10.0f;
        const ImVec2 center = ToViewportPosition(transform, clamped_game_x, clamped_game_y);
        const ImVec2 size(
            std::max(60.0f, 138.0f * entity_bar.distance_modifier) * transform.scale,
            std::max(4.0f, 5.0f * transform.scale));
        TrackDamageForHandle(entity_bar.handle, entity_bar.hp);
        g_LastAnchorByHandle[entity_bar.handle] = AnchorState{ImVec2(center.x, center.y - 4.0f * transform.scale), GetTickCount64()};
        if (show_posture_bars) {
            DrawSimplePostureBar(draw_list, center, size, entity_bar.current / entity_bar.maximum, GetPostureBarOrange());
        }
        if (show_damage_popups) {
            DrawDamagePopups(draw_list, ImVec2(center.x, center.y - 4.0f * transform.scale), entity_bar.handle, transform.scale);
        }
        drawn_popup_handles.insert(entity_bar.handle);
    }

    if (!show_damage_popups) {
        return;
    }

    for (const DamagePopupEntry& entry : g_DamagePopups) {
        if (entry.handle == 0 || drawn_popup_handles.contains(entry.handle)) {
            continue;
        }

        const auto anchor_it = g_LastAnchorByHandle.find(entry.handle);
        if (anchor_it == g_LastAnchorByHandle.end()) {
            continue;
        }

        DrawDamagePopups(draw_list, anchor_it->second.position, entry.handle, transform.scale);
        drawn_popup_handles.insert(entry.handle);
    }
}

HWND FindGameWindow() {
    HWND window = FindWindowW(nullptr, L"ELDEN RING");
    if (window == nullptr) {
        window = GetForegroundWindow();
    }
    return window;
}

class D3D12PostureOverlay {
public:
    ~D3D12PostureOverlay() noexcept {
        Unhook();
    }

    void Tick() {
        if (hooked_) {
            return;
        }

        if (!Hook() && !hook_failure_logged_) {
            SPDLOG_ERROR("Failed to initialize DX12 posture overlay hook.");
            hook_failure_logged_ = true;
        }
    }

    void Unhook() {
        ERD::Main::g_FeatureStatus.overlay_ready = false;

        disable_hook(present_target_);
        disable_hook(execute_command_lists_target_);
        disable_hook(resize_buffers_target_);

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

        hooked_ = false;
    }

private:
    struct FrameContext {
        ID3D12CommandAllocator* allocator = nullptr;
        ID3D12Resource* back_buffer = nullptr;
        D3D12_CPU_DESCRIPTOR_HANDLE rtv_handle{};
        UINT64 fence_value = 0;
    };

    using PresentFn = HRESULT(APIENTRY*)(IDXGISwapChain* swap_chain, UINT sync_interval, UINT flags);
    using ExecuteCommandListsFn = void(APIENTRY*)(
        ID3D12CommandQueue* queue,
        UINT num_command_lists,
        ID3D12CommandList* const* command_lists);
    using ResizeBuffersFn = HRESULT(APIENTRY*)(
        IDXGISwapChain* swap_chain,
        UINT buffer_count,
        UINT width,
        UINT height,
        DXGI_FORMAT new_format,
        UINT flags);

    static HRESULT APIENTRY HookPresent(IDXGISwapChain* swap_chain, UINT sync_interval, UINT flags) {
        if (g_overlay != nullptr) {
            g_overlay->Render(swap_chain);
            if (g_overlay->original_present_ != nullptr) {
                return g_overlay->original_present_(swap_chain, sync_interval, flags);
            }
        }
        return S_OK;
    }

    static void APIENTRY HookExecuteCommandLists(
        ID3D12CommandQueue* queue,
        UINT num_command_lists,
        ID3D12CommandList* const* command_lists) {
        if (g_overlay != nullptr && g_overlay->command_queue_ == nullptr) {
            if (queue->GetDesc().Type == D3D12_COMMAND_LIST_TYPE_DIRECT) {
                g_overlay->command_queue_ = queue;
            }
        }

        if (g_overlay != nullptr && g_overlay->original_execute_command_lists_ != nullptr) {
            g_overlay->original_execute_command_lists_(queue, num_command_lists, command_lists);
        }
    }

    static HRESULT APIENTRY HookResizeBuffers(
        IDXGISwapChain* swap_chain,
        UINT buffer_count,
        UINT width,
        UINT height,
        DXGI_FORMAT new_format,
        UINT flags) {
        if (g_overlay != nullptr) {
            g_overlay->ResetRenderState();
            if (g_overlay->original_resize_buffers_ != nullptr) {
                return g_overlay->original_resize_buffers_(swap_chain, buffer_count, width, height, new_format, flags);
            }
        }
        return S_OK;
    }

    bool Hook() {
        if (hooked_) {
            return true;
        }

        if (!InitializeHook()) {
            return false;
        }

        if (!CreateHook(
                kExecuteCommandListsIndex,
                execute_command_lists_target_,
                reinterpret_cast<void**>(&original_execute_command_lists_),
                reinterpret_cast<void*>(&HookExecuteCommandLists)) ||
            !CreateHook(
                kPresentIndex,
                present_target_,
                reinterpret_cast<void**>(&original_present_),
                reinterpret_cast<void*>(&HookPresent)) ||
            !CreateHook(
                kResizeBuffersIndex,
                resize_buffers_target_,
                reinterpret_cast<void**>(&original_resize_buffers_),
                reinterpret_cast<void*>(&HookResizeBuffers))) {
            return false;
        }

        hooked_ = true;
        SPDLOG_INFO("DX12 posture overlay hook installed.");
        return true;
    }

    bool InitializeHook() {
        game_window_ = FindGameWindow();
        if (game_window_ == nullptr) {
            return false;
        }

        window_class_.cbSize = sizeof(WNDCLASSEXW);
        window_class_.style = CS_HREDRAW | CS_VREDRAW;
        window_class_.lpfnWndProc = DefWindowProcW;
        window_class_.hInstance = GetModuleHandleW(nullptr);
        window_class_.lpszClassName = L"erdGameToolsDX12DummyWindow";
        RegisterClassExW(&window_class_);

        dummy_window_ = CreateWindowW(
            window_class_.lpszClassName,
            L"erdGameTools Dummy Window",
            WS_OVERLAPPEDWINDOW,
            0,
            0,
            100,
            100,
            nullptr,
            nullptr,
            window_class_.hInstance,
            nullptr);
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
                IID_PPV_ARGS(&command_list)) != S_OK) {
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

    bool InitializeRenderer(IDXGISwapChain* swap_chain) {
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
                IID_PPV_ARGS(&command_list_)) != S_OK) {
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
        io.ConfigFlags |= ImGuiConfigFlags_NoMouseCursorChange;
        io.Fonts->AddFontDefault();

        if (!ImGui_ImplWin32_Init(game_window_)) {
            return false;
        }

        if (!ImGui_ImplDX12_Init(
                device_,
                static_cast<int>(swap_chain_desc.BufferCount),
                swap_chain_desc.BufferDesc.Format,
                srv_descriptor_heap_,
                srv_descriptor_heap_->GetCPUDescriptorHandleForHeapStart(),
                srv_descriptor_heap_->GetGPUDescriptorHandleForHeapStart())) {
            return false;
        }

        initialized_ = true;
        ERD::Main::g_FeatureStatus.overlay_ready = true;
        SPDLOG_INFO("DX12 posture overlay renderer initialized.");
        return true;
    }

    bool InitializeSyncObjects() {
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

    void Render(IDXGISwapChain* swap_chain) {
        if (!InitializeRenderer(swap_chain)) {
            return;
        }

        const UINT buffer_index = swap_chain_->GetCurrentBackBufferIndex();
        FrameContext& frame = frames_[buffer_index];
        WaitForFrame(frame);

        ImGui_ImplDX12_NewFrame();
        ImGui_ImplWin32_NewFrame();
        ImGui::NewFrame();
        DrawIntegratedPostureBars();
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

    void ResetRenderState() {
        if (initialized_) {
            WaitForGpu();

            ImGui_ImplDX12_Shutdown();
            ImGui_ImplWin32_Shutdown();
            ImGui::DestroyContext();

            initialized_ = false;
            ERD::Main::g_FeatureStatus.overlay_ready = false;
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

    void WaitForFrame(FrameContext& frame) {
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

    void WaitForGpu() {
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

    bool CreateHook(std::uint16_t index, void*& target, void** original, void* detour) {
        target = reinterpret_cast<void*>(methods_table_[index]);
        const MH_STATUS create_status = MH_CreateHook(target, detour, original);
        if (create_status != MH_OK && create_status != MH_ERROR_ALREADY_CREATED) {
            return false;
        }

        const MH_STATUS enable_status = MH_EnableHook(target);
        return enable_status == MH_OK || enable_status == MH_ERROR_ENABLED;
    }

    static void disable_hook(void* target) {
        if (target == nullptr) {
            return;
        }
        MH_DisableHook(target);
        MH_RemoveHook(target);
    }

    bool initialized_ = false;
    bool hooked_ = false;
    bool hook_failure_logged_ = false;
    HWND game_window_ = nullptr;
    WNDCLASSEXW window_class_{};
    HWND dummy_window_ = nullptr;
    std::uint64_t* methods_table_ = nullptr;

    IDXGISwapChain3* swap_chain_ = nullptr;
    ID3D12Device* device_ = nullptr;
    ID3D12CommandQueue* command_queue_ = nullptr;
    ID3D12GraphicsCommandList* command_list_ = nullptr;
    ID3D12DescriptorHeap* srv_descriptor_heap_ = nullptr;
    ID3D12DescriptorHeap* rtv_descriptor_heap_ = nullptr;
    ID3D12Fence* fence_ = nullptr;
    HANDLE fence_event_ = nullptr;
    UINT64 next_fence_value_ = 0;
    std::vector<FrameContext> frames_;

    void* present_target_ = nullptr;
    void* execute_command_lists_target_ = nullptr;
    void* resize_buffers_target_ = nullptr;
    PresentFn original_present_ = nullptr;
    ExecuteCommandListsFn original_execute_command_lists_ = nullptr;
    ResizeBuffersFn original_resize_buffers_ = nullptr;
};

}  // namespace

void initialize(const std::filesystem::path&) {
    if (!g_overlay) {
        g_overlay = std::make_unique<D3D12PostureOverlay>();
    }
}

void tick() {
    if (!g_overlay) {
        g_overlay = std::make_unique<D3D12PostureOverlay>();
    }
    g_overlay->Tick();
}

void shutdown() {
    g_overlay.reset();
}

}  // namespace grace_test::overlay
