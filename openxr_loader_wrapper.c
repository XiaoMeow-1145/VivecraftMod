// OpenXR Loader Wrapper for Android
// Provides all xr* symbols by loading libopenxr_loader.so at runtime via dlopen/dlsym
// This allows the .so to be loaded by the Android linker without unresolved symbols.

#include <dlfcn.h>
#include <stdint.h>
#include <stddef.h>
#include <stdlib.h>

// OpenXR types (minimal subset needed for function signatures)
typedef uint64_t XrInstance;
typedef uint64_t XrSession;
typedef uint64_t XrSpace;
typedef uint64_t XrAction;
typedef uint64_t XrActionSet;
typedef uint64_t XrSwapchain;
typedef uint64_t XrPath;
typedef uint64_t XrActionSpace;
typedef uint32_t XrResult;
typedef uint32_t XrTime;
typedef uint32_t XrViewConfigurationType;
typedef uint32_t XrReferenceSpaceType;



// Android extern variables - provide default NULL values
// These are set by the application at runtime
#include <stddef.h>

// Minimal OpenXR types for Android
typedef struct XrInstanceCreateInfoAndroidKHR {
    void* type;
    const void* next;
    void* createFlags;
    void* applicationVM;
    void* applicationActivity;
} XrInstanceCreateInfoAndroidKHR;

typedef struct XrGraphicsBindingOpenGLESAndroidKHR {
    void* type;
    const void* next;
    void* display;
    void* config;
    void* context;
} XrGraphicsBindingOpenGLESAndroidKHR;

// Default NULL values (application must set these at runtime)
XrInstanceCreateInfoAndroidKHR* OpenComposite_Android_Create_Info = NULL;
XrGraphicsBindingOpenGLESAndroidKHR* OpenComposite_Android_GLES_Binding_Info = NULL;
static void* openxr_handle = NULL;
static int openxr_initialized = 0;
static int openxr_init_failed = 0;

static void* get_xr_func(const char* name) {
    if (!openxr_initialized) {
        openxr_initialized = 1;
        openxr_handle = dlopen("libopenxr_loader.so", RTLD_NOW | RTLD_GLOBAL);
        if (!openxr_handle) {
            openxr_init_failed = 1;
            return NULL;
        }
    }
    if (openxr_init_failed) return NULL;
    return dlsym(openxr_handle, name);
}

XrResult xrCreateInstance(const void* createInfo, void* instance) {
    static XrResult (*real_fn)(const void* createInfo, void* instance) = NULL;
    if (!real_fn) {
        void* sym = get_xr_func("xrCreateInstance");
        if (sym) {
            real_fn = (XrResult (*)(const void* createInfo, void* instance))sym;
        } else {
            return 0;
        }
    }
    return real_fn(createInfo, instance);
}

XrResult xrDestroyInstance(XrInstance instance) {
    static XrResult (*real_fn)(XrInstance instance) = NULL;
    if (!real_fn) {
        void* sym = get_xr_func("xrDestroyInstance");
        if (sym) {
            real_fn = (XrResult (*)(XrInstance instance))sym;
        } else {
            return 0;
        }
    }
    return real_fn(instance);
}

XrResult xrGetInstanceProcAddr(XrInstance instance, const char* name, void** functionPtr) {
    static XrResult (*real_fn)(XrInstance instance, const char* name, void** functionPtr) = NULL;
    if (!real_fn) {
        void* sym = get_xr_func("xrGetInstanceProcAddr");
        if (sym) {
            real_fn = (XrResult (*)(XrInstance instance, const char* name, void** functionPtr))sym;
        } else {
            return 0;
        }
    }
    return real_fn(instance, name, functionPtr);
}

XrResult xrEnumerateApiLayerProperties(uint32_t propertyCapacityInput, uint32_t* propertyCountOutput, void* properties) {
    static XrResult (*real_fn)(uint32_t propertyCapacityInput, uint32_t* propertyCountOutput, void* properties) = NULL;
    if (!real_fn) {
        void* sym = get_xr_func("xrEnumerateApiLayerProperties");
        if (sym) {
            real_fn = (XrResult (*)(uint32_t propertyCapacityInput, uint32_t* propertyCountOutput, void* properties))sym;
        } else {
            return 0;
        }
    }
    return real_fn(propertyCapacityInput, propertyCountOutput, properties);
}

XrResult xrEnumerateInstanceExtensionProperties(const char* layerName, uint32_t propertyCapacityInput, uint32_t* propertyCountOutput, void* properties) {
    static XrResult (*real_fn)(const char* layerName, uint32_t propertyCapacityInput, uint32_t* propertyCountOutput, void* properties) = NULL;
    if (!real_fn) {
        void* sym = get_xr_func("xrEnumerateInstanceExtensionProperties");
        if (sym) {
            real_fn = (XrResult (*)(const char* layerName, uint32_t propertyCapacityInput, uint32_t* propertyCountOutput, void* properties))sym;
        } else {
            return 0;
        }
    }
    return real_fn(layerName, propertyCapacityInput, propertyCountOutput, properties);
}

