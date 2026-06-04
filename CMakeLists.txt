cmake_minimum_required(VERSION 3.28)

set(CMAKE_GENERATOR_PLATFORM x64)
set(CMAKE_POLICY_VERSION_MINIMUM 3.5)

project(erdGameTools
  VERSION 0.1.0
  LANGUAGES C CXX ASM_MASM)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_INTERPROCEDURAL_OPTIMIZATION TRUE)
set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")

include(FetchContent)

set(SPDLOG_DISABLE_DEFAULT_LOGGER ON)
set(SPDLOG_USE_STD_FORMAT ON)
set(LOCAL_SPDLOG_DIR "${CMAKE_CURRENT_SOURCE_DIR}/build/_deps/spdlog-src")
if (EXISTS "${LOCAL_SPDLOG_DIR}/CMakeLists.txt")
  FetchContent_Declare(spdlog
    SOURCE_DIR "${LOCAL_SPDLOG_DIR}")
else ()
  FetchContent_Declare(spdlog
    GIT_REPOSITORY https://github.com/gabime/spdlog.git
    GIT_TAG v1.13.0)
endif ()

set(ER_WITH_HOOKS ON)
set(LOCAL_ELDEN_X_DIR "${CMAKE_CURRENT_SOURCE_DIR}/build/_deps/elden-x-src")
if (EXISTS "${LOCAL_ELDEN_X_DIR}/CMakeLists.txt")
  FetchContent_Declare(elden-x
    SOURCE_DIR "${LOCAL_ELDEN_X_DIR}")
else ()
  FetchContent_Declare(elden-x
    GIT_REPOSITORY https://github.com/ThomasJClark/elden-x.git
    GIT_TAG 363961dac2858b6bad9ced3900d2b1b0026aee0f)
endif ()

FetchContent_MakeAvailable(spdlog elden-x)

add_library(
  ${PROJECT_NAME} SHARED
  src/grace_test_config.cpp
  src/dllmain.cpp
  src/grace_test_localization.cpp
  src/grace_test_overlay.cpp
  src/grace_test_runtime.cpp
  src/grace_test_messages.cpp
  src/grace_test_plugins.cpp
  src/grace_test_talkscript.cpp
  Source/Game/SingletonRegistry.cpp
  Source/Game/EventFlags.cpp
  Source/Main/FeatureStatus.cpp
  Source/Main/Logger.cpp
  Source/Features/CharacterFlags.cpp
  Source/Features/EzStateTalkCommand.cpp
  Source/Features/AutoPickup.cpp
  Source/Features/ParamPatches.cpp
  Source/Features/InspectorTools.cpp
  Source/Features/RallyHooks.cpp
  Source/Features/SpEffectMonitor.cpp
  Source/Features/OverlayD3DRenderer.cpp
  Source/Features/TargetStatusService.cpp
  Source/Features/AutoPickupExecActionButtonWrapper.asm
  Source/ThirdParty/ImGui/imgui.cpp
  Source/ThirdParty/ImGui/imgui_demo.cpp
  Source/ThirdParty/ImGui/imgui_draw.cpp
  Source/ThirdParty/ImGui/imgui_tables.cpp
  Source/ThirdParty/ImGui/imgui_widgets.cpp
  Source/ThirdParty/ImGui/imgui_impl_dx12.cpp
  Source/ThirdParty/ImGui/imgui_impl_win32.cpp
  Source/ThirdParty/MinHook/buffer.c
  Source/ThirdParty/MinHook/hook.c
  Source/ThirdParty/MinHook/hde32.c
  Source/ThirdParty/MinHook/hde64.c
  Source/ThirdParty/MinHook/trampoline.c)

add_definitions(-DPROJECT_VERSION="${CMAKE_PROJECT_VERSION}")
add_definitions(-D_ITERATOR_DEBUG_LEVEL=0)

target_compile_definitions(
  ${PROJECT_NAME}
  PRIVATE
  ERD_FEATURE_STATUS_EXPORTS)

target_link_libraries(
  ${PROJECT_NAME}
  PRIVATE
  spdlog
  elden-x
  d3d12
  dxgi
  dwmapi)

target_compile_options(
  ${PROJECT_NAME}
  PRIVATE
  /utf-8)

target_include_directories(
  ${PROJECT_NAME}
  PRIVATE
  ${CMAKE_CURRENT_SOURCE_DIR}/src
  ${CMAKE_CURRENT_SOURCE_DIR}/Source)

add_custom_command(
  TARGET ${PROJECT_NAME}
  POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E make_directory "$<TARGET_FILE_DIR:${PROJECT_NAME}>/Resources/Lang"
  COMMAND ${CMAKE_COMMAND} -E make_directory "${CMAKE_CURRENT_SOURCE_DIR}/dist/erdGameTools"
  COMMAND ${CMAKE_COMMAND} -E make_directory "${CMAKE_CURRENT_SOURCE_DIR}/dist/erdGameTools/Resources"
  COMMAND ${CMAKE_COMMAND} -E copy_if_different
          "$<TARGET_FILE:${PROJECT_NAME}>"
          "${CMAKE_CURRENT_SOURCE_DIR}/dist/erdGameTools/$<TARGET_FILE_NAME:${PROJECT_NAME}>"
  COMMAND ${CMAKE_COMMAND} -E copy_if_different
          "${CMAKE_CURRENT_SOURCE_DIR}/erdGameTools.ini"
          "${CMAKE_CURRENT_SOURCE_DIR}/dist/erdGameTools/erdGameTools.ini"
  COMMAND ${CMAKE_COMMAND} -E copy_directory
          "${CMAKE_CURRENT_SOURCE_DIR}/Resources"
          "$<TARGET_FILE_DIR:${PROJECT_NAME}>/Resources"
  COMMAND ${CMAKE_COMMAND} -E copy_directory
          "${CMAKE_CURRENT_SOURCE_DIR}/Resources"
          "${CMAKE_CURRENT_SOURCE_DIR}/dist/erdGameTools/Resources")
