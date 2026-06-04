#include "Features/OverlayD3DRenderer.hpp"

#include "Main/FeatureStatus.hpp"
#include "ThirdParty/ImGui/imgui.h"
#include "ThirdParty/ImGui/imgui_impl_dx12.h"
#include "ThirdParty/ImGui/imgui_impl_win32.h"
#include "ThirdParty/MinHook/MinHook.h"
#define STB_IMAGE_IMPLEMENTATION
#include "ThirdParty/Stb/stb_image.h"

#include <algorithm>
#include <array>
#include <cmath>
#include <cstdlib>
#include <cstring>
#include <cstdio>
#include <filesystem>
#include <format>
#include <unordered_map>
#include <string>
#include <string_view>

#include <spdlog/spdlog.h>

#ifdef min
#undef min
#endif
#ifdef max
#undef max
#endif

IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

namespace ERD::Features {
namespace {

constexpr UINT kSrvDescriptorCount = 20;
constexpr float kBossPoiseBarHeight = 36.0f;
constexpr float kStatusBarHeight = 25.0f;
constexpr float kStatusBarGap = 4.0f;
constexpr float kStatusPanelScale = 1.0f;
constexpr float kBossOverlayYOffset = 58.0f;
constexpr float kUnifiedAnchorGameY = 945.0f;
constexpr float kGameUiWidth = 1920.0f;
constexpr float kGameUiHeight = 1080.0f;
constexpr float kBossBarBaseWidth = 998.0f;
constexpr float kPoiseBarBaseWidth = 1058.0f;
constexpr DWORD kDummyDx12StabilizeDelayMs = 1000;

struct StatusBarSpec {
    const char* label = "";
    int current = 0;
    int maximum = 0;
    float modifier = 0.0f;
    ImU32 fill_color = 0;
    const char* icon_texture = "";
    bool hidden = false;
};

struct TextureInfo {
    ID3D12Resource* texture_resource = nullptr;
    int width = 0;
    int height = 0;
    int descriptor_index = 0;
};

std::uint64_t* g_methods_table = nullptr;
OverlayD3DRenderer* g_renderer_instance = nullptr;
std::unordered_map<std::string, TextureInfo> g_texture_map;
std::filesystem::path g_base_folder;

HWND FindGameWindow() {
    HWND window = FindWindowW(nullptr, L"ELDEN RING");
    if (window == nullptr) {
        window = GetForegroundWindow();
    }
    return window;
}

std::filesystem::path GetOverlayAssetsFolder() {
    return g_base_folder / "Resources" / "OverlayAssets";
}

void ConfigureOverlayFont(ImGuiIO& io) {
    const std::array<std::filesystem::path, 4> candidate_fonts{
        std::filesystem::path{R"(C:\Windows\Fonts\msyh.ttc)"},
        std::filesystem::path{R"(C:\Windows\Fonts\msyhbd.ttc)"},
        std::filesystem::path{R"(C:\Windows\Fonts\simhei.ttf)"},
        std::filesystem::path{R"(C:\Windows\Fonts\simsun.ttc)"},
    };

    ImFontConfig font_config{};
    font_config.OversampleH = 1;
    font_config.OversampleV = 1;
    font_config.PixelSnapH = true;

    for (const auto& font_path : candidate_fonts) {
        if (!std::filesystem::exists(font_path)) {
            continue;
        }

        if (ImFont* font = io.Fonts->AddFontFromFileTTF(
                font_path.string().c_str(),
                18.0f,
                &font_config,
                io.Fonts->GetGlyphRangesChineseFull())) {
            io.FontDefault = font;
            spdlog::info("DX12 overlay: loaded CJK font {}", font_path.string());
            return;
        }
    }

    spdlog::warn("DX12 overlay: failed to load a CJK font; Chinese labels may render as fallback characters.");
}

TextureInfo* GetTextureInfo(const std::string& name) {
    const auto it = g_texture_map.find(name);
    if (it == g_texture_map.end()) {
        return nullptr;
    }
    return &it->second;
}

D3D12_GPU_DESCRIPTOR_HANDLE GetGpuDescriptorHandle(
    D3D12_GPU_DESCRIPTOR_HANDLE start,
    UINT increment,
    int descriptor_index) {
    D3D12_GPU_DESCRIPTOR_HANDLE handle = start;
    handle.ptr += static_cast<SIZE_T>(increment) * descriptor_index;
    return handle;
}

bool LoadTextureIntoHeap(
    ID3D12Device* device,
    ID3D12DescriptorHeap* descriptor_heap,
    int descriptor_index,
    const std::filesystem::path& file_path,
    TextureInfo& out_texture) {
    int image_width = 0;
    int image_height = 0;
    unsigned char* image_data = stbi_load(file_path.string().c_str(), &image_width, &image_height, nullptr, 4);
    if (image_data == nullptr) {
        spdlog::error("DX12 overlay: failed to load texture {}", file_path.string());
        return false;
    }

    UINT handle_increment = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
    D3D12_CPU_DESCRIPTOR_HANDLE cpu_handle = descriptor_heap->GetCPUDescriptorHandleForHeapStart();
    cpu_handle.ptr += handle_increment * descriptor_index;

    D3D12_RESOURCE_DESC texture_desc{};
    texture_desc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
    texture_desc.Width = image_width;
    texture_desc.Height = image_height;
    texture_desc.DepthOrArraySize = 1;
    texture_desc.MipLevels = 1;
    texture_desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    texture_desc.SampleDesc.Count = 1;
    texture_desc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;

    D3D12_HEAP_PROPERTIES default_heap{};
    default_heap.Type = D3D12_HEAP_TYPE_DEFAULT;

    ID3D12Resource* texture = nullptr;
    if (FAILED(device->CreateCommittedResource(
            &default_heap,
            D3D12_HEAP_FLAG_NONE,
            &texture_desc,
            D3D12_RESOURCE_STATE_COPY_DEST,
            nullptr,
            IID_PPV_ARGS(&texture)))) {
        stbi_image_free(image_data);
        return false;
    }

    const UINT upload_pitch = (image_width * 4 + D3D12_TEXTURE_DATA_PITCH_ALIGNMENT - 1u) &
        ~(D3D12_TEXTURE_DATA_PITCH_ALIGNMENT - 1u);
    const UINT upload_size = image_height * upload_pitch;

    D3D12_RESOURCE_DESC upload_desc{};
    upload_desc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
    upload_desc.Width = upload_size;
    upload_desc.Height = 1;
    upload_desc.DepthOrArraySize = 1;
    upload_desc.MipLevels = 1;
    upload_desc.SampleDesc.Count = 1;
    upload_desc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

    D3D12_HEAP_PROPERTIES upload_heap{};
    upload_heap.Type = D3D12_HEAP_TYPE_UPLOAD;

    ID3D12Resource* upload_buffer = nullptr;
    if (FAILED(device->CreateCommittedResource(
            &upload_heap,
            D3D12_HEAP_FLAG_NONE,
            &upload_desc,
            D3D12_RESOURCE_STATE_GENERIC_READ,
            nullptr,
            IID_PPV_ARGS(&upload_buffer)))) {
        texture->Release();
        stbi_image_free(image_data);
        return false;
    }

    void* mapped = nullptr;
    D3D12_RANGE range{0, upload_size};
    upload_buffer->Map(0, &range, &mapped);
    for (int y = 0; y < image_height; ++y) {
        std::memcpy(static_cast<std::uint8_t*>(mapped) + y * upload_pitch, image_data + y * image_width * 4, image_width * 4);
    }
    upload_buffer->Unmap(0, &range);
    stbi_image_free(image_data);

    ID3D12Fence* fence = nullptr;
    HANDLE event_handle = CreateEventW(nullptr, FALSE, FALSE, nullptr);
    ID3D12CommandQueue* temp_queue = nullptr;
    ID3D12CommandAllocator* temp_allocator = nullptr;
    ID3D12GraphicsCommandList* temp_list = nullptr;

    D3D12_COMMAND_QUEUE_DESC queue_desc{};
    queue_desc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
    if (FAILED(device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&fence))) ||
        FAILED(device->CreateCommandQueue(&queue_desc, IID_PPV_ARGS(&temp_queue))) ||
        FAILED(device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&temp_allocator))) ||
        FAILED(device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, temp_allocator, nullptr, IID_PPV_ARGS(&temp_list)))) {
        if (temp_list != nullptr) temp_list->Release();
        if (temp_allocator != nullptr) temp_allocator->Release();
        if (temp_queue != nullptr) temp_queue->Release();
        if (fence != nullptr) fence->Release();
        if (event_handle != nullptr) CloseHandle(event_handle);
        upload_buffer->Release();
        texture->Release();
        return false;
    }

    D3D12_TEXTURE_COPY_LOCATION src{};
    src.pResource = upload_buffer;
    src.Type = D3D12_TEXTURE_COPY_TYPE_PLACED_FOOTPRINT;
    src.PlacedFootprint.Footprint.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    src.PlacedFootprint.Footprint.Width = image_width;
    src.PlacedFootprint.Footprint.Height = image_height;
    src.PlacedFootprint.Footprint.Depth = 1;
    src.PlacedFootprint.Footprint.RowPitch = upload_pitch;

    D3D12_TEXTURE_COPY_LOCATION dst{};
    dst.pResource = texture;
    dst.Type = D3D12_TEXTURE_COPY_TYPE_SUBRESOURCE_INDEX;
    dst.SubresourceIndex = 0;

    D3D12_RESOURCE_BARRIER barrier{};
    barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
    barrier.Transition.pResource = texture;
    barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
    barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_COPY_DEST;
    barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;

    temp_list->CopyTextureRegion(&dst, 0, 0, 0, &src, nullptr);
    temp_list->ResourceBarrier(1, &barrier);
    temp_list->Close();

    auto* temp_lists = reinterpret_cast<ID3D12CommandList* const*>(&temp_list);
    temp_queue->ExecuteCommandLists(1, temp_lists);
    temp_queue->Signal(fence, 1);
    fence->SetEventOnCompletion(1, event_handle);
    WaitForSingleObject(event_handle, INFINITE);

    D3D12_SHADER_RESOURCE_VIEW_DESC srv_desc{};
    srv_desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    srv_desc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
    srv_desc.Texture2D.MipLevels = 1;
    srv_desc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
    device->CreateShaderResourceView(texture, &srv_desc, cpu_handle);

    temp_list->Release();
    temp_allocator->Release();
    temp_queue->Release();
    fence->Release();
    CloseHandle(event_handle);
    upload_buffer->Release();

    out_texture.texture_resource = texture;
    out_texture.width = image_width;
    out_texture.height = image_height;
    out_texture.descriptor_index = descriptor_index;
    return true;
}

