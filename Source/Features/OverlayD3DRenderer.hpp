#pragma once

#include <filesystem>
#include <memory>
#include <vector>
#include <cstdint>

#include <d3d12.h>
#include <dxgi1_4.h>
#include <windows.h>

namespace ERD::Features {

class OverlayD3DRenderer {
public:
    static OverlayD3DRenderer& Instance();

    void SetBaseFolder(const std::filesystem::path& folder);
    void Hook();
    void Shutdown();
    bool IsHooked() const { return hooked_; }
    ID3D12DescriptorHeap* GetSrvHeap() const { return srv_heap_; }
    ID3D12Device* GetDevice() const { return device_; }
    const std::filesystem::path& GetBaseFolder() const { return base_folder_; }

private:
    OverlayD3DRenderer();
    ~OverlayD3DRenderer();
    OverlayD3DRenderer(const OverlayD3DRenderer&) = delete;
    OverlayD3DRenderer& operator=(const OverlayD3DRenderer&) = delete;

    using PresentFn = HRESULT(APIENTRY*)(IDXGISwapChain*, UINT, UINT);
    using ExecuteCommandListsFn = void(APIENTRY*)(ID3D12CommandQueue*, UINT, ID3D12CommandList*);
    using ResizeTargetFn = HRESULT(APIENTRY*)(IDXGISwapChain*, const DXGI_MODE_DESC*);

    bool InitHook();
    bool InitWindow();
    bool DeleteWindow();
    bool CreateHook(std::uint16_t index, void** original, void* replacement);
    void DisableHook(std::uint16_t index);
    void DisableAll();

    void Overlay(IDXGISwapChain* swap_chain);
    void ResetRenderState(IDXGISwapChain3* swap_chain = nullptr);

    static HRESULT APIENTRY HookPresent(IDXGISwapChain* swap_chain, UINT sync_interval, UINT flags);
    static void HookExecuteCommandLists(ID3D12CommandQueue* queue, UINT num_command_lists, ID3D12CommandList* command_lists);
    static HRESULT APIENTRY HookResizeTarget(IDXGISwapChain* swap_chain, const DXGI_MODE_DESC* target_parameters);

    std::filesystem::path base_folder_;
    bool hooked_ = false;
    bool initialized_ = false;
    WNDCLASSEXW window_class_{};
    HWND window_hwnd_ = nullptr;

    IDXGISwapChain3* swap_chain_ = nullptr;
    ID3D12Device* device_ = nullptr;
    ID3D12DescriptorHeap* srv_heap_ = nullptr;
    ID3D12DescriptorHeap* rtv_heap_ = nullptr;
    ID3D12CommandAllocator** command_allocators_ = nullptr;
    ID3D12GraphicsCommandList* command_list_ = nullptr;
    ID3D12CommandQueue* command_queue_ = nullptr;
    ID3D12Resource** back_buffers_ = nullptr;
    std::vector<D3D12_CPU_DESCRIPTOR_HANDLE> render_targets_;
    std::uint64_t buffers_count_ = 0;
    WNDPROC old_wndproc_ = nullptr;

    PresentFn original_present_ = nullptr;
    ExecuteCommandListsFn original_execute_command_lists_ = nullptr;
    ResizeTargetFn original_resize_target_ = nullptr;
};

}  // namespace ERD::Features
