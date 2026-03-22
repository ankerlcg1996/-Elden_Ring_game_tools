param(
    [string]$CtPath = ".\cn_艾尔登法环CT表v4.25，适用游戏版本1.16.1.ct",
    [string]$OutPath = ".\Resources\Dropdowns\ItemCatalog.zh.txt"
)

$ErrorActionPreference = "Stop"

function Get-DropdownLines {
    param(
        [string]$Content,
        [string]$Description
    )

    $pattern = '<Description>"' + [regex]::Escape($Description) + '"</Description>\s*<DropDownList[^>]*>(?<list>.*?)</DropDownList>'
    $match = [regex]::Match(
        $Content,
        $pattern,
        [System.Text.RegularExpressions.RegexOptions]::Singleline
    )

    if (-not $match.Success) {
        throw "Dropdown not found in CT: $Description"
    }

    return ($match.Groups["list"].Value -split "`r?`n") |
        ForEach-Object { $_.Trim() } |
        Where-Object { $_ -ne "" }
}

if (-not (Test-Path -LiteralPath $CtPath)) {
    throw "CT file not found: $CtPath"
}

$content = Get-Content -Path $CtPath -Raw -Encoding UTF8

$sources = @(
    @{ Kind = "weapon"; Description = "WeaponDropdown(0)" },
    @{ Kind = "protector"; Description = "ArmorDropdown(1)" },
    @{ Kind = "accessory"; Description = "TailsmanDropdown(2)" },
    @{ Kind = "goods"; Description = "Magic/GoodsDropdown(3)" }
)

$outputLines = New-Object System.Collections.Generic.List[string]
$outputLines.Add("# ERDMod Chinese item catalog")
$outputLines.Add("# Source: Chinese Elden Ring CT v4.25 for game 1.16.1")
$outputLines.Add("# Format: kind|ct_raw_id|name")

foreach ($source in $sources) {
    $outputLines.Add("")
    $outputLines.Add("# " + $source.Description)

    foreach ($line in Get-DropdownLines -Content $content -Description $source.Description) {
        $separatorIndex = $line.IndexOf(":")
        if ($separatorIndex -lt 0) {
            continue
        }

        $rawId = $line.Substring(0, $separatorIndex).Trim()
        $name = $line.Substring($separatorIndex + 1).Trim()
        if ([string]::IsNullOrWhiteSpace($rawId) -or [string]::IsNullOrWhiteSpace($name)) {
            continue
        }

        $outputLines.Add("$($source.Kind)|$rawId|$name")
    }
}

$directory = Split-Path -Parent $OutPath
if (-not [string]::IsNullOrWhiteSpace($directory)) {
    New-Item -ItemType Directory -Force -Path $directory | Out-Null
}

$utf8NoBom = New-Object System.Text.UTF8Encoding($false)
[System.IO.File]::WriteAllLines((Resolve-Path -LiteralPath $directory).Path + "\" + (Split-Path -Leaf $OutPath), $outputLines, $utf8NoBom)

Write-Host "Extracted Chinese item catalog to $OutPath"
