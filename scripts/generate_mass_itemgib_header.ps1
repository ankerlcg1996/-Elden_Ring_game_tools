param(
    [string]$OutputPath = "Source\Features\MassItemGibData.hpp"
)

$ErrorActionPreference = "Stop"

$packageDefs = @(
    @{ Id = 1; Key = "PrattlingPates"; Category = "General"; Label = "Give All Prattling Pates"; Description = "CT: MassItemGib / Give all prattling pates."; RequiresDlc = $false; Path = "ct\Elden-Ring-CT-TGA-master\CheatTable\CheatEntries\Open - The Grand Archives - Elden Ring\Scripts\Build Creation\ItemGib\MassItemGib\Give all prattling pates.cea" }
    @{ Id = 2; Key = "BaseWeapons"; Category = "Base Game / Equipment"; Label = "Give All Weapons"; Description = "CT: MassItemGib / Base Game / Give all weapons."; RequiresDlc = $false; Path = "ct\Elden-Ring-CT-TGA-master\CheatTable\CheatEntries\Open - The Grand Archives - Elden Ring\Scripts\Build Creation\ItemGib\MassItemGib\-- Base Game --\Give all weapons.cea" }
    @{ Id = 3; Key = "BaseProtectors"; Category = "Base Game / Equipment"; Label = "Give All Protectors"; Description = "CT: MassItemGib / Base Game / Give all protectors."; RequiresDlc = $false; Path = "ct\Elden-Ring-CT-TGA-master\CheatTable\CheatEntries\Open - The Grand Archives - Elden Ring\Scripts\Build Creation\ItemGib\MassItemGib\-- Base Game --\Give all protectors.cea" }
    @{ Id = 4; Key = "BaseAccessories"; Category = "Base Game / Equipment"; Label = "Give All Accessories"; Description = "CT: MassItemGib / Base Game / Give all accessories."; RequiresDlc = $false; Path = "ct\Elden-Ring-CT-TGA-master\CheatTable\CheatEntries\Open - The Grand Archives - Elden Ring\Scripts\Build Creation\ItemGib\MassItemGib\-- Base Game --\Give all accessories.cea" }
    @{ Id = 5; Key = "BaseProjectiles"; Category = "Base Game / Equipment"; Label = "Give All Projectiles"; Description = "CT: MassItemGib / Base Game / Give all projectiles."; RequiresDlc = $false; Path = "ct\Elden-Ring-CT-TGA-master\CheatTable\CheatEntries\Open - The Grand Archives - Elden Ring\Scripts\Build Creation\ItemGib\MassItemGib\-- Base Game --\Give all projectiles.cea" }
    @{ Id = 6; Key = "BaseAshesOfWar"; Category = "Base Game / Equipment"; Label = "Give All Ashes Of War"; Description = "CT: MassItemGib / Base Game / Give all ashes of war."; RequiresDlc = $false; Path = "ct\Elden-Ring-CT-TGA-master\CheatTable\CheatEntries\Open - The Grand Archives - Elden Ring\Scripts\Build Creation\ItemGib\MassItemGib\-- Base Game --\Give all ashes of war.cea" }
    @{ Id = 7; Key = "BaseSorceries"; Category = "Base Game / Goods"; Label = "Give All Sorceries"; Description = "CT: MassItemGib / Base Game / Goods / Give all sorceries."; RequiresDlc = $false; Path = "ct\Elden-Ring-CT-TGA-master\CheatTable\CheatEntries\Open - The Grand Archives - Elden Ring\Scripts\Build Creation\ItemGib\MassItemGib\-- Base Game --\-- Goods --\Give all sorceries.cea" }
    @{ Id = 8; Key = "BaseIncantations"; Category = "Base Game / Goods"; Label = "Give All Incantations"; Description = "CT: MassItemGib / Base Game / Goods / Give all incantations."; RequiresDlc = $false; Path = "ct\Elden-Ring-CT-TGA-master\CheatTable\CheatEntries\Open - The Grand Archives - Elden Ring\Scripts\Build Creation\ItemGib\MassItemGib\-- Base Game --\-- Goods --\Give all incantations.cea" }
    @{ Id = 9; Key = "BaseSpiritAshes"; Category = "Base Game / Goods"; Label = "Give All Spirit Ashes"; Description = "CT: MassItemGib / Base Game / Goods / Give all spirit ashes."; RequiresDlc = $false; Path = "ct\Elden-Ring-CT-TGA-master\CheatTable\CheatEntries\Open - The Grand Archives - Elden Ring\Scripts\Build Creation\ItemGib\MassItemGib\-- Base Game --\-- Goods --\Give all spirit ashes.cea" }
    @{ Id = 10; Key = "BaseCrystalTears"; Category = "Base Game / Goods"; Label = "Give All Crystal Tears"; Description = "CT: MassItemGib / Base Game / Goods / Give all crystal tears."; RequiresDlc = $false; Path = "ct\Elden-Ring-CT-TGA-master\CheatTable\CheatEntries\Open - The Grand Archives - Elden Ring\Scripts\Build Creation\ItemGib\MassItemGib\-- Base Game --\-- Goods --\Give all crystal tears.cea" }
    @{ Id = 11; Key = "BaseBellBearings"; Category = "Base Game / Goods"; Label = "Give All Bell Bearings"; Description = "CT: MassItemGib / Base Game / Goods / Give all bell bearings."; RequiresDlc = $false; Path = "ct\Elden-Ring-CT-TGA-master\CheatTable\CheatEntries\Open - The Grand Archives - Elden Ring\Scripts\Build Creation\ItemGib\MassItemGib\-- Base Game --\-- Goods --\Give all bell bearings.cea" }
    @{ Id = 12; Key = "BaseCraftingMaterials"; Category = "Base Game / Goods"; Label = "Give All Crafting Materials"; Description = "CT: MassItemGib / Base Game / Goods / Give all crafting materials."; RequiresDlc = $false; Path = "ct\Elden-Ring-CT-TGA-master\CheatTable\CheatEntries\Open - The Grand Archives - Elden Ring\Scripts\Build Creation\ItemGib\MassItemGib\-- Base Game --\-- Goods --\Give all crafting materials.cea" }
    @{ Id = 13; Key = "BaseUpgradeMaterials"; Category = "Base Game / Goods"; Label = "Give All Upgrade Materials"; Description = "CT: MassItemGib / Base Game / Goods / Give all upgrade materials."; RequiresDlc = $false; Path = "ct\Elden-Ring-CT-TGA-master\CheatTable\CheatEntries\Open - The Grand Archives - Elden Ring\Scripts\Build Creation\ItemGib\MassItemGib\-- Base Game --\-- Goods --\Give all upgrade materials.cea" }
    @{ Id = 14; Key = "BaseLimitedCraftables"; Category = "Base Game / Goods"; Label = "Give All Limited Quantity Craftables"; Description = "CT: MassItemGib / Base Game / Goods / Give all limited quantity craftables."; RequiresDlc = $false; Path = "ct\Elden-Ring-CT-TGA-master\CheatTable\CheatEntries\Open - The Grand Archives - Elden Ring\Scripts\Build Creation\ItemGib\MassItemGib\-- Base Game --\-- Goods --\Give all limited quantity craftables.cea" }
    @{ Id = 15; Key = "BaseMiscConsumables"; Category = "Base Game / Goods"; Label = "Give Misc Consumables"; Description = "CT: MassItemGib / Base Game / Goods / Give misc consumables."; RequiresDlc = $false; Path = "ct\Elden-Ring-CT-TGA-master\CheatTable\CheatEntries\Open - The Grand Archives - Elden Ring\Scripts\Build Creation\ItemGib\MassItemGib\-- Base Game --\-- Goods --\Give misc consumables.cea" }
    @{ Id = 16; Key = "BaseGreases"; Category = "Base Game / Goods"; Label = "Give All Greases"; Description = "CT: MassItemGib / Base Game / Goods / Give all greases."; RequiresDlc = $false; Path = "ct\Elden-Ring-CT-TGA-master\CheatTable\CheatEntries\Open - The Grand Archives - Elden Ring\Scripts\Build Creation\ItemGib\MassItemGib\-- Base Game --\-- Goods --\Give all greases.cea" }
    @{ Id = 17; Key = "DlcWeapons"; Category = "Shadow Of The Erdtree / Equipment"; Label = "Give All Weapons (SotE)"; Description = "CT: MassItemGib / Shadow of the Erdtree / Give all weapons (SotE)."; RequiresDlc = $true; Path = "ct\Elden-Ring-CT-TGA-master\CheatTable\CheatEntries\Open - The Grand Archives - Elden Ring\Scripts\Build Creation\ItemGib\MassItemGib\-- Shadow of the Erdtree --\Give all weapons (SotE).cea" }
    @{ Id = 18; Key = "DlcProtectors"; Category = "Shadow Of The Erdtree / Equipment"; Label = "Give All Protectors (SotE)"; Description = "CT: MassItemGib / Shadow of the Erdtree / Give all protectors (SotE)."; RequiresDlc = $true; Path = "ct\Elden-Ring-CT-TGA-master\CheatTable\CheatEntries\Open - The Grand Archives - Elden Ring\Scripts\Build Creation\ItemGib\MassItemGib\-- Shadow of the Erdtree --\Give all protectors (SotE).cea" }
    @{ Id = 19; Key = "DlcAccessories"; Category = "Shadow Of The Erdtree / Equipment"; Label = "Give All Accessories (SotE)"; Description = "CT: MassItemGib / Shadow of the Erdtree / Give all accessories (SotE)."; RequiresDlc = $true; Path = "ct\Elden-Ring-CT-TGA-master\CheatTable\CheatEntries\Open - The Grand Archives - Elden Ring\Scripts\Build Creation\ItemGib\MassItemGib\-- Shadow of the Erdtree --\Give all accessories (SotE).cea" }
    @{ Id = 20; Key = "DlcProjectiles"; Category = "Shadow Of The Erdtree / Equipment"; Label = "Give All Projectiles (SotE)"; Description = "CT: MassItemGib / Shadow of the Erdtree / Give all projectiles (SotE)."; RequiresDlc = $true; Path = "ct\Elden-Ring-CT-TGA-master\CheatTable\CheatEntries\Open - The Grand Archives - Elden Ring\Scripts\Build Creation\ItemGib\MassItemGib\-- Shadow of the Erdtree --\Give all projectiles (SotE).cea" }
    @{ Id = 21; Key = "DlcAshesOfWar"; Category = "Shadow Of The Erdtree / Equipment"; Label = "Give All Ashes Of War (SotE)"; Description = "CT: MassItemGib / Shadow of the Erdtree / Give all ashes of war (SotE)."; RequiresDlc = $true; Path = "ct\Elden-Ring-CT-TGA-master\CheatTable\CheatEntries\Open - The Grand Archives - Elden Ring\Scripts\Build Creation\ItemGib\MassItemGib\-- Shadow of the Erdtree --\Give all ashes of war (SotE).cea" }
    @{ Id = 22; Key = "DlcSorceries"; Category = "Shadow Of The Erdtree / Goods"; Label = "Give All Sorceries (SotE)"; Description = "CT: MassItemGib / Shadow of the Erdtree / Goods / Give all sorceries (SotE)."; RequiresDlc = $true; Path = "ct\Elden-Ring-CT-TGA-master\CheatTable\CheatEntries\Open - The Grand Archives - Elden Ring\Scripts\Build Creation\ItemGib\MassItemGib\-- Shadow of the Erdtree --\-- Goods --\Give all sorceries (SotE).cea" }
    @{ Id = 23; Key = "DlcIncantations"; Category = "Shadow Of The Erdtree / Goods"; Label = "Give All Incantations (SotE)"; Description = "CT: MassItemGib / Shadow of the Erdtree / Goods / Give all incantations (SotE)."; RequiresDlc = $true; Path = "ct\Elden-Ring-CT-TGA-master\CheatTable\CheatEntries\Open - The Grand Archives - Elden Ring\Scripts\Build Creation\ItemGib\MassItemGib\-- Shadow of the Erdtree --\-- Goods --\Give all incantations (SotE).cea" }
    @{ Id = 24; Key = "DlcSpiritAshes"; Category = "Shadow Of The Erdtree / Goods"; Label = "Give All Spirit Ashes (SotE)"; Description = "CT: MassItemGib / Shadow of the Erdtree / Goods / Give all spirit ashes (SotE)."; RequiresDlc = $true; Path = "ct\Elden-Ring-CT-TGA-master\CheatTable\CheatEntries\Open - The Grand Archives - Elden Ring\Scripts\Build Creation\ItemGib\MassItemGib\-- Shadow of the Erdtree --\-- Goods --\Give all spirit ashes (SotE).cea" }
    @{ Id = 25; Key = "DlcCrystalTears"; Category = "Shadow Of The Erdtree / Goods"; Label = "Give All Crystal Tears (SotE)"; Description = "CT: MassItemGib / Shadow of the Erdtree / Goods / Give all crystal tears (SotE)."; RequiresDlc = $true; Path = "ct\Elden-Ring-CT-TGA-master\CheatTable\CheatEntries\Open - The Grand Archives - Elden Ring\Scripts\Build Creation\ItemGib\MassItemGib\-- Shadow of the Erdtree --\-- Goods --\Give all crystal tears (SotE).cea" }
    @{ Id = 26; Key = "DlcBellBearings"; Category = "Shadow Of The Erdtree / Goods"; Label = "Give All Bell Bearings (SotE)"; Description = "CT: MassItemGib / Shadow of the Erdtree / Goods / Give all bell bearings (SotE)."; RequiresDlc = $true; Path = "ct\Elden-Ring-CT-TGA-master\CheatTable\CheatEntries\Open - The Grand Archives - Elden Ring\Scripts\Build Creation\ItemGib\MassItemGib\-- Shadow of the Erdtree --\-- Goods --\Give all bell bearings (SotE).cea" }
    @{ Id = 27; Key = "DlcCraftingMaterials"; Category = "Shadow Of The Erdtree / Goods"; Label = "Give All Crafting Materials (SotE)"; Description = "CT: MassItemGib / Shadow of the Erdtree / Goods / Give all crafting materials (SotE)."; RequiresDlc = $true; Path = "ct\Elden-Ring-CT-TGA-master\CheatTable\CheatEntries\Open - The Grand Archives - Elden Ring\Scripts\Build Creation\ItemGib\MassItemGib\-- Shadow of the Erdtree --\-- Goods --\Give all crafting materials (SotE).cea" }
    @{ Id = 28; Key = "DlcMiscConsumables"; Category = "Shadow Of The Erdtree / Goods"; Label = "Give Misc Consumables (SotE)"; Description = "CT: MassItemGib / Shadow of the Erdtree / Goods / Give misc consumables (SotE)."; RequiresDlc = $true; Path = "ct\Elden-Ring-CT-TGA-master\CheatTable\CheatEntries\Open - The Grand Archives - Elden Ring\Scripts\Build Creation\ItemGib\MassItemGib\-- Shadow of the Erdtree --\-- Goods --\Give misc consumables (SotE).cea" }
    @{ Id = 29; Key = "DlcGreases"; Category = "Shadow Of The Erdtree / Goods"; Label = "Give All Greases (SotE)"; Description = "CT: MassItemGib / Shadow of the Erdtree / Goods / Give all greases (SotE)."; RequiresDlc = $true; Path = "ct\Elden-Ring-CT-TGA-master\CheatTable\CheatEntries\Open - The Grand Archives - Elden Ring\Scripts\Build Creation\ItemGib\MassItemGib\-- Shadow of the Erdtree --\-- Goods --\Give all greases (SotE).cea" }
    @{ Id = 30; Key = "DlcHeftyPots"; Category = "Shadow Of The Erdtree / Goods"; Label = "Give All Hefty Pots (SotE)"; Description = "CT: MassItemGib / Shadow of the Erdtree / Goods / Give all hefty pots (SotE)."; RequiresDlc = $true; Path = "ct\Elden-Ring-CT-TGA-master\CheatTable\CheatEntries\Open - The Grand Archives - Elden Ring\Scripts\Build Creation\ItemGib\MassItemGib\-- Shadow of the Erdtree --\-- Goods --\Give all hefty pots(SotE).cea" }
    @{ Id = 31; Key = "DlcRemembrances"; Category = "Shadow Of The Erdtree / Goods"; Label = "Give All Remembrances (SotE)"; Description = "CT: MassItemGib / Shadow of the Erdtree / Goods / Give all remembrances (SotE)."; RequiresDlc = $true; Path = "ct\Elden-Ring-CT-TGA-master\CheatTable\CheatEntries\Open - The Grand Archives - Elden Ring\Scripts\Build Creation\ItemGib\MassItemGib\-- Shadow of the Erdtree --\-- Goods --\Give all remembrances (SotE).cea" }
)