XrResult xrGetSystem(XrInstance instance, const void* getInfo, void* systemId) {
    static XrResult (*real_fn)(XrInstance instance, const void* getInfo, void* systemId) = NULL;
    if (!real_fn) {
        void* sym = get_xr_func("xrGetSystem");
        if (sym) {
            real_fn = (XrResult (*)(XrInstance instance, const void* getInfo, void* systemId))sym;
        } else {
            return 0;
        }
    }
    return real_fn(instance, getInfo, systemId);
}

XrResult xrGetSystemProperties(XrInstance instance, uint64_t systemId, void* properties) {
    static XrResult (*real_fn)(XrInstance instance, uint64_t systemId, void* properties) = NULL;
    if (!real_fn) {
        void* sym = get_xr_func("xrGetSystemProperties");
        if (sym) {
            real_fn = (XrResult (*)(XrInstance instance, uint64_t systemId, void* properties))sym;
        } else {
            return 0;
        }
    }
    return real_fn(instance, systemId, properties);
}

XrResult xrCreateSession(XrInstance instance, const void* createInfo, void* session) {
    static XrResult (*real_fn)(XrInstance instance, const void* createInfo, void* session) = NULL;
    if (!real_fn) {
        void* sym = get_xr_func("xrCreateSession");
        if (sym) {
            real_fn = (XrResult (*)(XrInstance instance, const void* createInfo, void* session))sym;
        } else {
            return 0;
        }
    }
    return real_fn(instance, createInfo, session);
}

XrResult xrDestroySession(XrSession session) {
    static XrResult (*real_fn)(XrSession session) = NULL;
    if (!real_fn) {
        void* sym = get_xr_func("xrDestroySession");
        if (sym) {
            real_fn = (XrResult (*)(XrSession session))sym;
        } else {
            return 0;
        }
    }
    return real_fn(session);
}

XrResult xrBeginSession(XrSession session, const void* beginInfo) {
    static XrResult (*real_fn)(XrSession session, const void* beginInfo) = NULL;
    if (!real_fn) {
        void* sym = get_xr_func("xrBeginSession");
        if (sym) {
            real_fn = (XrResult (*)(XrSession session, const void* beginInfo))sym;
        } else {
            return 0;
        }
    }
    return real_fn(session, beginInfo);
}

XrResult xrEndSession(XrSession session) {
    static XrResult (*real_fn)(XrSession session) = NULL;
    if (!real_fn) {
        void* sym = get_xr_func("xrEndSession");
        if (sym) {
            real_fn = (XrResult (*)(XrSession session))sym;
        } else {
            return 0;
        }
    }
    return real_fn(session);
}

XrResult xrRequestExitSession(XrSession session) {
    static XrResult (*real_fn)(XrSession session) = NULL;
    if (!real_fn) {
        void* sym = get_xr_func("xrRequestExitSession");
        if (sym) {
            real_fn = (XrResult (*)(XrSession session))sym;
        } else {
            return 0;
        }
    }
    return real_fn(session);
}

XrResult xrWaitFrame(XrSession session, const void* frameWaitInfo, void* frameState) {
    static XrResult (*real_fn)(XrSession session, const void* frameWaitInfo, void* frameState) = NULL;
    if (!real_fn) {
        void* sym = get_xr_func("xrWaitFrame");
        if (sym) {
            real_fn = (XrResult (*)(XrSession session, const void* frameWaitInfo, void* frameState))sym;
        } else {
            return 0;
        }
    }
    return real_fn(session, frameWaitInfo, frameState);
}

XrResult xrBeginFrame(XrSession session, const void* frameBeginInfo) {
    static XrResult (*real_fn)(XrSession session, const void* frameBeginInfo) = NULL;
    if (!real_fn) {
        void* sym = get_xr_func("xrBeginFrame");
        if (sym) {
            real_fn = (XrResult (*)(XrSession session, const void* frameBeginInfo))sym;
        } else {
            return 0;
        }
    }
    return real_fn(session, frameBeginInfo);
}

XrResult xrEndFrame(XrSession session, const void* frameEndInfo) {
    static XrResult (*real_fn)(XrSession session, const void* frameEndInfo) = NULL;
    if (!real_fn) {
        void* sym = get_xr_func("xrEndFrame");
        if (sym) {
            real_fn = (XrResult (*)(XrSession session, const void* frameEndInfo))sym;
        } else {
            return 0;
        }
    }
    return real_fn(session, frameEndInfo);
}