void LoadOverlayTextures(ID3D12Device* device, ID3D12DescriptorHeap* heap) {
    if (!g_texture_map.empty()) {
        return;
    }

    const std::filesystem::path assets_folder = GetOverlayAssetsFolder();
    spdlog::info("DX12 overlay: loading overlay textures from {}", assets_folder.string());
    const std::array<std::string, 18> texture_names{
        "GreenArrow.png",
        "RedArrow.png",
        "Bar.png",
        "BarBG.png",
        "BarEdge.png",
        "BarEdge2.png",
        "BuddyWaku.png",
        "ConditionWaku.png",
        "Green.png",
        "Red.png",
        "Yellow.png",
        "Poison.png",
        "ScarletRot.png",
        "Hemorrhage.png",
        "DeathBlight.png",
        "Frostbite.png",
        "Sleep.png",
        "Madness.png",
    };

    int descriptor_index = 1;
    for (const auto& name : texture_names) {
        TextureInfo texture{};
        if (LoadTextureIntoHeap(device, heap, descriptor_index, assets_folder / name, texture)) {
            g_texture_map.emplace(name, texture);
            ++descriptor_index;
        }
    }
    spdlog::info("DX12 overlay: loaded {}/{} overlay textures.", g_texture_map.size(), texture_names.size());
}

void ReleaseOverlayTextures() {
    for (auto& [name, texture] : g_texture_map) {
        if (texture.texture_resource != nullptr) {
            texture.texture_resource->Release();
            texture.texture_resource = nullptr;
        }
    }
    g_texture_map.clear();
}

float Ratio(int current, int maximum) {
    if (maximum <= 0) {
        return 0.0f;
    }
    return std::clamp(static_cast<float>(current) / static_cast<float>(maximum), 0.0f, 1.0f);
}

const char* LocalizeStatLabel(const char* english) {
    if (std::strcmp(english, "Poise") == 0) return "韧性";
    if (std::strcmp(english, "Poison") == 0) return "中毒";
    if (std::strcmp(english, "Rot") == 0) return "猩红腐败";
    if (std::strcmp(english, "Bleed") == 0) return "出血";
    if (std::strcmp(english, "Blight") == 0) return "死之污秽";
    if (std::strcmp(english, "Frost") == 0) return "冻伤";
    if (std::strcmp(english, "Sleep") == 0) return "睡眠";
    if (std::strcmp(english, "Madness") == 0) return "发狂";
    return english;
}

