#!/bin/bash
# Build libopenvr_api.so - DYNAMIC LOAD mode (dlopen + dlsym)
# NOT linked against libopenxr_loader.so at build time
# All xr* functions resolved at runtime via dlsym from system's libopenxr_loader.so

set -e

NDK_HOME="/opt/android-ndk-r26d"
TOOLCHAIN="${NDK_HOME}/toolchains/llvm/prebuilt/linux-x86_64"
SYSROOT="${TOOLCHAIN}/sysroot"
API_LEVEL=31

CC="${TOOLCHAIN}/bin/aarch64-linux-android${API_LEVEL}-clang"
STRIP="${TOOLCHAIN}/bin/llvm-strip"
LIB_DIR="${SYSROOT}/usr/lib/aarch64-linux-android/${API_LEVEL}"

OUTPUT="libopenvr_api.so"
SOURCES="openxr_loader_wrapper.c"

echo "=== Building libopenvr_api.so (DYNAMIC LOAD - no libopenxr_loader.so link) ==="
echo "NDK: ${NDK_HOME}"

rm -f ${OUTPUT}

echo "Compiling & linking..."
# NOTE: NOT linking against libopenxr_loader.so!
# xr* functions are resolved at runtime via dlopen + dlsym.
# Only link against system libraries that are always available.
${CC} -O2 -fPIC -shared \
    -I${SYSROOT}/usr/include \
    ${SOURCES} \
    -lEGL -lGLESv3 -lvulkan -llog -landroid -ldl -lm \
    -L${LIB_DIR} \
    -Wl,-z,noexecstack \
    -o ${OUTPUT}

echo ""
echo "=== Build complete ==="

if [ ! -f "${OUTPUT}" ]; then echo "BUILD FAILED!"; exit 1; fi

file ${OUTPUT}
echo ""

echo "=== NEEDED (should NOT have libopenxr_loader.so) ==="
echo "Expected: libEGL.so, libGLESv3.so, libvulkan.so, liblog.so, libandroid.so, libdl.so, libm.so, libc.so"
readelf -d ${OUTPUT} | grep NEEDED
echo ""

echo "=== JNI functions (exported FUNC) ==="
readelf -s --wide ${OUTPUT} | grep -E "FUNC.*GLOBAL DEFAULT [0-9]+.*(Java_|JNI_On)" | head -10
echo ""

echo "=== OpenComposite 3 pointers (OBJECT export, 8 bytes each) ==="
readelf -s --wide ${OUTPUT} | grep -E "OBJECT.*GLOBAL DEFAULT [0-9]+.*OpenComposite" | head -5
echo ""

echo "=== No xr* UND symbols (should be none - all resolved via dlsym) ==="
COUNT_XR=$(readelf -s --wide ${OUTPUT} | grep -E "UND.*xr[A-Z]" | wc -l)
echo "Total xr* UND symbols: ${COUNT_XR} (should be 0)"
echo ""

echo "=== Vulkan UND symbols (should be 3) ==="
readelf -s --wide ${OUTPUT} | grep -E "FUNC.*GLOBAL.*UND.*vk"
echo ""

${STRIP} ${OUTPUT}
echo "Stripped size: $(ls -lh ${OUTPUT} | awk '{print $5}')"