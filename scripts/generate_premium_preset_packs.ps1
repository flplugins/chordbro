$ErrorActionPreference = "Stop"

$repoRoot = Split-Path -Path $PSScriptRoot -Parent
$premiumRoot = Join-Path $repoRoot "commercial-content\premium-packs"
$whiteKeyInputs = @(48, 50, 52, 53, 55, 57, 59, 60)
$sharpNames = @("C", "C#", "D", "D#", "E", "F", "F#", "G", "G#", "A", "A#", "B")
$flatNames = @("C", "Db", "D", "Eb", "E", "F", "Gb", "G", "Ab", "A", "Bb", "B")

function Get-NoteName {
    param(
        [int]$PitchClass,
        [string]$Spelling
    )

    $index = ($PitchClass % 12 + 12) % 12

    if ($Spelling -eq "flat") {
        return $flatNames[$index]
    }

    return $sharpNames[$index]
}

function Get-TonicRootMidi {
    param([int]$PitchClass)
    return 36 + (($PitchClass % 12 + 12) % 12)
}

function Get-ChordNotes {
    param(
        [int]$RootMidi,
        [int[]]$Intervals
    )

    $notes = @()

    foreach ($interval in $Intervals) {
        $note = $RootMidi + $interval

        while ($note -lt 21) {
            $note += 12
        }

        while ($note -gt 108) {
            $note -= 12
        }

        if ($notes -notcontains $note) {
            $notes += $note
        }
    }

    $notes | Sort-Object
}

function ConvertTo-XmlSafe {
    param([string]$Value)
    return [System.Security.SecurityElement]::Escape($Value)
}

function Write-PresetFile {
    param(
        [string]$Path,
        [string]$PackName,
        [string]$PresetName,
        [string]$KeyName,
        [string]$Mode,
        [object[]]$ChordEntries
    )

    $lines = @()
    $lines += '<?xml version="1.0" encoding="UTF-8"?>'
    $lines += ''
    $lines += ('<ripchord product="ChordBro" manufacturer="FL Plugins" schemaVersion="2" source="commercial-pack" pack="{0}" preset="{1}" key="{2}" mode="{3}">' -f (ConvertTo-XmlSafe $PackName), (ConvertTo-XmlSafe $PresetName), (ConvertTo-XmlSafe $KeyName), (ConvertTo-XmlSafe $Mode))
    $lines += '  <preset>'

    foreach ($entry in $ChordEntries) {
        $notes = ($entry.Notes | ForEach-Object { $_.ToString() }) -join ";"
        $lines += ('    <input note="{0}">' -f $entry.InputNote)
        $lines += ('      <chord name="{0}" notes="{1}"/>' -f (ConvertTo-XmlSafe $entry.Name), (ConvertTo-XmlSafe $notes))
        $lines += '    </input>'
    }

    $lines += '  </preset>'
    $lines += '</ripchord>'

    $utf8NoBom = New-Object System.Text.UTF8Encoding($false)
    [System.IO.File]::WriteAllLines($Path, $lines, $utf8NoBom)
}

$majorKeys = @(
    @{ Name = "C"; Pc = 0; Spelling = "sharp" },
    @{ Name = "Db"; Pc = 1; Spelling = "flat" },
    @{ Name = "D"; Pc = 2; Spelling = "sharp" },
    @{ Name = "Eb"; Pc = 3; Spelling = "flat" },
    @{ Name = "E"; Pc = 4; Spelling = "sharp" },
    @{ Name = "F"; Pc = 5; Spelling = "flat" },
    @{ Name = "Gb"; Pc = 6; Spelling = "flat" },
    @{ Name = "G"; Pc = 7; Spelling = "sharp" },
    @{ Name = "Ab"; Pc = 8; Spelling = "flat" },
    @{ Name = "A"; Pc = 9; Spelling = "sharp" },
    @{ Name = "Bb"; Pc = 10; Spelling = "flat" },
    @{ Name = "B"; Pc = 11; Spelling = "sharp" }
)

$minorKeys = @(
    @{ Name = "C"; Pc = 0; Spelling = "flat" },
    @{ Name = "C#"; Pc = 1; Spelling = "sharp" },
    @{ Name = "D"; Pc = 2; Spelling = "sharp" },
    @{ Name = "Eb"; Pc = 3; Spelling = "flat" },
    @{ Name = "E"; Pc = 4; Spelling = "sharp" },
    @{ Name = "F"; Pc = 5; Spelling = "flat" },
    @{ Name = "F#"; Pc = 6; Spelling = "sharp" },
    @{ Name = "G"; Pc = 7; Spelling = "sharp" },
    @{ Name = "Ab"; Pc = 8; Spelling = "flat" },
    @{ Name = "A"; Pc = 9; Spelling = "sharp" },
    @{ Name = "Bb"; Pc = 10; Spelling = "flat" },
    @{ Name = "B"; Pc = 11; Spelling = "sharp" }
)