void DrawBar(
    ImDrawList* draw_list,
    ImVec2 origin,
    float width,
    float height,
    std::string_view label,
    int current,
    int maximum,
    ImU32 fill_color,
    const char* frame_texture_name,
    const char* icon_texture_name = nullptr,
    float alpha_scale = 1.0f,
    bool show_text = true,
    float modifier = 0.0f,
    bool show_modifier_arrow = false,
    bool fill_full_area = false,
    const char* fill_texture_name = nullptr) {
    const float ratio = Ratio(current, maximum);
    const bool has_icon = icon_texture_name != nullptr;
    const float icon_size = has_icon ? height : 0.0f;
    const float frame_x = has_icon ? origin.x + (icon_size * 0.86f) : origin.x;
    const float frame_width = has_icon ? (width - (icon_size * 0.86f)) : width;
    const ImVec2 frame_min{frame_x, origin.y};
    const ImVec2 frame_max{frame_x + frame_width, origin.y + height};

    ImVec2 bar_min{};
    ImVec2 bar_max{};
    if (has_icon) {
        const float bar_x = origin.x + (icon_size * 0.86f);
        const float bar_y = origin.y + ((icon_size * 0.5f) - ((height * 0.73f) * 0.5f));
        const float bar_width = width * 0.892f;
        const float bar_height = height * 0.73f;
        bar_min = ImVec2(bar_x, bar_y);
        bar_max = ImVec2(bar_x + bar_width, bar_y + bar_height);
    } else {
        const float inset_x = fill_full_area ? 0.0f : frame_width * 0.031f;
        const float inset_y = fill_full_area ? 0.0f : height * 0.137f;
        const float bar_width = fill_full_area ? frame_width : frame_width * 0.934f;
        const float bar_height = fill_full_area ? height : height * 0.708f;
        bar_min = ImVec2(frame_min.x + inset_x, origin.y + inset_y);
        bar_max = ImVec2(bar_min.x + bar_width, bar_min.y + bar_height);
    }
    const float fill_width = (bar_max.x - bar_min.x) * ratio;
    const int bg_alpha = static_cast<int>(72.0f * alpha_scale);
    const int fill_alpha = static_cast<int>(255.0f * alpha_scale);
    const ImU32 fallback_bg = IM_COL32(18, 20, 24, bg_alpha);
    const ImU32 effective_fill = (fill_color & 0x00FFFFFFu) | (static_cast<ImU32>(fill_alpha) << 24);
    const ImU32 textured_fill_tint = IM_COL32(255, 255, 255, fill_alpha);
    const ImU32 frame_tint = IM_COL32(255, 255, 255, static_cast<int>(255.0f * alpha_scale));

    auto get_texture_id = [&](const char* texture_name) -> ImTextureID {
        if (texture_name == nullptr || g_renderer_instance == nullptr || g_renderer_instance->GetSrvHeap() == nullptr ||
            g_renderer_instance->GetDevice() == nullptr) {
            return nullptr;
        }
        TextureInfo* texture = GetTextureInfo(texture_name);
        if (texture == nullptr) {
            return nullptr;
        }
        const UINT increment =
            g_renderer_instance->GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
        const D3D12_GPU_DESCRIPTOR_HANDLE heap_start =
            g_renderer_instance->GetSrvHeap()->GetGPUDescriptorHandleForHeapStart();
        return reinterpret_cast<ImTextureID>(GetGpuDescriptorHandle(heap_start, increment, texture->descriptor_index).ptr);
    };

    const ImTextureID bg_texture_id = get_texture_id("BarBG.png");
    const ImTextureID right_edge_texture_id = get_texture_id("BarEdge2.png");
    const ImTextureID frame_texture_id = get_texture_id("BuddyWaku.png");
    const ImTextureID default_fill_texture_id = get_texture_id(fill_texture_name != nullptr ? fill_texture_name : "Bar.png");
    const float edge_width = std::max(10.0f, width * 0.082f);
    const float edge_height_pad = height * 0.06f;

    if (bg_texture_id != nullptr) {
        draw_list->AddImage(bg_texture_id, bar_min, bar_max, ImVec2(0.0f, 0.0f), ImVec2(1.0f, 1.0f), frame_tint);
    } else {
        draw_list->AddRectFilled(bar_min, bar_max, fallback_bg, 3.0f);
    }

    if (fill_width > 0.0f) {
        draw_list->PushClipRect(bar_min, bar_max, true);
        if (default_fill_texture_id != nullptr) {
            draw_list->AddImage(
                default_fill_texture_id,
                bar_min,
                ImVec2(bar_min.x + fill_width, bar_max.y),
                ImVec2(0.0f, 0.0f),
                ImVec2(ratio, 1.0f),
                fill_texture_name != nullptr ? textured_fill_tint : effective_fill);
        } else {
            draw_list->AddRectFilled(bar_min, ImVec2(bar_min.x + fill_width, bar_max.y), effective_fill, 3.0f);
        }

        if (right_edge_texture_id != nullptr) {
            const float edge_x = std::clamp(bar_min.x + fill_width - (edge_width * 0.92f), bar_min.x, bar_max.x - edge_width * 0.16f);
            draw_list->AddImage(
                right_edge_texture_id,
                ImVec2(edge_x, bar_min.y - edge_height_pad),
                ImVec2(edge_x + edge_width, bar_max.y + edge_height_pad),
                ImVec2(0.0f, 0.0f),
                ImVec2(1.0f, 1.0f),
                frame_tint);
        } else {
            draw_list->AddLine(
                ImVec2(bar_min.x + fill_width, bar_min.y),
                ImVec2(bar_min.x + fill_width, bar_max.y),
                IM_COL32(255, 255, 255, static_cast<int>(160.0f * alpha_scale)),
                1.5f);
        }
        draw_list->PopClipRect();
    }

    if (frame_texture_id != nullptr) {
        draw_list->AddImage(frame_texture_id, frame_min, frame_max, ImVec2(0.0f, 0.0f), ImVec2(1.0f, 1.0f), frame_tint);
    } else {
        draw_list->AddRect(frame_min, frame_max, IM_COL32(220, 220, 220, static_cast<int>(200.0f * alpha_scale)), 3.0f);
    }

    if (has_icon) {
        TextureInfo* icon = GetTextureInfo(icon_texture_name);
        if (icon != nullptr && g_renderer_instance != nullptr && g_renderer_instance->GetSrvHeap() != nullptr &&
            g_renderer_instance->GetDevice() != nullptr) {
            const UINT increment = g_renderer_instance->GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
            const D3D12_GPU_DESCRIPTOR_HANDLE heap_start = g_renderer_instance->GetSrvHeap()->GetGPUDescriptorHandleForHeapStart();
            const ImTextureID icon_id = reinterpret_cast<ImTextureID>(GetGpuDescriptorHandle(heap_start, increment, icon->descriptor_index).ptr);
            draw_list->AddImage(
                icon_id,
                origin,
                ImVec2(origin.x + icon_size, origin.y + icon_size),
                ImVec2(0, 0),
                ImVec2(1, 1),
                IM_COL32(255, 255, 255, static_cast<int>(255.0f * alpha_scale)));
        } else {
            draw_list->AddCircleFilled(
                ImVec2(origin.x + icon_size * 0.5f, origin.y + icon_size * 0.5f),
                icon_size * 0.33f,
                effective_fill,
                20);
        }
    }

    if (show_text) {
        std::string label_text(label);
        const char* localized = LocalizeStatLabel(label_text.c_str());
        char value_text[48]{};
        std::snprintf(value_text, sizeof(value_text), "%d/%d", current, maximum);
        const bool hide_label = label == "Poise";
        const float text_font_size = hide_label ? (ImGui::GetFontSize() * 1.45f) : ImGui::GetFontSize();
        const ImVec2 text_pos{
            bar_min.x + 10.0f,
            bar_min.y + ((bar_max.y - bar_min.y) - text_font_size) * 0.5f - 1.0f};
        const ImVec2 value_size = ImGui::CalcTextSize(value_text);
        const float value_scale = hide_label ? 1.45f : 1.0f;
        const ImVec2 scaled_value_size{value_size.x * value_scale, value_size.y * value_scale};
        const ImVec2 value_pos = hide_label
            ? ImVec2(
                bar_min.x + ((bar_max.x - bar_min.x) - scaled_value_size.x) * 0.5f,
                bar_min.y + ((bar_max.y - bar_min.y) - text_font_size) * 0.5f - 1.0f)
            : ImVec2(
                bar_max.x - scaled_value_size.x - 10.0f,
                text_pos.y);

        if (!hide_label) {
            draw_list->AddText(ImGui::GetFont(), text_font_size, text_pos, IM_COL32(240, 240, 240, static_cast<int>(255.0f * alpha_scale)), localized);
        }
        draw_list->AddText(ImGui::GetFont(), text_font_size, value_pos, IM_COL32(248, 248, 248, static_cast<int>(255.0f * alpha_scale)), value_text);

        if (!hide_label && show_modifier_arrow && std::fabs(modifier) >= 0.001f) {
            const ImVec2 label_size = ImGui::CalcTextSize(localized);
            const char* modifier_text = modifier > 0.0f ? "+" : "-";
            const ImU32 modifier_color = modifier > 0.0f
                ? IM_COL32(170, 224, 148, static_cast<int>(255.0f * alpha_scale))
                : IM_COL32(231, 110, 102, static_cast<int>(255.0f * alpha_scale));
            draw_list->AddText(
                ImVec2(text_pos.x + label_size.x + 6.0f, text_pos.y),
                modifier_color,
                modifier_text);
        }
    }

}

