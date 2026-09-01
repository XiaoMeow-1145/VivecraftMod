#!/bin/bash
# Build libopenvr_api.so - DIRECT LINK mode (matches original library)
# All xr* are GLOBAL UND imports resolved from libopenxr_loader.so at load time

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

echo "=== Building libopenvr_api.so (DIRECT LINK to libopenxr_loader.so) ==="
echo "NDK: ${NDK_HOME}"

rm -f ${OUTPUT}

echo "Compiling & linking..."
# NOTE: We pass libopenxr_loader.so as an INPUT FILE, not with -l / -L
# This tells clang to:
#   (a) add libopenxr_loader.so as NEEDED
#   (b) resolve xr* UND references against it
# Also add GLESv3, vulkan, m, log, android, dl to match original NEEDED list
${CC} -O2 -fPIC -shared \
    -I${SYSROOT}/usr/include \
    ${SOURCES} \
    ${CMAKE_SOURCE_DIR:+./}libopenxr_loader.so \
    -lEGL -lGLESv3 -lvulkan -llog -landroid -ldl -lm \
    -L${LIB_DIR} \
    -Wl,--no-undefined -Wl,-z,noexecstack \
    -o ${OUTPUT}

echo ""
echo "=== Build complete ==="

if [ ! -f "${OUTPUT}" ]; then echo "BUILD FAILED!"; exit 1; fi

file ${OUTPUT}
echo ""

echo "=== NEEDED (should match original) ==="
echo "Expected: libopenxr_loader.so, libEGL.so, libGLESv3.so, libvulkan.so, liblog.so, libandroid.so, libdl.so, libm.so, libc.so"
readelf -d ${OUTPUT} | grep NEEDED
echo ""

echo "=== JNI functions (exported FUNC) ==="
readelf -s --wide ${OUTPUT} | grep -E "FUNC.*GLOBAL DEFAULT [0-9]+.*(Java_|JNI_On)" | head -10
echo ""

echo "=== OpenComposite 3 pointers (OBJECT export, 8 bytes each) ==="
readelf -s --wide ${OUTPUT} | grep -E "OBJECT.*GLOBAL DEFAULT [0-9]+.*OpenComposite" | head -5
echo ""

echo "=== Sample xr* - should be GLOBAL DEFAULT UND ==="
readelf -s --wide ${OUTPUT} | grep -E "UND.*xrCreateInstance|UND.*xrCreateSession|UND.*xrBeginSession|UND.*xrEndFrame|UND.*xrGetInstanceProcAddr" | head -10
COUNT_XR=$(readelf -s --wide ${OUTPUT} | grep -E "FUNC.*GLOBAL.*UND.*xr[A-Z]" | wc -l)
echo ""
echo "Total xr* UND FUNC symbols: ${COUNT_XR}"
echo ""
echo "=== Vulkan UND symbols (should be 3) ==="
readelf -s --wide ${OUTPUT} | grep -E "FUNC.*GLOBAL.*UND.*vk"
echo ""

${STRIP} ${OUTPUT}
echo "Stripped size: $(ls -lh ${OUTPUT} | awk '{print $5}')"