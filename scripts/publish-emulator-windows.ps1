#!/usr/bin/env pwsh
# Validate a complete candidate away from the live executable, then replace it
# atomically. A failed build, failed smoke test or locked exe leaves it intact.
[CmdletBinding()]
param(
    [Parameter(Mandatory = $true)][string] $BuiltExecutable,
    [string] $DestinationDirectory = 'C:\.piobuild\numOS\emulator_pc',
    [string[]] $RuntimeDirectory = @()
)
$ErrorActionPreference = 'Stop'
$source = (Resolve-Path -LiteralPath $BuiltExecutable).Path
$destination = [IO.Path]::GetFullPath($DestinationDirectory)
$target = Join-Path $destination 'program.exe'
$repo = Split-Path -Parent $PSScriptRoot
New-Item -ItemType Directory -Path $destination -Force | Out-Null
$stage = Join-Path $destination ('.publish-' + [Guid]::NewGuid().ToString('N'))
New-Item -ItemType Directory -Path $stage | Out-Null
$oldPath = $env:PATH
try {
    $stagedExe = Join-Path $stage 'program.exe'
    Copy-Item -LiteralPath $source -Destination $stagedExe
    $roots = @((Split-Path -Parent $source)) + $RuntimeDirectory + @(
        'C:\SDL2\x86_64-w64-mingw32\bin', 'C:\mingw64\bin', $destination
    )
    if ($env:NUMOS_SDL2_ROOT) { $roots = @((Join-Path $env:NUMOS_SDL2_ROOT 'bin')) + $roots }
    $runtime = @('SDL2.dll', 'libgcc_s_seh-1.dll', 'libstdc++-6.dll', 'libwinpthread-1.dll')
    foreach ($name in $runtime) {
        $found = $null
        foreach ($root in $roots) {
            if (-not $root) { continue }
            $path = Join-Path $root $name
            if (Test-Path -LiteralPath $path -PathType Leaf) { $found = $path; break }
        }
        if (-not $found) { throw "Missing runtime $name. The active emulator has not been changed." }
        Copy-Item -LiteralPath $found -Destination (Join-Path $stage $name)
        $activeDll = Join-Path $destination $name
        if ((Test-Path -LiteralPath $activeDll) -and
            (Get-FileHash -LiteralPath $activeDll).Hash -ne (Get-FileHash -LiteralPath $found).Hash) {
            # WHY: changing a DLL separately from the exe could break a launch
            # between those writes. This publisher requires the pinned runtime.
            throw "Runtime $name differs from the active release. Use its matching runtime; the active emulator is retained."
        }
    }
    $smoke = Join-Path $stage 'smoke.numos'
    [IO.File]::WriteAllText($smoke, "wait 200`nopen_app Calculation`nwait 30`nkey 2`nkey ADD`nkey 3`nkey ENTER`nassert_calc_status ok`nassert_calc_exact 5`nlog EMULATOR_RELEASE_VALIDATED`n", [Text.Encoding]::ASCII)
    $env:PATH = "$stage;$oldPath"
    Push-Location -LiteralPath $repo
    try {
        $output = & $stagedExe --headless --deterministic --quiet --frames 450 --script $smoke 2>&1
        $code = $LASTEXITCODE
    } finally { Pop-Location }
    if ($code -ne 0 -or -not ($output -match 'EMULATOR_RELEASE_VALIDATED')) {
        throw "Candidate smoke test failed (exit $code). The active emulator has not been changed."
    }
    # Existing DLLs are identical and never touched, including while in use.
    foreach ($name in $runtime) {
        $activeDll = Join-Path $destination $name
        if (-not (Test-Path -LiteralPath $activeDll)) {
            Copy-Item -LiteralPath (Join-Path $stage $name) -Destination $activeDll
        }
    }
    if (Test-Path -LiteralPath $target) {
        if ((Get-FileHash -LiteralPath $target).Hash -eq (Get-FileHash -LiteralPath $stagedExe).Hash) {
            Write-Host '[publish-emulator] The validated release is already current.'
        } else {
            # No delete/move of the live exe before replacement. Windows may
            # refuse when it is open; that failure preserves the running version.
            [IO.File]::Replace($stagedExe, $target, (Join-Path $destination 'previous-program.exe'))
            Write-Host "[publish-emulator] Updated: $target"
        }
    } else {
        [IO.File]::Move($stagedExe, $target)
        Write-Host "[publish-emulator] Installed: $target"
    }
} finally {
    $env:PATH = $oldPath
    # Only remove this invocation's private staging folder, never the release.
    $resolvedStage = [IO.Path]::GetFullPath($stage)
    if ($resolvedStage.StartsWith($destination.TrimEnd('\') + '\.publish-', [StringComparison]::OrdinalIgnoreCase)) {
        Remove-Item -LiteralPath $resolvedStage -Recurse -Force
    }
}