XrResult xrCreateReferenceSpace(XrSession session, const void* createInfo, void* space) {
    static XrResult (*real_fn)(XrSession session, const void* createInfo, void* space) = NULL;
    if (!real_fn) {
        void* sym = get_xr_func("xrCreateReferenceSpace");
        if (sym) {
            real_fn = (XrResult (*)(XrSession session, const void* createInfo, void* space))sym;
        } else {
            return 0;
        }
    }
    return real_fn(session, createInfo, space);
}

XrResult xrDestroySpace(XrSpace space) {
    static XrResult (*real_fn)(XrSpace space) = NULL;
    if (!real_fn) {
        void* sym = get_xr_func("xrDestroySpace");
        if (sym) {
            real_fn = (XrResult (*)(XrSpace space))sym;
        } else {
            return 0;
        }
    }
    return real_fn(space);
}

XrResult xrLocateSpace(XrSpace space, XrSpace baseSpace, XrTime time, void* location) {
    static XrResult (*real_fn)(XrSpace space, XrSpace baseSpace, XrTime time, void* location) = NULL;
    if (!real_fn) {
        void* sym = get_xr_func("xrLocateSpace");
        if (sym) {
            real_fn = (XrResult (*)(XrSpace space, XrSpace baseSpace, XrTime time, void* location))sym;
        } else {
            return 0;
        }
    }
    return real_fn(space, baseSpace, time, location);
}

XrResult xrCreateActionSet(XrInstance instance, const void* createInfo, void* actionSet) {
    static XrResult (*real_fn)(XrInstance instance, const void* createInfo, void* actionSet) = NULL;
    if (!real_fn) {
        void* sym = get_xr_func("xrCreateActionSet");
        if (sym) {
            real_fn = (XrResult (*)(XrInstance instance, const void* createInfo, void* actionSet))sym;
        } else {
            return 0;
        }
    }
    return real_fn(instance, createInfo, actionSet);
}

XrResult xrDestroyActionSet(XrActionSet actionSet) {
    static XrResult (*real_fn)(XrActionSet actionSet) = NULL;
    if (!real_fn) {
        void* sym = get_xr_func("xrDestroyActionSet");
        if (sym) {
            real_fn = (XrResult (*)(XrActionSet actionSet))sym;
        } else {
            return 0;
        }
    }
    return real_fn(actionSet);
}

XrResult xrCreateAction(XrActionSet actionSet, const void* createInfo, void* action) {
    static XrResult (*real_fn)(XrActionSet actionSet, const void* createInfo, void* action) = NULL;
    if (!real_fn) {
        void* sym = get_xr_func("xrCreateAction");
        if (sym) {
            real_fn = (XrResult (*)(XrActionSet actionSet, const void* createInfo, void* action))sym;
        } else {
            return 0;
        }
    }
    return real_fn(actionSet, createInfo, action);
}

XrResult xrDestroyAction(XrAction action) {
    static XrResult (*real_fn)(XrAction action) = NULL;
    if (!real_fn) {
        void* sym = get_xr_func("xrDestroyAction");
        if (sym) {
            real_fn = (XrResult (*)(XrAction action))sym;
        } else {
            return 0;
        }
    }
    return real_fn(action);
}

XrResult xrSuggestInteractionProfileBindings(XrInstance instance, const void* suggestedBindings) {
    static XrResult (*real_fn)(XrInstance instance, const void* suggestedBindings) = NULL;
    if (!real_fn) {
        void* sym = get_xr_func("xrSuggestInteractionProfileBindings");
        if (sym) {
            real_fn = (XrResult (*)(XrInstance instance, const void* suggestedBindings))sym;
        } else {
            return 0;
        }
    }
    return real_fn(instance, suggestedBindings);
}

XrResult xrAttachSessionActionSets(XrSession session, const void* attachInfo) {
    static XrResult (*real_fn)(XrSession session, const void* attachInfo) = NULL;
    if (!real_fn) {
        void* sym = get_xr_func("xrAttachSessionActionSets");
        if (sym) {
            real_fn = (XrResult (*)(XrSession session, const void* attachInfo))sym;
        } else {
            return 0;
        }
    }
    return real_fn(session, attachInfo);
}

XrResult xrSyncActions(XrSession session, const void* syncInfo) {
    static XrResult (*real_fn)(XrSession session, const void* syncInfo) = NULL;
    if (!real_fn) {
        void* sym = get_xr_func("xrSyncActions");
        if (sym) {
            real_fn = (XrResult (*)(XrSession session, const void* syncInfo))sym;
        } else {
            return 0;
        }
    }
    return real_fn(session, syncInfo);
}

