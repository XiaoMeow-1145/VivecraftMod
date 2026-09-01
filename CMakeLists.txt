cmake_minimum_required(VERSION 3.21)
project(openvr_api C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

# Build as a shared library
add_library(openvr_api SHARED
    openxr_loader_wrapper.c
)

# Target Android platform
target_include_directories(openvr_api PRIVATE
    ${CMAKE_SYSROOT}/usr/include
)

# Link required libraries
# Note: libopenxr_loader.so is loaded at runtime via dlopen, not linked directly
target_link_libraries(openvr_api PRIVATE
    EGL
    log
    android
    dl
)

# Ensure the output name matches what the app expects
set_target_properties(openvr_api PROPERTIES
    OUTPUT_NAME "openvr_api"
    PREFIX "lib"
)