param(
    [string]$OutputDir = "build-cl"
)

$ErrorActionPreference = "Stop"

$vswhere = Join-Path ${env:ProgramFiles(x86)} "Microsoft Visual Studio\Installer\vswhere.exe"
if (-not (Test-Path $vswhere)) {
    throw "vswhere.exe not found. Please install Visual Studio or Build Tools."
}

$vsPath = & $vswhere -latest -products * -requires Microsoft.Component.MSBuild -property installationPath
if (-not $vsPath) {
    throw "No Visual Studio installation with MSBuild was found."
}

$vsDevCmd = Join-Path $vsPath "Common7\Tools\VsDevCmd.bat"
if (-not (Test-Path $vsDevCmd)) {
    throw "VsDevCmd.bat was not found at $vsDevCmd"
}

New-Item -ItemType Directory -Path $OutputDir -Force | Out-Null
$absoluteOutputDir = (Resolve-Path $OutputDir).Path

$commandParts = @(
    'call'
    ('"' + $vsDevCmd + '"')
    '-arch=x64'
    '&&'
    'cl'
    '/nologo'
    '/std:c++17'
    '/EHsc'
    '/utf-8'
    '/LD'
    '/ISource'
    '/Ict\\table_files\\include'
    '/DWIN32_LEAN_AND_MEAN'
    '/DNOMINMAX'
    '/DUNICODE'
    '/D_UNICODE'
    ('/Fo' + $absoluteOutputDir + '\')
    'Source\ERDMod.cpp'
    'Source\dllmain.cpp'
    'Source\Main\Config.cpp'
    'Source\Main\D3D12Overlay.cpp'
    'Source\Main\Logger.cpp'
    'Source\Main\Paths.cpp'
    'Source\Main\Runtime.cpp'
    'Source\Game\EventFlags.cpp'
    'Source\Game\SingletonRegistry.cpp'
    'Source\Features\CharacterFlags.cpp'
    'Source\Features\GameActions.cpp'
    'Source\Features\InspectorTools.cpp'
    'Source\Features\MovementMods.cpp'
    'Source\Features\NpcDebug.cpp'
    'Source\Features\ParamPatches.cpp'
    'Source\Features\PostureBars.cpp'
    'Source\Features\Unlocks.cpp'
    'Source\ThirdParty\ImGui\imgui.cpp'
    'Source\ThirdParty\ImGui\imgui_demo.cpp'
    'Source\ThirdParty\ImGui\imgui_draw.cpp'
    'Source\ThirdParty\ImGui\imgui_tables.cpp'
    'Source\ThirdParty\ImGui\imgui_widgets.cpp'
    'Source\ThirdParty\ImGui\imgui_impl_dx12.cpp'
    'Source\ThirdParty\ImGui\imgui_impl_win32.cpp'
    'Source\ThirdParty\MinHook\buffer.c'
    'Source\ThirdParty\MinHook\hde32.c'
    'Source\ThirdParty\MinHook\hde64.c'
    'Source\ThirdParty\MinHook\hook.c'
    'Source\ThirdParty\MinHook\trampoline.c'
    '/link'
    ('/OUT:"' + $absoluteOutputDir + '\ERDMod.dll"')
    ('/IMPLIB:"' + $absoluteOutputDir + '\ERDMod.lib"')
    'user32.lib'
    'd3d12.lib'
    'dxgi.lib'
    'd3dcompiler.lib'
    'dwmapi.lib'
)

$command = $commandParts -join ' '

cmd /c $command
if ($LASTEXITCODE -ne 0) {
    exit $LASTEXITCODE
}

Write-Host "Build completed: $absoluteOutputDir\ERDMod.dll"
