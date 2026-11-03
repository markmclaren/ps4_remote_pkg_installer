@echo off
setlocal EnableDelayedExpansion

pushd "%~dp0"

set "TARGET=all"
set "MODE=auto"

:parse_args
if "%~1"=="" goto :args_done
if /I "%~1"=="--docker" (
    set "MODE=docker"
) else if /I "%~1"=="--local" (
    set "MODE=local"
) else (
    set "TARGET=%~1"
)
shift
goto :parse_args
:args_done

if /I "%MODE%"=="local" goto :run_local
goto :run_docker

:run_local
if "%OO_PS4_TOOLCHAIN%"=="" (
    echo Error: OO_PS4_TOOLCHAIN environment variable is not set.
    popd
    exit /b 1
)
where make >nul 2>nul
if %ERRORLEVEL% equ 0 (
    if /I "%TARGET%"=="clean" (
        make -f Makefile.openorbis clean
    ) else if /I "%TARGET%"=="all" (
        make -f Makefile.openorbis clean && make -f Makefile.openorbis all
    ) else (
        make -f Makefile.openorbis %TARGET%
    )
    set "EXIT_CODE=%ERRORLEVEL%"
    popd
    exit /b !EXIT_CODE!
)
echo Error: 'make' was not found in PATH for local build. Use Docker or run without --local.
popd
exit /b 1

:run_docker
where docker >nul 2>nul
if %ERRORLEVEL% neq 0 (
    echo Error: Docker is not installed or not in PATH.
    popd
    exit /b 1
)

docker compose version >nul 2>nul
if %ERRORLEVEL% equ 0 (
    if /I "%TARGET%"=="clean" (
        docker compose run --rm builder make -f Makefile.openorbis clean
    ) else if /I "%TARGET%"=="all" (
        docker compose run --rm builder sh -c "make -f Makefile.openorbis clean && make -f Makefile.openorbis all"
    ) else (
        docker compose run --rm builder make -f Makefile.openorbis %TARGET%
    )
    set "EXIT_CODE=%ERRORLEVEL%"
    popd
    exit /b !EXIT_CODE!
)

set "IMAGE_NAME=ps4-remote-pkg-installer-builder:latest"
docker build -t "%IMAGE_NAME%" --build-arg OO_TOOLCHAIN_VERSION=v0.5.2 .
if %ERRORLEVEL% neq 0 (
    popd
    exit /b %ERRORLEVEL%
)

if /I "%TARGET%"=="clean" (
    docker run --rm -v "%CD%:/src" -w /src "%IMAGE_NAME%" make -f Makefile.openorbis clean
) else if /I "%TARGET%"=="all" (
    docker run --rm -v "%CD%:/src" -w /src "%IMAGE_NAME%" sh -c "make -f Makefile.openorbis clean && make -f Makefile.openorbis all"
) else (
    docker run --rm -v "%CD%:/src" -w /src "%IMAGE_NAME%" make -f Makefile.openorbis %TARGET%
)
set "EXIT_CODE=%ERRORLEVEL%"
popd
exit /b !EXIT_CODE!