void DrawTargetDebugPanel(ImDrawList* draw_list, const ImVec2& display) {
    auto& status = Main::g_FeatureStatus;
    const float panel_width = 300.0f;
    const float line_height = 16.0f;
    const float panel_height = 12.0f + (13.0f * line_height);
    const ImVec2 panel_min{display.x - panel_width - 24.0f, 24.0f};
    const ImVec2 panel_max{display.x - 24.0f, 24.0f + panel_height};

    draw_list->AddRectFilled(panel_min, panel_max, IM_COL32(8, 10, 14, 210), 8.0f);
    draw_list->AddRect(panel_min, panel_max, IM_COL32(180, 180, 190, 180), 8.0f, 0, 1.0f);

    float y = panel_min.y + 8.0f;
    auto draw_line = [&](const char* text, ImU32 color = IM_COL32(235, 235, 240, 255)) {
        draw_list->AddText(ImVec2(panel_min.x + 10.0f, y), color, text);
        y += line_height;
    };

    char line[160]{};
    draw_line("Target Debug");
    std::snprintf(line, sizeof(line), "valid: %s", status.targeted_npc_valid.load() ? "true" : "false");
    draw_line(line, status.targeted_npc_valid.load() ? IM_COL32(140, 220, 140, 255) : IM_COL32(255, 180, 120, 255));
    std::snprintf(line, sizeof(line), "addr: 0x%llX", static_cast<unsigned long long>(status.targeted_npc_address.load()));
    draw_line(line);
    std::snprintf(line, sizeof(line), "hp: %d / %d", status.targeted_npc_hp.load(), status.targeted_npc_max_hp.load());
    draw_line(line);
    std::snprintf(line, sizeof(line), "poise: %d / %d", status.targeted_npc_poise.load(), status.targeted_npc_max_poise.load());
    draw_line(line);
    std::snprintf(line, sizeof(line), "sleep: %d / %d", status.targeted_npc_sleep.load(), status.targeted_npc_sleep_max.load());
    draw_line(line);
    std::snprintf(line, sizeof(line), "poison: %d / %d", status.targeted_npc_poison.load(), status.targeted_npc_poison_max.load());
    draw_line(line);
    std::snprintf(line, sizeof(line), "rot: %d / %d", status.targeted_npc_rot.load(), status.targeted_npc_rot_max.load());
    draw_line(line);
    std::snprintf(line, sizeof(line), "frost: %d / %d", status.targeted_npc_frost.load(), status.targeted_npc_frost_max.load());
    draw_line(line);
    std::snprintf(line, sizeof(line), "bleed: %d / %d", status.targeted_npc_bleed.load(), status.targeted_npc_bleed_max.load());
    draw_line(line);
    std::snprintf(line, sizeof(line), "madness: %d / %d", status.targeted_npc_madness.load(), status.targeted_npc_madness_max.load());
    draw_line(line);
    std::snprintf(line, sizeof(line), "blight: %d / %d", status.targeted_npc_blight.load(), status.targeted_npc_blight_max.load());
    draw_line(line);
    std::snprintf(
        line,
        sizeof(line),
        "mods: p=%.2f r=%.2f b=%.2f bl=%.2f f=%.2f s=%.2f m=%.2f",
        status.targeted_npc_poison_mod.load(),
        status.targeted_npc_rot_mod.load(),
        status.targeted_npc_bleed_mod.load(),
        status.targeted_npc_blight_mod.load(),
        status.targeted_npc_frost_mod.load(),
        status.targeted_npc_sleep_mod.load(),
        status.targeted_npc_madness_mod.load());
    draw_line(line);
    std::snprintf(
        line,
        sizeof(line),
        "ui: x=%.1f y=%.1f mod=%.2f boss=%s",
        status.targeted_npc_ui_x.load(),
        status.targeted_npc_ui_y.load(),
        status.targeted_npc_ui_mod.load(),
        status.targeted_npc_ui_is_boss.load() ? "true" : "false");
    draw_line(line);
}

