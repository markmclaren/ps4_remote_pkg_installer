#!/usr/bin/env bash
set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
cd "$SCRIPT_DIR"

TARGET="${1:-all}"
MODE="${BUILD_MODE:-auto}" # auto | docker | local

for arg in "$@"; do
    case "$arg" in
        --docker) MODE="docker" ;;
        --local)  MODE="local" ;;
        clean|all|pkg|eboot|sfo) TARGET="$arg" ;;
    esac
done

run_local() {
    if [ -z "${OO_PS4_TOOLCHAIN:-}" ]; then
        echo "Error: OO_PS4_TOOLCHAIN is not set for local build." >&2
        exit 1
    fi
    if [ "$TARGET" = "clean" ]; then
        make -f Makefile.openorbis clean
    elif [ "$TARGET" = "all" ]; then
        make -f Makefile.openorbis clean && make -f Makefile.openorbis all
    else
        make -f Makefile.openorbis "$TARGET"
    fi
}

run_docker() {
    if command -v docker >/dev/null 2>&1 && docker compose version >/dev/null 2>&1; then
        if [ "$TARGET" = "clean" ]; then
            docker compose run --rm builder make -f Makefile.openorbis clean
        elif [ "$TARGET" = "all" ]; then
            docker compose run --rm builder sh -c "make -f Makefile.openorbis clean && make -f Makefile.openorbis all"
        else
            docker compose run --rm builder make -f Makefile.openorbis "$TARGET"
        fi
    elif command -v docker >/dev/null 2>&1; then
        IMAGE_NAME="ps4-remote-pkg-installer-builder:latest"
        docker build -t "$IMAGE_NAME" --build-arg OO_TOOLCHAIN_VERSION=v0.5.2 .
        if [ "$TARGET" = "clean" ]; then
            docker run --rm -v "$SCRIPT_DIR:/src" -w /src "$IMAGE_NAME" make -f Makefile.openorbis clean
        elif [ "$TARGET" = "all" ]; then
            docker run --rm -v "$SCRIPT_DIR:/src" -w /src "$IMAGE_NAME" sh -c "make -f Makefile.openorbis clean && make -f Makefile.openorbis all"
        else
            docker run --rm -v "$SCRIPT_DIR:/src" -w /src "$IMAGE_NAME" make -f Makefile.openorbis "$TARGET"
        fi
    else
        echo "Error: Docker is not installed or not in PATH." >&2
        exit 1
    fi
}

if [ "$MODE" = "local" ]; then
    run_local
elif [ "$MODE" = "docker" ]; then
    run_docker
else
    if [ -n "${OO_PS4_TOOLCHAIN:-}" ] && command -v clang >/dev/null 2>&1; then
        run_local
    else
        run_docker
    fi
fi