function Resolve-TokenValue {
    param(
        [string]$Token,
        [hashtable]$Locals
    )

    if ($Locals.ContainsKey($Token)) {
        return [int]$Locals[$Token]
    }

    if ($Token -match '^0x[0-9A-Fa-f]+$') {
        return [int64]$Token
    }

    return [int]$Token
}

function Parse-ItemFile {
    param(
        [string]$Path
    )

    $content = Get-Content -Raw -LiteralPath $Path
    $locals = @{}

    foreach ($match in [regex]::Matches($content, 'local\s+([A-Za-z_][A-Za-z0-9_]*)\s*=\s*(-?\d+)')) {
        $locals[$match.Groups[1].Value] = [int]$match.Groups[2].Value
    }

    $items = New-Object System.Collections.Generic.List[object]
    $pattern = '\{\s*(0x[0-9A-Fa-f]+|-?\d+|[A-Za-z_][A-Za-z0-9_]*)\s*,\s*(0x[0-9A-Fa-f]+|-?\d+|[A-Za-z_][A-Za-z0-9_]*)\s*,\s*(0x[0-9A-Fa-f]+|-?\d+|[A-Za-z_][A-Za-z0-9_]*)\s*,\s*(0x[0-9A-Fa-f]+|-?\d+|[A-Za-z_][A-Za-z0-9_]*)\s*,\s*(0x[0-9A-Fa-f]+|-?\d+|[A-Za-z_][A-Za-z0-9_]*)\s*\}'
    foreach ($match in [regex]::Matches($content, $pattern)) {
        $items.Add([pscustomobject]@{
            ItemId = [int64](Resolve-TokenValue $match.Groups[1].Value $locals)
            Quantity = [int](Resolve-TokenValue $match.Groups[2].Value $locals)
            ReinforceLevel = [int](Resolve-TokenValue $match.Groups[3].Value $locals)
            Upgrade = [int](Resolve-TokenValue $match.Groups[4].Value $locals)
            Gem = [int](Resolve-TokenValue $match.Groups[5].Value $locals)
        })
    }

    return $items
}

