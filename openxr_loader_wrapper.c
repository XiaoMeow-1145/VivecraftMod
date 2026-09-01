// OpenXR Loader Wrapper for Android
// Provides all xr* symbols by loading libopenxr_loader.so at runtime via dlopen/dlsym
// This allows the .so to be loaded by the Android linker without unresolved symbols.

#include <dlfcn.h>
#include <stdint.h>
#include <stddef.h>

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
