#!/bin/bash
# Build script for libopenvr_api.so with runtime OpenXR loading
# Uses Android NDK r26d for ARM64 cross-compilation

set -e

NDK_HOME="/opt/android-ndk-r26d"
TOOLCHAIN="${NDK_HOME}/toolchains/llvm/prebuilt/linux-x86_64"
SYSROOT="${TOOLCHAIN}/sysroot"
API_LEVEL=31

# ARM64 (aarch64) toolchain
CC="${TOOLCHAIN}/bin/aarch64-linux-android${API_LEVEL}-clang"
AR="${TOOLCHAIN}/bin/llvm-ar"
STRIP="${TOOLCHAIN}/bin/llvm-strip"

# Source files
SOURCES="openxr_loader_wrapper.c"

# Output
OUTPUT="libopenvr_api.so"

# Compile flags
CFLAGS="-O2 -fPIC -shared -I${SYSROOT}/usr/include"

# Link flags
LDFLAGS="-shared -Wl,--no-undefined -Wl,-z,noexecstack"
LDFLAGS="${LDFLAGS} -L${SYSROOT}/usr/lib/aarch64-linux-android/${API_LEVEL}"

echo "=== Building libopenvr_api.so with runtime OpenXR loading ==="
echo "NDK: ${NDK_HOME}"
echo "CC: ${CC}"
echo "API Level: ${API_LEVEL}"
echo ""

# Clean old build
rm -f ${OUTPUT}

# Build
echo "Compiling..."
${CC} ${CFLAGS} ${SOURCES} ${LDFLAGS} -o ${OUTPUT} \
    -lEGL -llog -landroid -ldl

echo ""
echo "=== Build complete ==="

# Check if built successfully
if [ -f "${OUTPUT}" ]; then
    echo "Output: ${OUTPUT}"
    file ${OUTPUT}

    echo ""
    echo "=== Checking dependencies ==="
    readelf -d ${OUTPUT} | grep NEEDED

    echo ""
    echo "=== Checking JNI functions ==="
    readelf -s ${OUTPUT} | grep -E "JNI_On|Java_"

    echo ""
    echo "=== Checking xr* symbols (should be exported as real functions) ==="
    readelf -s ${OUTPUT} | grep -E "FUNC.*GLOBAL.*xr" | head -15

    # Strip debug symbols
    echo ""
    echo "Stripping debug symbols..."
    ${STRIP} ${OUTPUT}
    echo "Done - final size: $(ls -lh ${OUTPUT} | awk '{print $5}')"

    echo ""
    echo "=== IMPORTANT ==="
    echo "libopenxr_loader.so must be available on the device at runtime"
    echo "Place it in the same directory as libopenvr_api.so"
else
    echo "BUILD FAILED!"
    exit 1
fi