XrResult xrEnumerateBoundSourcesForAction(XrSession session, const void* enumerateInfo, uint32_t sourceCapacityInput, uint32_t* sourceCountOutput, void* sources) {
    static XrResult (*real_fn)(XrSession session, const void* enumerateInfo, uint32_t sourceCapacityInput, uint32_t* sourceCountOutput, void* sources) = NULL;
    if (!real_fn) {
        void* sym = get_xr_func("xrEnumerateBoundSourcesForAction");
        if (sym) {
            real_fn = (XrResult (*)(XrSession session, const void* enumerateInfo, uint32_t sourceCapacityInput, uint32_t* sourceCountOutput, void* sources))sym;
        } else {
            return 0;
        }
    }
    return real_fn(session, enumerateInfo, sourceCapacityInput, sourceCountOutput, sources);
}

XrResult xrGetCurrentInteractionProfile(XrSession session, XrPath topLevelUserPath, void* interactionProfile) {
    static XrResult (*real_fn)(XrSession session, XrPath topLevelUserPath, void* interactionProfile) = NULL;
    if (!real_fn) {
        void* sym = get_xr_func("xrGetCurrentInteractionProfile");
        if (sym) {
            real_fn = (XrResult (*)(XrSession session, XrPath topLevelUserPath, void* interactionProfile))sym;
        } else {
            return 0;
        }
    }
    return real_fn(session, topLevelUserPath, interactionProfile);
}

XrResult xrGetActionStateBoolean(XrSession session, const void* getInfo, void* state) {
    static XrResult (*real_fn)(XrSession session, const void* getInfo, void* state) = NULL;
    if (!real_fn) {
        void* sym = get_xr_func("xrGetActionStateBoolean");
        if (sym) {
            real_fn = (XrResult (*)(XrSession session, const void* getInfo, void* state))sym;
        } else {
            return 0;
        }
    }
    return real_fn(session, getInfo, state);
}

XrResult xrGetActionStateFloat(XrSession session, const void* getInfo, void* state) {
    static XrResult (*real_fn)(XrSession session, const void* getInfo, void* state) = NULL;
    if (!real_fn) {
        void* sym = get_xr_func("xrGetActionStateFloat");
        if (sym) {
            real_fn = (XrResult (*)(XrSession session, const void* getInfo, void* state))sym;
        } else {
            return 0;
        }
    }
    return real_fn(session, getInfo, state);
}

XrResult xrGetActionStateVector2f(XrSession session, const void* getInfo, void* state) {
    static XrResult (*real_fn)(XrSession session, const void* getInfo, void* state) = NULL;
    if (!real_fn) {
        void* sym = get_xr_func("xrGetActionStateVector2f");
        if (sym) {
            real_fn = (XrResult (*)(XrSession session, const void* getInfo, void* state))sym;
        } else {
            return 0;
        }
    }
    return real_fn(session, getInfo, state);
}

XrResult xrStringToPath(XrInstance instance, const char* pathString, void* path) {
    static XrResult (*real_fn)(XrInstance instance, const char* pathString, void* path) = NULL;
    if (!real_fn) {
        void* sym = get_xr_func("xrStringToPath");
        if (sym) {
            real_fn = (XrResult (*)(XrInstance instance, const char* pathString, void* path))sym;
        } else {
            return 0;
        }
    }
    return real_fn(instance, pathString, path);
}

XrResult xrPathToString(XrInstance instance, XrPath path, uint32_t bufferCapacityInput, uint32_t* bufferCountOutput, char* buffer) {
    static XrResult (*real_fn)(XrInstance instance, XrPath path, uint32_t bufferCapacityInput, uint32_t* bufferCountOutput, char* buffer) = NULL;
    if (!real_fn) {
        void* sym = get_xr_func("xrPathToString");
        if (sym) {
            real_fn = (XrResult (*)(XrInstance instance, XrPath path, uint32_t bufferCapacityInput, uint32_t* bufferCountOutput, char* buffer))sym;
        } else {
            return 0;
        }
    }
    return real_fn(instance, path, bufferCapacityInput, bufferCountOutput, buffer);
}

XrResult xrPollEvent(XrInstance instance, void* eventData) {
    static XrResult (*real_fn)(XrInstance instance, void* eventData) = NULL;
    if (!real_fn) {
        void* sym = get_xr_func("xrPollEvent");
        if (sym) {
            real_fn = (XrResult (*)(XrInstance instance, void* eventData))sym;
        } else {
            return 0;
        }
    }
    return real_fn(instance, eventData);
}