void DrawImpendingStatusIcons(ImDrawList* draw_list, ImVec2 anchor) {
    auto& status = Main::g_FeatureStatus;
    struct ImpendingSpec {
        int current;
        int maximum;
        const char* icon_texture;
        ImU32 glow_color;
    };

    const ImpendingSpec impending[] = {
        {status.targeted_npc_poison.load(), status.targeted_npc_poison_max.load(), "Poison.png", IM_COL32(118, 156, 64, 210)},
        {status.targeted_npc_rot.load(), status.targeted_npc_rot_max.load(), "ScarletRot.png", IM_COL32(156, 66, 36, 210)},
        {status.targeted_npc_bleed.load(), status.targeted_npc_bleed_max.load(), "Hemorrhage.png", IM_COL32(176, 38, 50, 210)},
        {status.targeted_npc_blight.load(), status.targeted_npc_blight_max.load(), "DeathBlight.png", IM_COL32(116, 110, 112, 210)},
        {status.targeted_npc_frost.load(), status.targeted_npc_frost_max.load(), "Frostbite.png", IM_COL32(74, 152, 208, 210)},
        {status.targeted_npc_sleep.load(), status.targeted_npc_sleep_max.load(), "Sleep.png", IM_COL32(96, 100, 162, 210)},
        {status.targeted_npc_madness.load(), status.targeted_npc_madness_max.load(), "Madness.png", IM_COL32(202, 132, 36, 210)},
    };

    float x = anchor.x;
    const float size = 20.0f;
    const float spacing = 6.0f;
    const float pulse = 0.7f + (0.3f * std::sin(static_cast<float>(GetTickCount64() % 1000) / 1000.0f * 6.28318f));
    for (const ImpendingSpec& spec : impending) {
        if (spec.maximum <= 0) {
            continue;
        }
        const float ratio = static_cast<float>(spec.current) / static_cast<float>(spec.maximum);
        if (ratio > 0.30f) {
            continue;
        }

        draw_list->AddCircleFilled(
            ImVec2(x + size * 0.5f, anchor.y + size * 0.5f),
            size * 0.72f,
            IM_COL32(
                (spec.glow_color >> IM_COL32_R_SHIFT) & 0xFF,
                (spec.glow_color >> IM_COL32_G_SHIFT) & 0xFF,
                (spec.glow_color >> IM_COL32_B_SHIFT) & 0xFF,
                static_cast<int>(110.0f * pulse)),
            24);

        TextureInfo* icon = GetTextureInfo(spec.icon_texture);
        if (icon != nullptr && g_renderer_instance != nullptr && g_renderer_instance->GetSrvHeap() != nullptr &&
            g_renderer_instance->GetDevice() != nullptr) {
            const UINT increment = g_renderer_instance->GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
            const D3D12_GPU_DESCRIPTOR_HANDLE heap_start = g_renderer_instance->GetSrvHeap()->GetGPUDescriptorHandleForHeapStart();
            const ImTextureID icon_id = reinterpret_cast<ImTextureID>(GetGpuDescriptorHandle(heap_start, increment, icon->descriptor_index).ptr);
            draw_list->AddImage(icon_id, ImVec2(x, anchor.y), ImVec2(x + size, anchor.y + size));
        } else {
            draw_list->AddCircleFilled(ImVec2(x + size * 0.5f, anchor.y + size * 0.5f), size * 0.35f, spec.glow_color, 20);
        }
        x += size + spacing;
    }
}

void DrawTargetHud() {
    auto& status = Main::g_FeatureStatus;
    const bool show_poise = status.enemy_poise_overlay_enabled.load();
    const bool show_status_panel = status.enemy_resistance_overlay_enabled.load();
    if (!show_poise && !show_status_panel) {
        return;
    }

    ImDrawList* draw_list = ImGui::GetBackgroundDrawList();
    const ImVec2 display = ImGui::GetIO().DisplaySize;
    const float viewport_scale = std::min(display.x / kGameUiWidth, display.y / kGameUiHeight);
    const float viewport_pos_x = (display.x - std::ceilf(kGameUiWidth * viewport_scale)) * 0.5f;
    const float viewport_pos_y = (display.y - std::ceilf(kGameUiHeight * viewport_scale)) * 0.5f;
    const bool is_boss_anchor = status.targeted_npc_ui_is_boss.load();
    const float poise_height =
        kBossPoiseBarHeight * std::clamp(status.overlay_poise_height_scale.load(), 0.5f, 2.0f) * viewport_scale;
    const float poise_width =
        kPoiseBarBaseWidth * std::clamp(status.overlay_poise_width_scale.load(), 0.5f, 2.0f) * viewport_scale;
    const float main_width =
        kBossBarBaseWidth * std::clamp(status.overlay_status_width_scale.load(), 0.5f, 2.0f) * viewport_scale;
    const float poise_block_height = poise_height + 18.0f;
    if (!status.targeted_npc_valid.load()) {
        return;
    }
    const float poise_vertical_offset = std::clamp(status.overlay_poise_vertical_offset.load(), -300.0f, 300.0f);
    const float entity_status_vertical_offset =
        std::clamp(status.overlay_entity_status_vertical_offset.load(), -300.0f, 300.0f);
    const float boss_status_gap = std::clamp(status.overlay_boss_status_gap.load(), -100.0f, 200.0f);
    const ImVec2 bars_origin(
        (display.x - poise_width) * 0.5f,
        ((kUnifiedAnchorGameY - kBossOverlayYOffset) + poise_vertical_offset) * viewport_scale + viewport_pos_y);

    const float poise_y = bars_origin.y - poise_block_height;
    const int max_poise = status.targeted_npc_max_poise.load();

    if (show_poise && max_poise > 0) {
        DrawBar(
            draw_list,
            ImVec2(bars_origin.x, poise_y),
            poise_width,
            poise_height,
            "Poise",
            status.targeted_npc_poise.load(),
            max_poise,
            IM_COL32(108, 166, 98, 255),
            "ConditionWaku.png",
            nullptr,
            1.0f,
            true,
            0.0f,
            false,
            false,
            "Green.png");
        DrawImpendingStatusIcons(draw_list, ImVec2(bars_origin.x + 6.0f, poise_y - 28.0f));
    }

    const StatusBarSpec status_bars[] = {
        {"Bleed", status.targeted_npc_bleed.load(), status.targeted_npc_bleed_max.load(), status.targeted_npc_bleed_mod.load(), IM_COL32(176, 38, 50, 255), "Hemorrhage.png", !status.enemy_resistance_bleed_enabled.load()},
        {"Frost", status.targeted_npc_frost.load(), status.targeted_npc_frost_max.load(), status.targeted_npc_frost_mod.load(), IM_COL32(74, 152, 208, 255), "Frostbite.png", !status.enemy_resistance_frost_enabled.load()},
        {"Rot", status.targeted_npc_rot.load(), status.targeted_npc_rot_max.load(), status.targeted_npc_rot_mod.load(), IM_COL32(156, 66, 36, 255), "ScarletRot.png", !status.enemy_resistance_rot_enabled.load()},
        {"Poison", status.targeted_npc_poison.load(), status.targeted_npc_poison_max.load(), status.targeted_npc_poison_mod.load(), IM_COL32(118, 156, 64, 255), "Poison.png", !status.enemy_resistance_poison_enabled.load()},
        {"Sleep", status.targeted_npc_sleep.load(), status.targeted_npc_sleep_max.load(), status.targeted_npc_sleep_mod.load(), IM_COL32(96, 100, 162, 255), "Sleep.png", !status.enemy_resistance_sleep_enabled.load()},
        {"Madness", status.targeted_npc_madness.load(), status.targeted_npc_madness_max.load(), status.targeted_npc_madness_mod.load(), IM_COL32(202, 132, 36, 255), "Madness.png", !status.enemy_resistance_madness_enabled.load()},
    };

    const float status_scale = kStatusPanelScale;
    constexpr int kStatusColumns = 3;
    const float status_height =
        kStatusBarHeight * std::clamp(status.overlay_status_height_scale.load(), 0.5f, 2.0f) * viewport_scale * status_scale;
    const float status_panel_width = is_boss_anchor ? main_width : (main_width * (2.0f / 3.0f));
    const float single_bar_width = status_panel_width / static_cast<float>(kStatusColumns);
    const float column_gap = 0.0f;
    const float status_gap = kStatusBarGap * viewport_scale * status_scale;
    std::size_t visible_status_count = 0;
    for (const StatusBarSpec& spec : status_bars) {
        if (!spec.hidden) {
            ++visible_status_count;
        }
    }
    if (!show_status_panel || visible_status_count == 0) {
        return;
    }
    const std::size_t row_count = (visible_status_count + kStatusColumns - 1) / kStatusColumns;
    const float panel_width = status_panel_width;
    const float panel_height = (row_count * status_height) +
        ((row_count > 0 ? row_count - 1 : 0) * status_gap) +
        18.0f * viewport_scale * status_scale;
    const ImVec2 panel_min = is_boss_anchor
        ? ImVec2(
            (status.targeted_npc_ui_x.load() * viewport_scale) + viewport_pos_x - (panel_width * 0.5f),
            (status.targeted_npc_ui_y.load() * viewport_scale) + viewport_pos_y + (boss_status_gap * viewport_scale))
        : ImVec2(
            (display.x - panel_width) * 0.5f,
            (entity_status_vertical_offset * viewport_scale) + viewport_pos_y);

    const float start_x = panel_min.x;
    const float start_y = panel_min.y + 9.0f * viewport_scale * status_scale;
    std::size_t visible_index = 0;
    for (const StatusBarSpec& spec : status_bars) {
        if (spec.hidden) {
            continue;
        }
        const std::size_t row = visible_index / kStatusColumns;
        const std::size_t column = visible_index % kStatusColumns;
        const float x = start_x + (column * (single_bar_width + column_gap));
        const float y = start_y + (row * (status_height + status_gap));
        DrawBar(
            draw_list,
            ImVec2(x, y),
            single_bar_width,
            status_height,
            spec.label,
            spec.current,
            spec.maximum,
            spec.fill_color,
            "ConditionWaku.png",
            spec.icon_texture,
            0.72f,
            true,
            spec.modifier,
            true,
            false);
        ++visible_index;
    }
}

}  // namespace

