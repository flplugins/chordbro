param(
    [string]$InnoCompiler = "iscc",
    [string]$ScriptPath = ".\ChordBro.iss"
)

$ErrorActionPreference = "Stop"

if (-not (Get-Command $InnoCompiler -ErrorAction SilentlyContinue)) {
    $commonPaths = @(
        "C:\Program Files (x86)\Inno Setup 6\ISCC.exe",
        "C:\Program Files\Inno Setup 6\ISCC.exe",
        "$env:LOCALAPPDATA\Programs\Inno Setup 6\ISCC.exe"
    )

    $resolvedCompiler = $commonPaths | Where-Object { Test-Path $_ } | Select-Object -First 1

    if (-not $resolvedCompiler) {
        throw "Inno Setup compiler not found. Install Inno Setup or pass -InnoCompiler with the full path to ISCC.exe."
    }

    $InnoCompiler = $resolvedCompiler
}

& $InnoCompiler $ScriptPath
