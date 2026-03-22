cmake_minimum_required(VERSION 3.21)

# 项目定义：生成一个 64 位 Windows DLL。
project(ERDMod LANGUAGES C CXX)

# 统一使用 C++17，避免不同编译器默认标准不一致。
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# 本 DLL 的源码列表。
add_library(ERDMod SHARED
    Source/ERDMod.cpp
    Source/dllmain.cpp
    Source/Main/Config.cpp
    Source/Main/D3D12Overlay.cpp
    Source/Main/FeatureStatus.hpp
    Source/Main/Logger.cpp
    Source/Main/Paths.cpp
    Source/Main/Runtime.cpp
    Source/Game/EventFlags.cpp
    Source/Game/SingletonRegistry.cpp
    Source/Features/CharacterFlags.cpp
    Source/Features/GameActions.cpp
    Source/Features/InspectorTools.cpp
    Source/Features/MovementMods.cpp
    Source/Features/NpcDebug.cpp
    Source/Features/ParamPatches.cpp
    Source/Features/PostureBars.cpp
    Source/Features/Unlocks.cpp
    Source/ThirdParty/ImGui/imgui.cpp
    Source/ThirdParty/ImGui/imgui_demo.cpp
    Source/ThirdParty/ImGui/imgui_draw.cpp
    Source/ThirdParty/ImGui/imgui_tables.cpp
    Source/ThirdParty/ImGui/imgui_widgets.cpp
    Source/ThirdParty/ImGui/imgui_impl_dx12.cpp
    Source/ThirdParty/ImGui/imgui_impl_win32.cpp
    Source/ThirdParty/MinHook/buffer.c
    Source/ThirdParty/MinHook/hde32.c
    Source/ThirdParty/MinHook/hde64.c
    Source/ThirdParty/MinHook/hook.c
    Source/ThirdParty/MinHook/trampoline.c
)

# 本项目自己的源码目录，以及从 CT 里复用的类型定义。
target_include_directories(ERDMod PRIVATE
    Source
    ct/table_files/include
)

target_link_libraries(ERDMod PRIVATE
    d3d12
    dxgi
    d3dcompiler
    dwmapi
    user32
)

# 常用 Windows 编译宏，减少头文件污染并启用 Unicode。
target_compile_definitions(ERDMod PRIVATE
    WIN32_LEAN_AND_MEAN
    NOMINMAX
    UNICODE
    _UNICODE
)

if(MSVC)
    # MSVC 下开启较严格的警告级别，并显式指定 UTF-8 编码。
    target_compile_options(ERDMod PRIVATE /W4 /EHsc /permissive- /utf-8)
else()
    # 非 MSVC 编译器使用常见的严格警告组合。
    target_compile_options(ERDMod PRIVATE -Wall -Wextra -Wpedantic)
endif()

# 固定输出文件名，方便注入器或加载器直接引用。
set_target_properties(ERDMod PROPERTIES
    OUTPUT_NAME "ERDMod"
)