OverlayD3DRenderer& OverlayD3DRenderer::Instance() {
    static OverlayD3DRenderer instance;
    return instance;
}

OverlayD3DRenderer::OverlayD3DRenderer() {
    g_renderer_instance = this;
}

OverlayD3DRenderer::~OverlayD3DRenderer() {
    Shutdown();
    g_renderer_instance = nullptr;
}

void OverlayD3DRenderer::SetBaseFolder(const std::filesystem::path& folder) {
    base_folder_ = folder;
    g_base_folder = folder;
}

bool OverlayD3DRenderer::InitWindow() {
    window_class_.cbSize = sizeof(WNDCLASSEXW);
    window_class_.style = CS_HREDRAW | CS_VREDRAW;
    window_class_.lpfnWndProc = DefWindowProcW;
    window_class_.hInstance = GetModuleHandleW(nullptr);
    window_class_.lpszClassName = L"erdGameToolsPostureOverlayWindow";
    RegisterClassExW(&window_class_);
    window_hwnd_ = CreateWindowW(
        window_class_.lpszClassName,
        L"erdGameToolsDX12Window",
        WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_THICKFRAME,
        0,
        0,
        100,
        100,
        nullptr,
        nullptr,
        window_class_.hInstance,
        nullptr);
    return window_hwnd_ != nullptr;
}

bool OverlayD3DRenderer::DeleteWindow() {
    if (window_hwnd_ != nullptr) {
        DestroyWindow(window_hwnd_);
        window_hwnd_ = nullptr;
    }
    if (window_class_.lpszClassName != nullptr) {
        UnregisterClassW(window_class_.lpszClassName, window_class_.hInstance);
    }
    return true;
}

