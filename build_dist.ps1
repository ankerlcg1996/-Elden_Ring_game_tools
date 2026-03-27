param(
    [string]$Configuration = "Release"
)

$ErrorActionPreference = "Stop"

$projectRoot = Split-Path -Parent $MyInvocation.MyCommand.Path
$buildDir = Join-Path $projectRoot "build"
$buildOutputDir = Join-Path $buildDir $Configuration
$distDir = Join-Path $projectRoot "dist"
$distLangDir = Join-Path $distDir "Resources\\Lang"

Write-Host "[1/4] Building ($Configuration)..."
cmake --build $buildDir --config $Configuration

$dllPath = Join-Path $buildOutputDir "erdGameTools.dll"
if (-not (Test-Path $dllPath)) {
    throw "Build output not found: $dllPath"
}

Write-Host "[2/4] Preparing dist directory..."
New-Item -ItemType Directory -Force -Path $distLangDir | Out-Null
if (Test-Path (Join-Path $distDir "config.ini")) {
    Remove-Item -Force (Join-Path $distDir "config.ini")
}

Write-Host "[3/4] Copying DLL and language files..."
Copy-Item -Force $dllPath (Join-Path $distDir "erdGameTools.dll")
Copy-Item -Force (Join-Path $projectRoot "Resources\\Lang\\*.txt") $distLangDir

$configCandidates = @(
    (Join-Path $buildOutputDir "erdGameTools.ini"),
    (Join-Path $projectRoot "erdGameTools.ini"),
    (Join-Path $buildOutputDir "config.ini"),
    (Join-Path $projectRoot "config.ini")
)

$configSource = $null
foreach ($candidate in $configCandidates) {
    if (Test-Path $candidate) {
        $configSource = $candidate
        break
    }
}

if ($null -ne $configSource) {
    Write-Host "[4/4] Copying config file from: $configSource"
    Copy-Item -Force $configSource (Join-Path $distDir "erdGameTools.ini")
} else {
    Write-Host "[4/4] No erdGameTools.ini/config.ini found, skipped."
}

Write-Host ""
Write-Host "Done. Dist output:"
Write-Host "  $distDir"