XrResult xrResultToString(XrInstance instance, XrResult value, void* buffer) {
    static XrResult (*real_fn)(XrInstance instance, XrResult value, void* buffer) = NULL;
    if (!real_fn) {
        void* sym = get_xr_func("xrResultToString");
        if (sym) {
            real_fn = (XrResult (*)(XrInstance instance, XrResult value, void* buffer))sym;
        } else {
            return 0;
        }
    }
    return real_fn(instance, value, buffer);
}

XrResult xrCreateSwapchain(XrSession session, const void* createInfo, void* swapchain) {
    static XrResult (*real_fn)(XrSession session, const void* createInfo, void* swapchain) = NULL;
    if (!real_fn) {
        void* sym = get_xr_func("xrCreateSwapchain");
        if (sym) {
            real_fn = (XrResult (*)(XrSession session, const void* createInfo, void* swapchain))sym;
        } else {
            return 0;
        }
    }
    return real_fn(session, createInfo, swapchain);
}

XrResult xrDestroySwapchain(XrSwapchain swapchain) {
    static XrResult (*real_fn)(XrSwapchain swapchain) = NULL;
    if (!real_fn) {
        void* sym = get_xr_func("xrDestroySwapchain");
        if (sym) {
            real_fn = (XrResult (*)(XrSwapchain swapchain))sym;
        } else {
            return 0;
        }
    }
    return real_fn(swapchain);
}

XrResult xrEnumerateSwapchainFormats(XrSession session, uint32_t formatCapacityInput, uint32_t* formatCountOutput, void* formats) {
    static XrResult (*real_fn)(XrSession session, uint32_t formatCapacityInput, uint32_t* formatCountOutput, void* formats) = NULL;
    if (!real_fn) {
        void* sym = get_xr_func("xrEnumerateSwapchainFormats");
        if (sym) {
            real_fn = (XrResult (*)(XrSession session, uint32_t formatCapacityInput, uint32_t* formatCountOutput, void* formats))sym;
        } else {
            return 0;
        }
    }
    return real_fn(session, formatCapacityInput, formatCountOutput, formats);
}

XrResult xrEnumerateSwapchainImages(XrSwapchain swapchain, uint32_t imageCapacityInput, uint32_t* imageCountOutput, void* images) {
    static XrResult (*real_fn)(XrSwapchain swapchain, uint32_t imageCapacityInput, uint32_t* imageCountOutput, void* images) = NULL;
    if (!real_fn) {
        void* sym = get_xr_func("xrEnumerateSwapchainImages");
        if (sym) {
            real_fn = (XrResult (*)(XrSwapchain swapchain, uint32_t imageCapacityInput, uint32_t* imageCountOutput, void* images))sym;
        } else {
            return 0;
        }
    }
    return real_fn(swapchain, imageCapacityInput, imageCountOutput, images);
}

XrResult xrAcquireSwapchainImage(XrSwapchain swapchain, const void* acquireInfo, void* index) {
    static XrResult (*real_fn)(XrSwapchain swapchain, const void* acquireInfo, void* index) = NULL;
    if (!real_fn) {
        void* sym = get_xr_func("xrAcquireSwapchainImage");
        if (sym) {
            real_fn = (XrResult (*)(XrSwapchain swapchain, const void* acquireInfo, void* index))sym;
        } else {
            return 0;
        }
    }
    return real_fn(swapchain, acquireInfo, index);
}

XrResult xrWaitSwapchainImage(XrSwapchain swapchain, const void* waitInfo) {
    static XrResult (*real_fn)(XrSwapchain swapchain, const void* waitInfo) = NULL;
    if (!real_fn) {
        void* sym = get_xr_func("xrWaitSwapchainImage");
        if (sym) {
            real_fn = (XrResult (*)(XrSwapchain swapchain, const void* waitInfo))sym;
        } else {
            return 0;
        }
    }
    return real_fn(swapchain, waitInfo);
}

XrResult xrReleaseSwapchainImage(XrSwapchain swapchain, const void* releaseInfo) {
    static XrResult (*real_fn)(XrSwapchain swapchain, const void* releaseInfo) = NULL;
    if (!real_fn) {
        void* sym = get_xr_func("xrReleaseSwapchainImage");
        if (sym) {
            real_fn = (XrResult (*)(XrSwapchain swapchain, const void* releaseInfo))sym;
        } else {
            return 0;
        }
    }
    return real_fn(swapchain, releaseInfo);
}