bool OverlayD3DRenderer::InitHook() {
    if (!InitWindow()) {
        spdlog::error("DX12 overlay: failed to create dummy hook window.");
        return false;
    }

    HMODULE d3d12_module = GetModuleHandleW(L"d3d12.dll");
    HMODULE dxgi_module = GetModuleHandleW(L"dxgi.dll");
    if (d3d12_module == nullptr || dxgi_module == nullptr) {
        DeleteWindow();
        spdlog::error("DX12 overlay: d3d12.dll or dxgi.dll not loaded.");
        return false;
    }

    auto create_dxgi_factory = reinterpret_cast<long(__stdcall*)(const IID&, void**)>(GetProcAddress(dxgi_module, "CreateDXGIFactory"));
    auto d3d12_create_device = reinterpret_cast<HRESULT(__stdcall*)(IUnknown*, D3D_FEATURE_LEVEL, const IID&, void**)>(
        GetProcAddress(d3d12_module, "D3D12CreateDevice"));
    if (create_dxgi_factory == nullptr || d3d12_create_device == nullptr) {
        DeleteWindow();
        spdlog::error("DX12 overlay: failed to resolve DXGI/D3D12 exports.");
        return false;
    }

    IDXGIFactory* factory = nullptr;
    IDXGIAdapter* adapter = nullptr;
    ID3D12Device* device = nullptr;
    ID3D12CommandQueue* command_queue = nullptr;
    ID3D12CommandAllocator* command_allocator = nullptr;
    ID3D12GraphicsCommandList* command_list = nullptr;
    IDXGISwapChain* swap_chain = nullptr;

    if (create_dxgi_factory(IID_PPV_ARGS(&factory)) < 0 ||
        factory->EnumAdapters(0, &adapter) == DXGI_ERROR_NOT_FOUND ||
        d3d12_create_device(adapter, D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&device)) < 0) {
        if (device != nullptr) {
            device->Release();
        }
        if (adapter != nullptr) {
            adapter->Release();
        }
        if (factory != nullptr) {
            factory->Release();
        }
        DeleteWindow();
        spdlog::error("DX12 overlay: failed to create dummy DX12 device.");
        return false;
    }

    D3D12_COMMAND_QUEUE_DESC queue_desc{};
    queue_desc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
    if (device->CreateCommandQueue(&queue_desc, IID_PPV_ARGS(&command_queue)) < 0 ||
        device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&command_allocator)) < 0 ||
        device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, command_allocator, nullptr, IID_PPV_ARGS(&command_list)) < 0) {
        if (command_list != nullptr) {
            command_list->Release();
        }
        if (command_allocator != nullptr) {
            command_allocator->Release();
        }
        command_queue->Release();
        device->Release();
        adapter->Release();
        factory->Release();
        DeleteWindow();
        spdlog::error("DX12 overlay: failed to create dummy command objects.");
        return false;
    }

    DXGI_RATIONAL refresh_rate{60, 1};
    DXGI_MODE_DESC buffer_desc{};
    buffer_desc.Width = 100;
    buffer_desc.Height = 100;
    buffer_desc.RefreshRate = refresh_rate;
    buffer_desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    buffer_desc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
    buffer_desc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;

    DXGI_SAMPLE_DESC sample_desc{1, 0};
    DXGI_SWAP_CHAIN_DESC swap_desc{};
    swap_desc.BufferDesc = buffer_desc;
    swap_desc.SampleDesc = sample_desc;
    swap_desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    swap_desc.BufferCount = 2;
    swap_desc.OutputWindow = window_hwnd_;
    swap_desc.Windowed = TRUE;
    swap_desc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
    swap_desc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

    if (factory->CreateSwapChain(command_queue, &swap_desc, &swap_chain) < 0) {
        command_list->Release();
        command_allocator->Release();
        command_queue->Release();
        device->Release();
        adapter->Release();
        factory->Release();
        DeleteWindow();
        spdlog::error("DX12 overlay: failed to create dummy swap chain.");
        return false;
    }

    if (g_methods_table == nullptr) {
        g_methods_table = static_cast<std::uint64_t*>(std::calloc(150, sizeof(std::uint64_t)));
    }
    std::memcpy(g_methods_table, *reinterpret_cast<std::uint64_t**>(device), 44 * sizeof(std::uint64_t));
    std::memcpy(g_methods_table + 44, *reinterpret_cast<std::uint64_t**>(command_queue), 19 * sizeof(std::uint64_t));
    std::memcpy(g_methods_table + 44 + 19, *reinterpret_cast<std::uint64_t**>(command_allocator), 9 * sizeof(std::uint64_t));
    std::memcpy(g_methods_table + 44 + 19 + 9, *reinterpret_cast<std::uint64_t**>(command_list), 60 * sizeof(std::uint64_t));
    std::memcpy(g_methods_table + 44 + 19 + 9 + 60, *reinterpret_cast<std::uint64_t**>(swap_chain), 18 * sizeof(std::uint64_t));

    Sleep(kDummyDx12StabilizeDelayMs);

    const MH_STATUS status = MH_Initialize();
    if (status != MH_OK && status != MH_ERROR_ALREADY_INITIALIZED) {
        spdlog::error("DX12 overlay: MH_Initialize failed: {}", static_cast<int>(status));
    }

    swap_chain->Release();
    command_list->Release();
    command_allocator->Release();
    command_queue->Release();
    device->Release();
    adapter->Release();
    factory->Release();
    DeleteWindow();
    return true;
}

bool OverlayD3DRenderer::CreateHook(std::uint16_t index, void** original, void* replacement) {
    if (g_methods_table == nullptr) {
        return false;
    }
    void* target = reinterpret_cast<void*>(g_methods_table[index]);
    return MH_CreateHook(target, replacement, original) == MH_OK && MH_EnableHook(target) == MH_OK;
}

void OverlayD3DRenderer::DisableHook(std::uint16_t index) {
    if (g_methods_table != nullptr) {
        MH_DisableHook(reinterpret_cast<void*>(g_methods_table[index]));
    }
}

void OverlayD3DRenderer::DisableAll() {
    DisableHook(54);
    DisableHook(140);
    DisableHook(146);
}

void OverlayD3DRenderer::Hook() {
    if (hooked_) {
        return;
    }
    spdlog::info("DX12 overlay: installing PostureBarMod-style hooks.");
    if (!InitHook()) {
        spdlog::error("DX12 overlay: InitHook failed.");
        return;
    }
    if (!CreateHook(54, reinterpret_cast<void**>(&original_execute_command_lists_), reinterpret_cast<void*>(&HookExecuteCommandLists)) ||
        !CreateHook(140, reinterpret_cast<void**>(&original_present_), reinterpret_cast<void*>(&HookPresent)) ||
        !CreateHook(146, reinterpret_cast<void**>(&original_resize_target_), reinterpret_cast<void*>(&HookResizeTarget))) {
        spdlog::error("DX12 overlay: failed to install one or more D3D hooks.");
        return;
    }
    hooked_ = true;
}

void OverlayD3DRenderer::ResetRenderState(IDXGISwapChain3* swap_chain) {
    if (swap_chain != nullptr) {
        swap_chain->Release();
    }
    if (device_ != nullptr) {
        device_->Release();
        device_ = nullptr;
    }
    if (command_list_ != nullptr) {
        command_list_->Release();
        command_list_ = nullptr;
    }
    if (srv_heap_ != nullptr) {
        srv_heap_->Release();
        srv_heap_ = nullptr;
    }
    if (rtv_heap_ != nullptr) {
        rtv_heap_->Release();
        rtv_heap_ = nullptr;
    }
    if (command_allocators_ != nullptr) {
        for (std::uint64_t i = 0; i < buffers_count_; ++i) {
            if (command_allocators_[i] != nullptr) {
                command_allocators_[i]->Release();
            }
        }
        delete[] command_allocators_;
        command_allocators_ = nullptr;
    }
    if (back_buffers_ != nullptr) {
        for (std::uint64_t i = 0; i < buffers_count_; ++i) {
            if (back_buffers_[i] != nullptr) {
                back_buffers_[i]->Release();
            }
        }
        delete[] back_buffers_;
        back_buffers_ = nullptr;
    }

    render_targets_.clear();
    buffers_count_ = 0;
    ReleaseOverlayTextures();

    if (initialized_) {
        ImGui_ImplDX12_Shutdown();
        ImGui_ImplWin32_Shutdown();
        ImGui::DestroyContext();
        initialized_ = false;
    }
    Main::g_FeatureStatus.overlay_ready = false;
}