$packDefinitions = @(
    @{
        Folder = "essentials"
        Sku = "cb-pack-essentials"
        Name = "Essentials"
        Tier = "starter-premium"
        Mode = "major"
        Description = "Twelve key-locked songwriter presets with balanced major-key voicings for pop, topline writing, and fast hook sketching."
        PresetLabel = "Songwriter Starter"
        Keys = $majorKeys
        Chords = @(
            @{ Degree = 0; Suffix = "maj9"; Intervals = @(0, 12, 19, 23, 26) },
            @{ Degree = 2; Suffix = "m7"; Intervals = @(0, 12, 15, 19, 22) },
            @{ Degree = 4; Suffix = "m7"; Intervals = @(0, 12, 15, 19, 22) },
            @{ Degree = 5; Suffix = "maj7"; Intervals = @(0, 12, 16, 19, 23) },
            @{ Degree = 7; Suffix = "7sus4"; Intervals = @(0, 12, 17, 19, 22) },
            @{ Degree = 9; Suffix = "m7"; Intervals = @(0, 12, 15, 19, 22) },
            @{ Degree = 11; Suffix = "m7b5"; Intervals = @(0, 12, 18, 22) },
            @{ Degree = 0; Suffix = "6/9"; Intervals = @(0, 12, 16, 19, 21, 26) }
        )
    },
    @{
        Folder = "trap-vault"
        Sku = "cb-pack-trap-vault"
        Name = "Trap Vault"
        Tier = "signature-premium"
        Mode = "minor"
        Description = "Twelve key-locked minor presets built for dark 808 records, tense hooks, and moody melodic trap progressions."
        PresetLabel = "Night Code"
        Keys = $minorKeys
        Chords = @(
            @{ Degree = 0; Suffix = "m(add9)"; Intervals = @(0, 12, 15, 19, 26) },
            @{ Degree = 8; Suffix = "maj7"; Intervals = @(0, 12, 16, 19, 23) },
            @{ Degree = 3; Suffix = "add9"; Intervals = @(0, 12, 16, 19, 26) },
            @{ Degree = 10; Suffix = "sus2"; Intervals = @(0, 12, 14, 19, 26) },
            @{ Degree = 5; Suffix = "m9"; Intervals = @(0, 12, 15, 19, 22, 26) },
            @{ Degree = 7; Suffix = "7(b13)"; Intervals = @(0, 12, 16, 22, 32) },
            @{ Degree = 2; Suffix = "m7b5"; Intervals = @(0, 12, 18, 22) },
            @{ Degree = 0; Suffix = "m11"; Intervals = @(0, 12, 15, 19, 22, 26, 29) }
        )
    },
    @{
        Folder = "rb-velvet"
        Sku = "cb-pack-rb-velvet"
        Name = "R&B Velvet"
        Tier = "signature-premium"
        Mode = "major"
        Description = "Twelve key-locked neo-soul and R&B presets with smooth extensions, borrowed color, and soft top-line support."
        PresetLabel = "Velvet Room"
        Keys = $majorKeys
        Chords = @(
            @{ Degree = 0; Suffix = "maj9"; Intervals = @(0, 12, 16, 19, 23, 26) },
            @{ Degree = 4; Suffix = "m7"; Intervals = @(0, 12, 15, 19, 22) },
            @{ Degree = 9; Suffix = "m9"; Intervals = @(0, 12, 15, 19, 22, 26) },
            @{ Degree = 2; Suffix = "m11"; Intervals = @(0, 12, 15, 19, 22, 26, 29) },
            @{ Degree = 7; Suffix = "13"; Intervals = @(0, 12, 16, 22, 29) },
            @{ Degree = 5; Suffix = "maj9"; Intervals = @(0, 12, 16, 19, 23, 26) },
            @{ Degree = 5; Suffix = "m6"; Intervals = @(0, 12, 15, 19, 21) },
            @{ Degree = 0; Suffix = "6/9"; Intervals = @(0, 12, 16, 19, 21, 26) }
        )
    },
    @{
        Folder = "gospel-colors"
        Sku = "cb-pack-gospel-colors"
        Name = "Gospel Colors"
        Tier = "signature-premium"
        Mode = "major"
        Description = "Twelve key-locked worship and gospel presets with passing diminished movement, strong cadences, and lifted major voicings."
        PresetLabel = "Sunday Lift"
        Keys = $majorKeys
        Chords = @(
            @{ Degree = 0; Suffix = "maj9"; Intervals = @(0, 12, 16, 19, 23, 26) },
            @{ Degree = 1; Suffix = "dim7"; Intervals = @(0, 12, 15, 18, 21) },
            @{ Degree = 2; Suffix = "m9"; Intervals = @(0, 12, 15, 19, 22, 26) },
            @{ Degree = 4; Suffix = "m7"; Intervals = @(0, 12, 15, 19, 22) },
            @{ Degree = 5; Suffix = "maj9"; Intervals = @(0, 12, 16, 19, 23, 26) },
            @{ Degree = 6; Suffix = "dim7"; Intervals = @(0, 12, 15, 18, 21) },
            @{ Degree = 7; Suffix = "13sus4"; Intervals = @(0, 12, 17, 22, 29) },
            @{ Degree = 0; Suffix = "6/9"; Intervals = @(0, 12, 16, 19, 21, 26) }
        )
    },
    @{
        Folder = "dark-cinema"
        Sku = "cb-pack-dark-cinema"
        Name = "Dark Cinema"
        Tier = "signature-premium"
        Mode = "minor"
        Description = "Twelve key-locked cinematic presets focused on suspended tension, low-register weight, and harmonic-minor edge."
        PresetLabel = "Tension Scenes"
        Keys = $minorKeys
        Chords = @(
            @{ Degree = 0; Suffix = "m(add9)"; Intervals = @(0, 12, 15, 19, 26) },
            @{ Degree = 8; Suffix = "maj7(#11)"; Intervals = @(0, 12, 16, 19, 23, 30) },
            @{ Degree = 5; Suffix = "m11"; Intervals = @(0, 12, 15, 19, 22, 26, 29) },
            @{ Degree = 2; Suffix = "m7b5"; Intervals = @(0, 12, 18, 22) },
            @{ Degree = 7; Suffix = "7(b9)"; Intervals = @(0, 12, 16, 22, 25) },
            @{ Degree = 3; Suffix = "add2"; Intervals = @(0, 12, 16, 19, 26) },
            @{ Degree = 10; Suffix = "sus4"; Intervals = @(0, 12, 17, 19, 26) },
            @{ Degree = 0; Suffix = "m(maj9)"; Intervals = @(0, 12, 15, 19, 23, 26) }
        )
    }
)

