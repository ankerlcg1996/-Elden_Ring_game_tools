param(
    [string]$BuildDir = "build-cl",
    [string]$OutputDir = "dist\\mods"
)

$ErrorActionPreference = "Stop"

$dllPath = Join-Path $BuildDir "ERDMod.dll"
if (-not (Test-Path $dllPath)) {
    throw "ERDMod.dll not found in $BuildDir. Build the project first."
}

New-Item -ItemType Directory -Path $OutputDir -Force | Out-Null

$targetDll = Join-Path $OutputDir "ERDMod.dll"
$targetConfigDir = Join-Path $OutputDir "Config"
$targetResourcesDir = Join-Path $OutputDir "Resources"

if (Test-Path $targetDll) {
    Remove-Item $targetDll -Force
}
if (Test-Path $targetConfigDir) {
    Remove-Item $targetConfigDir -Recurse -Force
}
if (Test-Path $targetResourcesDir) {
    Remove-Item $targetResourcesDir -Recurse -Force
}

New-Item -ItemType Directory -Path $targetConfigDir -Force | Out-Null
New-Item -ItemType Directory -Path $targetResourcesDir -Force | Out-Null

Copy-Item $dllPath $targetDll -Force
Copy-Item "Config\\*" $targetConfigDir -Recurse -Force
Copy-Item "Resources\\*" $targetResourcesDir -Recurse -Force

Write-Host "Packaged mod to $(Resolve-Path $OutputDir)"
