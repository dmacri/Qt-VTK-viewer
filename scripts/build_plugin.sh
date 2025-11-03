#!/usr/bin/env bash
# ==============================================================
# build_plugin.sh
#
# Helper script to build a plugin module for OOpenCAL / Qt-VTK Viewer
#
# Usage:
#   ./build_plugin.sh path/to/MyCell.h [--includes /path/to/include] [CMAKE_ARGS...]
#
# Example:
#   ./build_plugin.sh ./BallCell.h \
#       --includes /home/agh/Pulpit/ItaliaSoftware/OOpenCAL/models \
#       -DPLUGIN_MODEL_NAME='"Ball2"' \
#       -DPLUGIN_CELL_CLASS=BallCell \
#       -DOOPENCAL_DIR=/home/agh/Pulpit/ItaliaSoftware \
#       -DQTVTKVIEWER_DIR=/home/agh/Pulpit/ItaliaSoftware/Qt-VTK-viewer
# ==============================================================

set -euo pipefail

# --------------------------------------------------------------
# Utility: print info and error
# --------------------------------------------------------------
die() { echo "❌ Error: $*" >&2; exit 1; }
info() { echo "🔹 $*"; }

# --------------------------------------------------------------
# Parse arguments
# --------------------------------------------------------------
parse_args() {
    if [[ $# -lt 1 ]]; then
        die "Usage: $0 path/to/MyCell.h [--includes /path/to/include] [CMAKE_ARGS...]"
    fi

    HEADER_FILE="$1"
    shift

    EXTRA_INCLUDE_DIR=""
    CMAKE_ARGS=()

    # Parse custom --includes option
    while [[ $# -gt 0 ]]; do
        case "$1" in
            --includes)
                shift
                [[ $# -gt 0 ]] || die "--includes requires a path argument"
                EXTRA_INCLUDE_DIR="$1"
                ;;
            *)
                CMAKE_ARGS+=("$1")
                ;;
        esac
        shift
    done

    [[ -f "$HEADER_FILE" ]] || die "Header file '$HEADER_FILE' not found"

    HEADER_DIR="$(cd "$(dirname "$HEADER_FILE")" && pwd)"
    HEADER_BASE="$(basename "$HEADER_FILE")"
    HEADER_NAME="${HEADER_BASE%.*}"
    BUILD_DIR="${HEADER_DIR}/build"
}

# --------------------------------------------------------------
# Prepare build directory
# --------------------------------------------------------------
prepare_build_dir() {
    info "Preparing build directory: ${BUILD_DIR}"
    mkdir -p "$BUILD_DIR"
}

# --------------------------------------------------------------
# Copy or symlink required files
# --------------------------------------------------------------
copy_required_files() {
    local REQUIRED_FILES=("CMakeLists.txt" "Plugin_FullTemplate.cpp")

    for FILE in "${REQUIRED_FILES[@]}"; do
        if [[ -f "$FILE" ]]; then
            info "Linking $FILE → $BUILD_DIR"
            ln -sf "$(realpath "$FILE")" "$BUILD_DIR/$FILE"
        fi
    done

    info "Linking $HEADER_FILE → $BUILD_DIR"
    ln -sf "$HEADER_FILE" "$BUILD_DIR/$HEADER_BASE"
}

# --------------------------------------------------------------
# Run CMake + make
# --------------------------------------------------------------
run_cmake_build() {
    local cmake_cmd=("cmake" "." "${CMAKE_ARGS[@]}")
    if [[ -n "$EXTRA_INCLUDE_DIR" ]]; then
        cmake_cmd+=("-DEXTRA_INCLUDE_DIR=${EXTRA_INCLUDE_DIR}")
    fi

    info "Running CMake configuration..."
    (
        cd "$BUILD_DIR"
        "${cmake_cmd[@]}"
        info "Building plugin..."
        make -j"$(nproc)"
    )
}

# --------------------------------------------------------------
# Copy output library next to header
# --------------------------------------------------------------
move_output_library() {
    local SO_FILE
    SO_FILE=$(find "$BUILD_DIR" -maxdepth 1 -type f -name "*.so" | head -n 1 || true)
    [[ -n "$SO_FILE" ]] || die "No .so file found in build directory."

    info "Copying result: $(basename "$SO_FILE") → $HEADER_DIR"
    cp -f "$SO_FILE" "$HEADER_DIR/"
}

# --------------------------------------------------------------
# Main
# --------------------------------------------------------------
main() {
    parse_args "$@"
    prepare_build_dir
    copy_required_files
    run_cmake_build
    move_output_library

    info "✅ Plugin build completed successfully."
}

main "$@"