$outputDir = Split-Path -Parent $OutputPath
if ($outputDir) {
    New-Item -ItemType Directory -Path $outputDir -Force | Out-Null
}

$lines = New-Object System.Collections.Generic.List[string]
$lines.Add('#pragma once')
$lines.Add('')
$lines.Add('#include <cstddef>')
$lines.Add('#include <cstdint>')
$lines.Add('')
$lines.Add('namespace ERD::Features {')
$lines.Add('')
$lines.Add('struct MassItemGibItem {')
$lines.Add('    std::uint32_t item_id;')
$lines.Add('    int quantity;')
$lines.Add('    int reinforce_level;')
$lines.Add('    int upgrade;')
$lines.Add('    int gem;')
$lines.Add('};')
$lines.Add('')
$lines.Add('struct MassItemGibPackage {')
$lines.Add('    int id;')
$lines.Add('    const char* category;')
$lines.Add('    const char* label;')
$lines.Add('    const char* description;')
$lines.Add('    bool requires_dlc;')
$lines.Add('    const MassItemGibItem* items;')
$lines.Add('    std::size_t item_count;')
$lines.Add('};')
$lines.Add('')

foreach ($package in $packageDefs) {
    $items = Parse-ItemFile -Path $package.Path
    $arrayName = "k$($package.Key)Items"
    $lines.Add("inline constexpr MassItemGibItem $arrayName" + '[] = {')
    foreach ($item in $items) {
        $lines.Add(("    {{ 0x{0:X8}, {1}, {2}, {3}, {4} }}," -f [uint32]$item.ItemId, $item.Quantity, $item.ReinforceLevel, $item.Upgrade, $item.Gem))
    }
    $lines.Add('};')
    $lines.Add('')
}