XrResult xrEnumerateViewConfigurationViews(XrInstance instance, uint64_t systemId, XrViewConfigurationType viewConfigurationType, uint32_t viewCapacityInput, uint32_t* viewCountOutput, void* views) {
    static XrResult (*real_fn)(XrInstance instance, uint64_t systemId, XrViewConfigurationType viewConfigurationType, uint32_t viewCapacityInput, uint32_t* viewCountOutput, void* views) = NULL;
    if (!real_fn) {
        void* sym = get_xr_func("xrEnumerateViewConfigurationViews");
        if (sym) {
            real_fn = (XrResult (*)(XrInstance instance, uint64_t systemId, XrViewConfigurationType viewConfigurationType, uint32_t viewCapacityInput, uint32_t* viewCountOutput, void* views))sym;
        } else {
            return 0;
        }
    }
    return real_fn(instance, systemId, viewConfigurationType, viewCapacityInput, viewCountOutput, views);
}

XrResult xrLocateViews(XrSession session, const void* viewLocateInfo, void* viewState, uint32_t viewCapacityInput, uint32_t* viewCountOutput, void* views) {
    static XrResult (*real_fn)(XrSession session, const void* viewLocateInfo, void* viewState, uint32_t viewCapacityInput, uint32_t* viewCountOutput, void* views) = NULL;
    if (!real_fn) {
        void* sym = get_xr_func("xrLocateViews");
        if (sym) {
            real_fn = (XrResult (*)(XrSession session, const void* viewLocateInfo, void* viewState, uint32_t viewCapacityInput, uint32_t* viewCountOutput, void* views))sym;
        } else {
            return 0;
        }
    }
    return real_fn(session, viewLocateInfo, viewState, viewCapacityInput, viewCountOutput, views);
}

XrResult xrApplyHapticFeedback(XrSession session, const void* hapticActionInfo, const void* hapticFeedback) {
    static XrResult (*real_fn)(XrSession session, const void* hapticActionInfo, const void* hapticFeedback) = NULL;
    if (!real_fn) {
        void* sym = get_xr_func("xrApplyHapticFeedback");
        if (sym) {
            real_fn = (XrResult (*)(XrSession session, const void* hapticActionInfo, const void* hapticFeedback))sym;
        } else {
            return 0;
        }
    }
    return real_fn(session, hapticActionInfo, hapticFeedback);
}

XrResult xrGetReferenceSpaceBoundsRect(XrSession session, XrReferenceSpaceType referenceSpaceType, void* bounds) {
    static XrResult (*real_fn)(XrSession session, XrReferenceSpaceType referenceSpaceType, void* bounds) = NULL;
    if (!real_fn) {
        void* sym = get_xr_func("xrGetReferenceSpaceBoundsRect");
        if (sym) {
            real_fn = (XrResult (*)(XrSession session, XrReferenceSpaceType referenceSpaceType, void* bounds))sym;
        } else {
            return 0;
        }
    }
    return real_fn(session, referenceSpaceType, bounds);
}

XrResult xrCreateActionSpace(XrSession session, const void* createInfo, void* space) {
    static XrResult (*real_fn)(XrSession session, const void* createInfo, void* space) = NULL;
    if (!real_fn) {
        void* sym = get_xr_func("xrCreateActionSpace");
        if (sym) {
            real_fn = (XrResult (*)(XrSession session, const void* createInfo, void* space))sym;
        } else {
            return 0;
        }
    }
    return real_fn(session, createInfo, space);
}



// Vulkan extension wrapper
typedef struct VkPhysicalDeviceProperties2 {
    void* sType;
    void* pNext;
    void* properties;
} VkPhysicalDeviceProperties2;

void vkGetPhysicalDeviceProperties2(void* physicalDevice, VkPhysicalDeviceProperties2* pProperties) {
    static void (*real_fn)(void*, VkPhysicalDeviceProperties2*) = NULL;
    if (!real_fn) {
        void* handle = dlopen("libvulkan.so", RTLD_NOW | RTLD_GLOBAL);
        if (handle) {
            real_fn = (void (*)(void*, VkPhysicalDeviceProperties2*))dlsym(handle, "vkGetPhysicalDeviceProperties2");
        }
        if (!real_fn) {
            // Fallback - just return without doing anything
            return;
        }
    }
    real_fn(physicalDevice, pProperties);
}


// ============================================================
// PojavLauncher JNI bridge for MCXRLoader
// References: net.kdt.pojavlaunch.MCXRLoader
// ============================================================

#include <jni.h>
#include <EGL/egl.h>
#include <EGL/eglext.h>

