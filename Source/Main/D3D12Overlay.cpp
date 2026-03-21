#include "D3D12Overlay.hpp"

#include "FeatureStatus.hpp"
#include "Logger.hpp"

#include "../ThirdParty/ImGui/imgui_impl_dx12.h"
#include "../ThirdParty/ImGui/imgui_impl_win32.h"
#include "../ThirdParty/MinHook/MinHook.h"

#include <d3d12.h>
#include <dxgi1_4.h>

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
        g_D3D12Overlay->ToggleMenuVisibility();
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

    if (IsToggleHotkeyPressed()) {
        ToggleMenuVisibility();
    }

    if (!g_FeatureStatus.menu_visible.load()) {
        return;
    }

    const UINT buffer_index = swap_chain_->GetCurrentBackBufferIndex();
    FrameContext& frame = frames_[buffer_index];
    WaitForFrame(frame);

    ImGuiIO& io = ImGui::GetIO();
    io.MouseDrawCursor = true;

    ImGui_ImplDX12_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();
    DrawMenu();
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
        ImGui::TextWrapped("说明：持续功能可以随时开关；一次性动作执行后通常不会自动回滚，正式存档请谨慎。");

        if (g_MenuMinimized) {
            ImGui::Separator();
            ImGui::TextUnformatted("菜单已最小化");
            ImGui::TextDisabled("点击展开可恢复完整功能列表。");
            ImGui::Text("叠加层状态：%s", g_FeatureStatus.overlay_ready.load() ? "已就绪" : "初始化中");
            ImGui::Text("游戏状态：%s", g_FeatureStatus.game_ready.load() ? "角色已加载" : "等待进档");
        } else {
            ImGui::Separator();
            ImGui::TextUnformatted("持续功能");
            DrawBooleanToggle("无限 FP / 不耗蓝", "施法和战技不消耗 FP。", g_FeatureStatus.infinite_fp);
            DrawBooleanToggle("道具不消耗", "使用消耗品时数量不会减少。", g_FeatureStatus.infinite_items);
            DrawBooleanToggle("不耗精力", "攻击、翻滚和奔跑不会消耗精力。", g_FeatureStatus.no_stamina_consumption);
            DrawBooleanToggle("魔法没有使用需求", "把全部魔法的智力、信仰和感应需求归零。", g_FeatureStatus.no_magic_requirements);
            DrawBooleanToggle("所有魔法只占一个栏位", "把全部魔法记忆栏位需求改成单栏位。", g_FeatureStatus.all_magic_one_slot);
            DrawBooleanToggle("装备无重量", "按 CT 原版逻辑把装备重量计算结果直接清零。", g_FeatureStatus.weightless_equipment);
            DrawBooleanToggle("随处骑马", "解除大部分区域的灵马呼唤限制；切图后若失效可再切一次开关。", g_FeatureStatus.mount_anywhere);
            DrawBooleanToggle("随处召唤骨灰", "解除骨灰召唤限制，并同时扩大可召唤判定范围。", g_FeatureStatus.spirit_ashes_anywhere);
            DrawBooleanToggle("商店免费购买", "把商店价格字段和 sellValue 归零，等价于 CT 的免费购买。", g_FeatureStatus.free_purchase);
            DrawBooleanToggle("制作不消耗材料", "迁移 CT 的制作不耗材料脚本，制作道具时不会扣除材料。", g_FeatureStatus.no_crafting_material_cost);
            DrawBooleanToggle("强化不消耗材料", "强化武器时不检查首项强化材料和数量。", g_FeatureStatus.no_upgrade_material_cost);
            DrawIntSlider("掉宝率倍率", "1 为原版，2-100 为倍率增强。", g_FeatureStatus.item_discovery_multiplier, 1, 100, "%d 倍");
            DrawBooleanToggle("常亮提灯", "提灯效果在切图和死亡后也会继续保存。", g_FeatureStatus.permanent_lantern);
            DrawBooleanToggle("头盔隐藏", "隐藏头盔模型，通常需要重新装备头盔后生效。", g_FeatureStatus.invisible_helmets);
            DrawBooleanToggle("更快复活", "死亡后淡出等待时间改为 0。", g_FeatureStatus.faster_respawn);
            DrawBooleanToggle("未清小地牢强制传出", "允许从未清理的小地牢中正常传出。", g_FeatureStatus.warp_out_of_uncleared_minidungeons);

            ImGui::Separator();
            ImGui::TextUnformatted("一次性动作");
            DrawOneShotButton("解锁全部地图", "解锁世界地图和 DLC 地图显示。", g_FeatureStatus.unlock_all_maps, "待执行", "已执行");
            DrawOneShotButton("解锁全部食谱", "一次性解锁全部制作食谱。", g_FeatureStatus.unlock_all_cookbooks, "待执行", "已执行");
            DrawOneShotButton("解锁全部砥石刀", "一次性解锁全部战灰质变用砥石刀。", g_FeatureStatus.unlock_all_whetblades, "待执行", "已执行");
            DrawOneShotButton("解锁全部赐福", "一次性点亮全部赐福，DLC 已购买时也会一起解锁。", g_FeatureStatus.unlock_all_graces, "待执行", "已执行");
            DrawOneShotButton("解锁全部召唤池", "一次性开启全部召唤池，适合联机测试。", g_FeatureStatus.unlock_all_summoning_pools, "待执行", "已执行");
            DrawOneShotButton("解锁全部斗技场", "一次性开放宁姆格福、盖利德和王城斗技场。", g_FeatureStatus.unlock_all_colosseums, "待执行", "已执行");
            DrawOneShotButton("开始下一周目", "向 GameMan 写入 NG+ 请求。建议只在主线通关后手动点击。", g_FeatureStatus.start_next_cycle, "未触发", "已触发");

            ImGui::Separator();
            ImGui::Text("叠加层状态：%s", g_FeatureStatus.overlay_ready.load() ? "已就绪" : "初始化中");
            ImGui::Text("游戏状态：%s", g_FeatureStatus.game_ready.load() ? "角色已加载" : "等待进档");
            ImGui::TextDisabled("如果菜单仍然明显卡顿，优先把游戏切到无边框窗口后再测试。");
        }
    }
    ImGui::End();

    g_FeatureStatus.menu_visible.store(menu_visible);
}

}  // namespace ERD::Main
