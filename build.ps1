<#
.SYNOPSIS
    Builds ps4_remote_pkg_installer using Docker/Docker Compose (or a local OpenOrbis PS4 Toolchain).

.PARAMETER Target
    Build target to run: 'all' (default: clean + pkg), 'pkg', 'eboot', 'sfo', or 'clean'.

.PARAMETER Local
    Force building on the host with a locally installed OpenOrbis PS4 Toolchain ($env:OO_PS4_TOOLCHAIN) instead of Docker.
#>
[CmdletBinding()]
param(
    [Parameter(Position = 0)]
    [ValidateSet('all', 'pkg', 'eboot', 'sfo', 'clean')]
    [string]$Target = 'all',

    [Parameter()]
    [switch]$Local
)

$ErrorActionPreference = 'Stop'
Push-Location $PSScriptRoot

try {
    if ($Local) {
        if (-not $env:OO_PS4_TOOLCHAIN) {
            throw "OO_PS4_TOOLCHAIN environment variable is not set."
        }
        if (-not (Get-Command make -ErrorAction SilentlyContinue)) {
            throw "'make' was not found in PATH."
        }

        if ($Target -eq 'clean') {
            & make -f Makefile.openorbis clean
        } elseif ($Target -eq 'all') {
            & make -f Makefile.openorbis clean
            if ($LASTEXITCODE -ne 0) { exit $LASTEXITCODE }
            & make -f Makefile.openorbis all
        } else {
            & make -f Makefile.openorbis $Target
        }
        if ($LASTEXITCODE -ne 0) { exit $LASTEXITCODE }
        return
    }

    if (-not (Get-Command docker -ErrorAction SilentlyContinue)) {
        throw "Docker is not installed or not in PATH."
    }

    & docker info *> $null
    if ($LASTEXITCODE -ne 0) {
        throw "Docker daemon is not running. Please start Docker Desktop and try again."
    }

    & docker compose version *> $null
    $hasCompose = ($LASTEXITCODE -eq 0)

    if ($hasCompose) {
        switch ($Target) {
            'clean' {
                & docker compose run --rm builder make -f Makefile.openorbis clean
            }
            'all' {
                & docker compose run --rm builder sh -c "make -f Makefile.openorbis clean && make -f Makefile.openorbis all"
            }
            default {
                & docker compose run --rm builder make -f Makefile.openorbis $Target
            }
        }
        if ($LASTEXITCODE -ne 0) { exit $LASTEXITCODE }
    } else {
        $imageName = 'ps4-remote-pkg-installer-builder:latest'
        & docker build -t $imageName --build-arg OO_TOOLCHAIN_VERSION=v0.5.2 .
        if ($LASTEXITCODE -ne 0) { exit $LASTEXITCODE }

        if ($Target -eq 'clean') {
            & docker run --rm -v "${PSScriptRoot}:/src" -w /src $imageName make -f Makefile.openorbis clean
        } elseif ($Target -eq 'all') {
            & docker run --rm -v "${PSScriptRoot}:/src" -w /src $imageName sh -c "make -f Makefile.openorbis clean && make -f Makefile.openorbis all"
        } else {
            & docker run --rm -v "${PSScriptRoot}:/src" -w /src $imageName make -f Makefile.openorbis $Target
        }
        if ($LASTEXITCODE -ne 0) { exit $LASTEXITCODE }
    }
} finally {
    Pop-Location
}