// pojav_environ struct (from environ.h)
typedef struct {
    void* eventCounter;
    void* events;
    void* outEventIndex;
    void* outTargetIndex;
    void* inEventIndex;
    void* inEventCount;
    double cursorX, cursorY, cLastX, cLastY;
    void* method_accessAndroidClipboard;
    void* method_onGrabStateChanged;
    void* method_glftSetWindowAttrib;
    void* method_internalWindowSizeChanged;
    void* bridgeClazz;
    void* vmGlfwClass;
    int isGrabbing;
    void* keyDownBuffer;
    void* mouseDownBuffer;
    void* runtimeJavaVMPtr;
    void* runtimeJNIEnvPtr_JRE;
    void* dalvikJavaVMPtr;
    void* dalvikJNIEnvPtr_ANDROID;
    void* activity;
    XrInstanceCreateInfoAndroidKHR* OpenComposite_Android_Create_Info;
    XrGraphicsBindingOpenGLESAndroidKHR* OpenComposite_Android_GLES_Binding_Info;
    long showingWindow;
    int isInputReady, isCursorEntered, isUseStackQueueCall, shouldUpdateMouse;
    int savedWidth, savedHeight;
    void* GLFW_invoke_Char;
    void* GLFW_invoke_CharMods;
    void* GLFW_invoke_CursorEnter;
    void* GLFW_invoke_CursorPos;
    void* GLFW_invoke_FramebufferSize;
    void* GLFW_invoke_Key;
    void* GLFW_invoke_MouseButton;
    void* GLFW_invoke_Scroll;
    void* GLFW_invoke_WindowSize;
} pojav_environ_s;

// pojav_environ instance
static pojav_environ_s g_pojav_environ = {0};
pojav_environ_s *pojav_environ = &g_pojav_environ;

// Android OpenXR data instances
// These are used by DrvOpenXR.cpp
static XrInstanceCreateInfoAndroidKHR g_android_create_info = {0};
static XrGraphicsBindingOpenGLESAndroidKHR g_android_gles_binding = {0};

// Forward declarations
extern void* JavaVM_CreateJavaVM(void* vm, void* activity);

// Saved JavaVM pointer (set by JNI_OnLoad, used by launch thread)
static JavaVM* g_jvm = NULL;

// Static long long values for display/config/context
static EGLDisplay g_egl_display = EGL_NO_DISPLAY;
static EGLConfig g_egl_config = NULL;
static EGLContext g_egl_context = EGL_NO_CONTEXT;

// JNI function: net.kdt.pojavlaunch.MCXRLoader.launch(android.app.Activity)
// Starts the VR application by calling activity.runCraft() in a new thread
// This matches the original libopenvr_api.so behavior
#include <pthread.h>

struct launch_thread_args {
    JavaVM* jvm;
    jobject activity_ref;
    jmethodID method_id;
    jclass activity_class;
};

static void* launch_thread_func(void* arg) {
    struct launch_thread_args* args = (struct launch_thread_args*)arg;
    JavaVM* jvm = args->jvm;
    JNIEnv* env = NULL;

    if (jvm == NULL) {
        free(args);
        return NULL;
    }

    // Attach this thread to the JVM to get a valid JNIEnv
    jint attach_result = (*jvm)->AttachCurrentThread(jvm, &env, NULL);
    if (attach_result != JNI_OK || env == NULL) {
        // Clean up without using env (which is invalid)
        (*jvm)->DetachCurrentThread(jvm);
        free(args);
        return NULL;
    }

    // Call activity.runCraft()
    (*env)->CallVoidMethod(env, args->activity_ref, args->method_id);

    // Clean up global references
    (*env)->DeleteGlobalRef(env, args->activity_ref);

    // Detach from JVM
    (*jvm)->DetachCurrentThread(jvm);
    free(args);
    return NULL;
}

JNIEXPORT void JNICALL
Java_net_kdt_pojavlaunch_MCXRLoader_launch(
    JNIEnv* env, jclass clazz, jobject activity)
{
    (void)clazz;

    // Create a global reference to the activity
    jobject activityRef = (*env)->NewGlobalRef(env, activity);

    // Get the activity class
    jclass activityClass = (*env)->GetObjectClass(env, activityRef);

    // Find the "runCraft" method with signature "()V"
    jmethodID runCraftMethod = (*env)->GetMethodID(env, activityClass, "runCraft", "()V");

    if (runCraftMethod == NULL) {
        // Clean up if method not found
        (*env)->DeleteGlobalRef(env, activityRef);
        return;
    }

    // Create thread args
    struct launch_thread_args* args = malloc(sizeof(struct launch_thread_args));
    args->jvm = g_jvm;
    args->activity_ref = activityRef;
    args->method_id = runCraftMethod;

    // Create a new thread to call activity.runCraft()
    // The new thread must attach to JVM to get its own valid JNIEnv
    pthread_t thread;
    pthread_create(&thread, NULL, launch_thread_func, args);
    pthread_detach(thread);
}