void OverlayD3DRenderer::Overlay(IDXGISwapChain* swap_chain) {
    if (command_queue_ == nullptr) {
        return;
    }

    IDXGISwapChain3* swap_chain3 = nullptr;
    if (FAILED(swap_chain->QueryInterface(IID_PPV_ARGS(&swap_chain3))) || swap_chain3 == nullptr) {
        return;
    }

    DXGI_SWAP_CHAIN_DESC desc{};
    swap_chain3->GetDesc(&desc);

    if (!initialized_) {
        UINT buffer_index = swap_chain3->GetCurrentBackBufferIndex();
        if (FAILED(swap_chain3->GetDevice(IID_PPV_ARGS(&device_)))) {
            swap_chain3->Release();
            return;
        }

        buffers_count_ = desc.BufferCount;
        render_targets_.clear();

        D3D12_DESCRIPTOR_HEAP_DESC srv_desc{};
        srv_desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
        srv_desc.NumDescriptors = kSrvDescriptorCount;
        srv_desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
        if (device_->CreateDescriptorHeap(&srv_desc, IID_PPV_ARGS(&srv_heap_)) != S_OK) {
            device_->Release();
            device_ = nullptr;
            swap_chain3->Release();
            return;
        }

        D3D12_DESCRIPTOR_HEAP_DESC rtv_desc{};
        rtv_desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
        rtv_desc.NumDescriptors = desc.BufferCount;
        rtv_desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
        rtv_desc.NodeMask = 1;
        if (device_->CreateDescriptorHeap(&rtv_desc, IID_PPV_ARGS(&rtv_heap_)) != S_OK) {
            device_->Release();
            device_ = nullptr;
            srv_heap_->Release();
            srv_heap_ = nullptr;
            swap_chain3->Release();
            return;
        }

        const SIZE_T rtv_descriptor_size = device_->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
        D3D12_CPU_DESCRIPTOR_HANDLE rtv_handle = rtv_heap_->GetCPUDescriptorHandleForHeapStart();
        command_allocators_ = new ID3D12CommandAllocator*[desc.BufferCount]();
        for (UINT i = 0; i < desc.BufferCount; ++i) {
            render_targets_.push_back(rtv_handle);
            rtv_handle.ptr += rtv_descriptor_size;
        }

        for (UINT i = 0; i < desc.BufferCount; ++i) {
            if (device_->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&command_allocators_[i])) != S_OK) {
                swap_chain3->Release();
                return;
            }
        }

        if (device_->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, command_allocators_[0], nullptr, IID_PPV_ARGS(&command_list_)) != S_OK ||
            command_list_->Close() != S_OK) {
            swap_chain3->Release();
            return;
        }

        back_buffers_ = new ID3D12Resource*[desc.BufferCount]();
        for (UINT i = 0; i < desc.BufferCount; ++i) {
            swap_chain3->GetBuffer(i, IID_PPV_ARGS(&back_buffers_[i]));
            device_->CreateRenderTargetView(back_buffers_[i], nullptr, render_targets_[i]);
        }

        ImGui::CreateContext();
        ImGuiIO& io = ImGui::GetIO();
        io.IniFilename = nullptr;
        ConfigureOverlayFont(io);

        HWND game_window = FindGameWindow();
        if (game_window == nullptr || !ImGui_ImplWin32_Init(game_window)) {
            swap_chain3->Release();
            return;
        }

        if (!ImGui_ImplDX12_Init(
                device_,
                static_cast<int>(desc.BufferCount),
                DXGI_FORMAT_R8G8B8A8_UNORM,
                srv_heap_,
                srv_heap_->GetCPUDescriptorHandleForHeapStart(),
                srv_heap_->GetGPUDescriptorHandleForHeapStart())) {
            swap_chain3->Release();
            return;
        }

        LoadOverlayTextures(device_, srv_heap_);

        initialized_ = true;
        Main::g_FeatureStatus.overlay_ready = true;
        spdlog::info("DX12 overlay: PostureBarMod-style renderer initialized.");
        if (buffer_index >= desc.BufferCount) {
            swap_chain3->Release();
            return;
        }
    }

    ImGui_ImplDX12_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();

    DrawTargetHud();

    ImGui::EndFrame();
    ImGui::Render();

    const UINT buffer_index = swap_chain3->GetCurrentBackBufferIndex();
    if (buffer_index >= buffers_count_) {
        swap_chain3->Release();
        return;
    }

    command_allocators_[buffer_index]->Reset();
    command_list_->Reset(command_allocators_[buffer_index], nullptr);

    D3D12_RESOURCE_BARRIER barrier{};
    barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
    barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
    barrier.Transition.pResource = back_buffers_[buffer_index];
    barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
    barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;
    barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;

    command_list_->ResourceBarrier(1, &barrier);
    command_list_->OMSetRenderTargets(1, &render_targets_[buffer_index], FALSE, nullptr);
    command_list_->SetDescriptorHeaps(1, &srv_heap_);
    ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), command_list_);

    barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
    barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;
    command_list_->ResourceBarrier(1, &barrier);
    command_list_->Close();

    auto* lists = reinterpret_cast<ID3D12CommandList* const*>(&command_list_);
    command_queue_->ExecuteCommandLists(1, lists);
    swap_chain3->Release();
}

HRESULT APIENTRY OverlayD3DRenderer::HookPresent(IDXGISwapChain* swap_chain, UINT sync_interval, UINT flags) {
    if (g_renderer_instance != nullptr) {
        g_renderer_instance->Overlay(swap_chain);
        return g_renderer_instance->original_present_(swap_chain, sync_interval, flags);
    }
    return S_OK;
}

void OverlayD3DRenderer::HookExecuteCommandLists(
    ID3D12CommandQueue* queue,
    UINT num_command_lists,
    ID3D12CommandList* command_lists) {
    if (g_renderer_instance != nullptr && g_renderer_instance->command_queue_ == nullptr &&
        queue->GetDesc().Type == D3D12_COMMAND_LIST_TYPE_DIRECT) {
        g_renderer_instance->command_queue_ = queue;
        spdlog::info("DX12 overlay: captured direct command queue.");
    }
    g_renderer_instance->original_execute_command_lists_(queue, num_command_lists, command_lists);
}

HRESULT APIENTRY OverlayD3DRenderer::HookResizeTarget(IDXGISwapChain* swap_chain, const DXGI_MODE_DESC* target_parameters) {
    if (g_renderer_instance != nullptr && g_renderer_instance->initialized_) {
        g_renderer_instance->ResetRenderState();
    }
    return g_renderer_instance->original_resize_target_(swap_chain, target_parameters);
}

void OverlayD3DRenderer::Shutdown() {
    if (hooked_) {
        DisableAll();
        hooked_ = false;
    }
    ResetRenderState();
}

}  // namespace ERD::Features