foreach ($pack in $packDefinitions) {
    $packDir = Join-Path $premiumRoot $pack.Folder
    $presetDir = Join-Path $packDir "presets"

    New-Item -ItemType Directory -Force -Path $presetDir | Out-Null
    Get-ChildItem -Path $presetDir -Filter "*.rpc" -ErrorAction SilentlyContinue | Remove-Item -Force

    $presetFiles = @()

    foreach ($key in $pack.Keys) {
        $presetName = "$($pack.Name) - $($key.Name) $([char]::ToUpper($pack.Mode[0]) + $pack.Mode.Substring(1)) $($pack.PresetLabel)"
        $presetFile = Join-Path $presetDir ($presetName + ".rpc")
        $tonicRootMidi = Get-TonicRootMidi $key.Pc
        $chordEntries = @()

        for ($index = 0; $index -lt $pack.Chords.Count; $index++) {
            $chordDef = $pack.Chords[$index]
            $inputNote = $whiteKeyInputs[$index]
            $rootPc = ($key.Pc + $chordDef.Degree) % 12
            $rootMidi = $tonicRootMidi + $chordDef.Degree
            $chordName = "{0}{1}" -f (Get-NoteName $rootPc $key.Spelling), $chordDef.Suffix
            $chordNotes = Get-ChordNotes $rootMidi $chordDef.Intervals

            $chordEntries += [ordered]@{
                InputNote = $inputNote
                Name = $chordName
                Notes = $chordNotes
            }
        }

        Write-PresetFile -Path $presetFile -PackName $pack.Name -PresetName $presetName -KeyName $key.Name -Mode $pack.Mode -ChordEntries $chordEntries
        $presetFiles += [System.IO.Path]::GetFileName($presetFile)
    }

    $manifest = [ordered]@{
        sku = $pack.Sku
        name = $pack.Name
        tier = $pack.Tier
        format = "rpc"
        status = "ready"
        description = $pack.Description
        bundleEligible = $true
        keyLocked = $true
        installerDefault = $true
        presetCount = $presetFiles.Count
        mode = $pack.Mode
        installSubdirectory = $pack.Name
        keys = @($pack.Keys | ForEach-Object { $_.Name })
        presetFiles = $presetFiles
    }

    $manifestPath = Join-Path $packDir "manifest.json"
    $manifestJson = $manifest | ConvertTo-Json -Depth 5
    $utf8NoBom = New-Object System.Text.UTF8Encoding($false)
    [System.IO.File]::WriteAllText($manifestPath, $manifestJson + [Environment]::NewLine, $utf8NoBom)
}