// JNI function: net.kdt.pojavlaunch.MCXRLoader.setEGLGlobal(long, long, long)
JNIEXPORT void JNICALL
Java_net_kdt_pojavlaunch_MCXRLoader_setEGLGlobal(
    JNIEnv* env, jclass clazz, jlong display, jlong config, jlong context)
{
    (void)clazz;
    (void)env;
    g_egl_display = (EGLDisplay)(intptr_t)display;
    g_egl_config = (EGLConfig)(intptr_t)config;
    g_egl_context = (EGLContext)(intptr_t)context;

    // Set up the OpenComposite_Android_GLES_Binding_Info
    g_android_gles_binding.type = (void*)(intptr_t)1000296001; // XR_TYPE_GRAPHICS_BINDING_OPENGL_ES_ANDROID_KHR
    g_android_gles_binding.next = NULL;
    g_android_gles_binding.display = g_egl_display;
    g_android_gles_binding.config = g_egl_config;
    g_android_gles_binding.context = g_egl_context;

    // Point the OpenComposite pointers to our instances
    g_pojav_environ.OpenComposite_Android_GLES_Binding_Info = &g_android_gles_binding;
    g_pojav_environ.OpenComposite_Android_Create_Info = &g_android_create_info;

    // Also set the extern variables for the old API
    OpenComposite_Android_GLES_Binding_Info = &g_android_gles_binding;
    OpenComposite_Android_Create_Info = &g_android_create_info;
}

// Also provide the mangled version with parameter signature suffix for compatibility
JNIEXPORT void JNICALL
Java_net_kdt_pojavlaunch_MCXRLoader_setEGLGlobal__JJJ(
    JNIEnv* env, jclass clazz, jlong display, jlong config, jlong context)
{
    Java_net_kdt_pojavlaunch_MCXRLoader_setEGLGlobal(env, clazz, display, config, context);
}


// ============================================================
// Additional JNI functions matching original libopenvr_api.so
// ============================================================

// JNI_OnLoad: Called when library is loaded via System.loadLibrary
// Stores the JavaVM pointer for later use
JNIEXPORT jint JNICALL
JNI_OnLoad(JavaVM* vm, void* reserved)
{
    (void)reserved;
    g_jvm = vm;

    // Store JavaVM in the pojav_environ struct
    g_pojav_environ.dalvikJavaVMPtr = vm;
    g_pojav_environ.runtimeJavaVMPtr = vm;

    // Store JavaVM in the android create info
    g_android_create_info.applicationVM = vm;

    return JNI_VERSION_1_4;
}

// JNI function: net.kdt.pojavlaunch.MCXRLoader.setAndroidInitInfo(android.app.Activity)
// Sets up the Android OpenXR initialization info
// Must match original libopenvr_api.so behavior exactly
JNIEXPORT void JNICALL
Java_net_kdt_pojavlaunch_MCXRLoader_setAndroidInitInfo(
    JNIEnv* env, jclass clazz, jobject activity)
{
    (void)clazz;

    // Get JavaVM pointer from env (like original library does)
    JavaVM* jvm_local = NULL;
    (*env)->GetJavaVM(env, &jvm_local);
    if (jvm_local != NULL) {
        g_jvm = jvm_local;
        g_pojav_environ.dalvikJavaVMPtr = jvm_local;
        g_pojav_environ.runtimeJavaVMPtr = jvm_local;
    }

    // Store the activity reference
    jobject activityRef = (*env)->NewGlobalRef(env, activity);
    g_pojav_environ.activity = activityRef;

    // Set up the Android create info for xrCreateInstance
    g_android_create_info.type = (void*)(intptr_t)1000296000; // XR_TYPE_INSTANCE_CREATE_INFO_ANDROID_KHR
    g_android_create_info.next = NULL;
    g_android_create_info.createFlags = NULL;
    g_android_create_info.applicationVM = g_jvm;
    g_android_create_info.applicationActivity = activityRef;

    // Try to initialize the OpenXR loader via xrInitializeLoaderKHR
    // Use the xrGetInstanceProcAddr from the wrapper to find the function
    void* initLoaderFunc = NULL;
    XrResult res = xrGetInstanceProcAddr((XrInstance)0, "xrInitializeLoaderKHR", &initLoaderFunc);

    if (res == 0 && initLoaderFunc != NULL) {
        // Set up loader init info struct on the stack
        // XrLoaderInitInfoAndroidKHR layout:
        //   offset 0: type (uint64_t)
        //   offset 8: next (void*)
        //   offset 16: applicationVM (void*)
        //   offset 24: applicationActivity (void*)
        void* loaderInfo[4];
        loaderInfo[0] = (void*)(intptr_t)1000295000; // XR_TYPE_LOADER_INIT_INFO_ANDROID_KHR
        loaderInfo[1] = NULL;
        loaderInfo[2] = (void*)g_jvm;
        loaderInfo[3] = (void*)activityRef;

        // Call xrInitializeLoaderKHR
        ((XrResult (*)(void*))initLoaderFunc)(loaderInfo);
    }
}
