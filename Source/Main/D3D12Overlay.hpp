#pragma once

#include "../Common.hpp"

#include <d3d12.h>
#include <dxgi1_4.h>

#include "../ThirdParty/ImGui/imgui.h"

namespace ERD::Main {

// DX12 + ImGui 叠加层。
// 通过 hook ExecuteCommandLists / Present / ResizeBuffers，把插件菜单绘制到游戏画面中。
class D3D12Overlay {
public:
    D3D12Overlay() = default;
    ~D3D12Overlay() noexcept;

    bool Hook();
    void Unhook();

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
        ID3D12CommandList* const* command_lists
    );
    using ResizeBuffersFn = HRESULT(APIENTRY*)(
        IDXGISwapChain* swap_chain,
        UINT buffer_count,
        UINT width,
        UINT height,
        DXGI_FORMAT new_format,
        UINT flags
    );

    static HRESULT APIENTRY HookPresent(IDXGISwapChain* swap_chain, UINT sync_interval, UINT flags);
    static void APIENTRY HookExecuteCommandLists(
        ID3D12CommandQueue* queue,
        UINT num_command_lists,
        ID3D12CommandList* const* command_lists
    );
    static HRESULT APIENTRY HookResizeBuffers(
        IDXGISwapChain* swap_chain,
        UINT buffer_count,
        UINT width,
        UINT height,
        DXGI_FORMAT new_format,
        UINT flags
    );
    static LRESULT CALLBACK HookWndProc(HWND hwnd, UINT msg, WPARAM w_param, LPARAM l_param);

    bool InitializeHook();
    bool InitializeRenderer(IDXGISwapChain* swap_chain);
    bool InitializeSyncObjects();
    bool InstallWindowProcHook();
    bool LoadChineseFont();
    void Render(IDXGISwapChain* swap_chain);
    void ResetRenderState();
    void WaitForFrame(FrameContext& frame);
    void WaitForGpu();
    bool CreateHook(std::uint16_t index, void** original, void* detour);
    void ToggleMenuVisibility();

    static void DrawMenu();

    bool initialized_ = false;
    bool hooked_ = false;
    HWND game_window_ = nullptr;
    WNDPROC original_wnd_proc_ = nullptr;
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

    PresentFn original_present_ = nullptr;
    ExecuteCommandListsFn original_execute_command_lists_ = nullptr;
    ResizeBuffersFn original_resize_buffers_ = nullptr;
};

inline std::unique_ptr<D3D12Overlay> g_D3D12Overlay;

float GetMenuOpacity();
bool IsMenuMinimized();
void SetMenuPreferences(float opacity, bool minimized);

}  // namespace ERD::Main