$lines.Add('inline constexpr MassItemGibPackage kMassItemGibPackages[] = {')
foreach ($package in $packageDefs) {
    $arrayName = "k$($package.Key)Items"
    $requiresDlc = if ($package.RequiresDlc) { 'true' } else { 'false' }
    $lines.Add(("    {{ {0}, ""{1}"", ""{2}"", ""{3}"", {4}, {5}, sizeof({5}) / sizeof({5}[0]) }}," -f $package.Id, $package.Category, $package.Label, $package.Description, $requiresDlc, $arrayName))
}
$lines.Add('};')
$lines.Add('')
$lines.Add('inline constexpr std::size_t kMassItemGibPackageCount = sizeof(kMassItemGibPackages) / sizeof(kMassItemGibPackages[0]);')
$lines.Add('')
$lines.Add('inline constexpr const MassItemGibPackage* FindMassItemGibPackage(int id) {')
$lines.Add('    for (std::size_t index = 0; index < kMassItemGibPackageCount; ++index) {')
$lines.Add('        if (kMassItemGibPackages[index].id == id) {')
$lines.Add('            return &kMassItemGibPackages[index];')
$lines.Add('        }')
$lines.Add('    }')
$lines.Add('    return nullptr;')
$lines.Add('}')
$lines.Add('')
$lines.Add('}  // namespace ERD::Features')

Set-Content -LiteralPath $OutputPath -Value $lines -Encoding UTF8
Write-Host "Generated $OutputPath"
