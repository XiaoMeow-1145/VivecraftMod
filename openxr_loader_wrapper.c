// OpenXR Loader Wrapper for Android (Runtime Dynamic Loading)
// Provides JNI bridge functions for PojavLauncher MCXRLoader
// Loads libopenxr_loader.so at runtime via dlopen/dlsym
// All xr* functions are exported as wrapper functions that call dlsym-resolved pointers

#include <dlfcn.h>
#include <stdint.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <android/log.h>
#include <pthread.h>
#include <jni.h>
#include <EGL/egl.h>
#include <EGL/eglext.h>

#define LOG_TAG "OpenXR-Wrapper"
#define LOGD(...) __android_log_print(ANDROID_LOG_DEBUG, LOG_TAG, __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)

// ============================================================
// OpenXR types (minimal subset)
// ============================================================
typedef uint64_t XrInstance;
typedef uint64_t XrSession;
typedef uint64_t XrSpace;
typedef uint64_t XrAction;
typedef uint64_t XrActionSet;
typedef uint64_t XrSwapchain;
typedef uint64_t XrPath;
typedef uint64_t XrTime;
typedef uint32_t XrResult;
typedef uint32_t XrViewConfigurationType;
typedef uint32_t XrReferenceSpaceType;

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

// ============================================================
// OpenXR loader handle and function pointer table
// ============================================================
static void* g_openxr_handle = NULL;

// Macro to declare a function pointer type and a static pointer
#define XR_FUNC_DECL(ret, name, ...) \
    typedef ret (*name##_fn)(__VA_ARGS__); \
    static name##_fn g_##name = NULL;

// Declare all xr* function pointers
XR_FUNC_DECL(XrResult, xrGetInstanceProcAddr, XrInstance, const char*, void**)
XR_FUNC_DECL(XrResult, xrCreateInstance, const void*, void*)
XR_FUNC_DECL(XrResult, xrDestroyInstance, XrInstance)
XR_FUNC_DECL(XrResult, xrEnumerateApiLayerProperties, uint32_t, uint32_t*, void*)
XR_FUNC_DECL(XrResult, xrEnumerateInstanceExtensionProperties, const char*, uint32_t, uint32_t*, void*)
XR_FUNC_DECL(XrResult, xrGetSystem, XrInstance, const void*, void*)
XR_FUNC_DECL(XrResult, xrGetSystemProperties, XrInstance, uint64_t, void*)
XR_FUNC_DECL(XrResult, xrCreateSession, XrInstance, const void*, void*)
XR_FUNC_DECL(XrResult, xrDestroySession, XrSession)
XR_FUNC_DECL(XrResult, xrBeginSession, XrSession, const void*)
XR_FUNC_DECL(XrResult, xrEndSession, XrSession)
XR_FUNC_DECL(XrResult, xrRequestExitSession, XrSession)
XR_FUNC_DECL(XrResult, xrWaitFrame, XrSession, const void*, void*)
XR_FUNC_DECL(XrResult, xrBeginFrame, XrSession, const void*)
XR_FUNC_DECL(XrResult, xrEndFrame, XrSession, const void*)
XR_FUNC_DECL(XrResult, xrCreateReferenceSpace, XrSession, const void*, void*)
XR_FUNC_DECL(XrResult, xrDestroySpace, XrSpace)
XR_FUNC_DECL(XrResult, xrLocateSpace, XrSpace, XrSpace, XrTime, void*)
XR_FUNC_DECL(XrResult, xrCreateActionSet, XrInstance, const void*, void*)
XR_FUNC_DECL(XrResult, xrDestroyActionSet, XrActionSet)
XR_FUNC_DECL(XrResult, xrCreateAction, XrActionSet, const void*, void*)
XR_FUNC_DECL(XrResult, xrDestroyAction, XrAction)
XR_FUNC_DECL(XrResult, xrSuggestInteractionProfileBindings, XrInstance, const void*)
XR_FUNC_DECL(XrResult, xrAttachSessionActionSets, XrSession, const void*)
XR_FUNC_DECL(XrResult, xrSyncActions, XrSession, const void*)
XR_FUNC_DECL(XrResult, xrEnumerateBoundSourcesForAction, XrSession, const void*, uint32_t, uint32_t*, void*)
XR_FUNC_DECL(XrResult, xrGetCurrentInteractionProfile, XrSession, XrPath, void*)
XR_FUNC_DECL(XrResult, xrGetActionStateBoolean, XrSession, const void*, void*)
XR_FUNC_DECL(XrResult, xrGetActionStateFloat, XrSession, const void*, void*)
XR_FUNC_DECL(XrResult, xrGetActionStateVector2f, XrSession, const void*, void*)
XR_FUNC_DECL(XrResult, xrStringToPath, XrInstance, const char*, void*)
XR_FUNC_DECL(XrResult, xrPathToString, XrInstance, XrPath, uint32_t, uint32_t*, char*)
XR_FUNC_DECL(XrResult, xrPollEvent, XrInstance, void*)
XR_FUNC_DECL(XrResult, xrResultToString, XrInstance, XrResult, void*)
XR_FUNC_DECL(XrResult, xrCreateSwapchain, XrSession, const void*, void*)
XR_FUNC_DECL(XrResult, xrDestroySwapchain, XrSwapchain)
XR_FUNC_DECL(XrResult, xrEnumerateSwapchainFormats, XrSession, uint32_t, uint32_t*, void*)
XR_FUNC_DECL(XrResult, xrEnumerateSwapchainImages, XrSwapchain, uint32_t, uint32_t*, void*)
XR_FUNC_DECL(XrResult, xrAcquireSwapchainImage, XrSwapchain, const void*, void*)
XR_FUNC_DECL(XrResult, xrWaitSwapchainImage, XrSwapchain, const void*)
XR_FUNC_DECL(XrResult, xrReleaseSwapchainImage, XrSwapchain, const void*)
XR_FUNC_DECL(XrResult, xrEnumerateViewConfigurationViews, XrInstance, uint64_t, XrViewConfigurationType, uint32_t, uint32_t*, void*)
XR_FUNC_DECL(XrResult, xrLocateViews, XrSession, const void*, void*, uint32_t, uint32_t*, void*)
XR_FUNC_DECL(XrResult, xrApplyHapticFeedback, XrSession, const void*, const void*)
XR_FUNC_DECL(XrResult, xrGetReferenceSpaceBoundsRect, XrSession, XrReferenceSpaceType, void*)
XR_FUNC_DECL(XrResult, xrCreateActionSpace, XrSession, const void*, void*)
XR_FUNC_DECL(XrResult, xrEnumerateReferenceSpaces, XrSession, uint32_t, uint32_t*, void*)
XR_FUNC_DECL(XrResult, xrGetViewConfigurationProperties, XrInstance, uint64_t, XrViewConfigurationType, void*)
XR_FUNC_DECL(XrResult, xrGetInputSourceLocalizedName, XrSession, const void*, uint32_t, uint32_t*, char*)
XR_FUNC_DECL(XrResult, xrGetActionStatePose, XrSession, const void*, void*)
XR_FUNC_DECL(XrResult, xrStopHapticFeedback, XrSession, const void*)
XR_FUNC_DECL(XrResult, xrEnumerateEnvironmentBlendModes, XrInstance, uint64_t, XrViewConfigurationType, uint32_t, uint32_t*, void*)
XR_FUNC_DECL(XrResult, xrStructureTypeToString, XrInstance, void*, void*)

// ============================================================
// Load all xr* function pointers from libopenxr_loader.so
// ============================================================
static int load_openxr_functions(void) {
    if (g_openxr_handle) return 1; // Already loaded

    LOGD("Loading libopenxr_loader.so...");
    g_openxr_handle = dlopen("libopenxr_loader.so", RTLD_NOW | RTLD_GLOBAL);
    if (!g_openxr_handle) {
        LOGE("Failed to load libopenxr_loader.so: %s", dlerror());
        return 0;
    }
    LOGD("libopenxr_loader.so loaded at %p", g_openxr_handle);

    // Macro to resolve a function by name
    #define RESOLVE(name) do { \
        g_##name = (name##_fn)dlsym(g_openxr_handle, #name); \
        if (!g_##name) { \
            LOGE("Failed to resolve %s: %s", #name, dlerror()); \
            return 0; \
        } \
        LOGD("  Resolved %s -> %p", #name, g_##name); \
    } while(0)

    // Resolve all xr* functions
    RESOLVE(xrGetInstanceProcAddr);
    RESOLVE(xrCreateInstance);
    RESOLVE(xrDestroyInstance);
    RESOLVE(xrEnumerateApiLayerProperties);
    RESOLVE(xrEnumerateInstanceExtensionProperties);
    RESOLVE(xrGetSystem);
    RESOLVE(xrGetSystemProperties);
    RESOLVE(xrCreateSession);
    RESOLVE(xrDestroySession);
    RESOLVE(xrBeginSession);
    RESOLVE(xrEndSession);
    RESOLVE(xrRequestExitSession);
    RESOLVE(xrWaitFrame);
    RESOLVE(xrBeginFrame);
    RESOLVE(xrEndFrame);
    RESOLVE(xrCreateReferenceSpace);
    RESOLVE(xrDestroySpace);
    RESOLVE(xrLocateSpace);
    RESOLVE(xrCreateActionSet);
    RESOLVE(xrDestroyActionSet);
    RESOLVE(xrCreateAction);
    RESOLVE(xrDestroyAction);
    RESOLVE(xrSuggestInteractionProfileBindings);
    RESOLVE(xrAttachSessionActionSets);
    RESOLVE(xrSyncActions);
    RESOLVE(xrEnumerateBoundSourcesForAction);
    RESOLVE(xrGetCurrentInteractionProfile);
    RESOLVE(xrGetActionStateBoolean);
    RESOLVE(xrGetActionStateFloat);
    RESOLVE(xrGetActionStateVector2f);
    RESOLVE(xrStringToPath);
    RESOLVE(xrPathToString);
    RESOLVE(xrPollEvent);
    RESOLVE(xrResultToString);
    RESOLVE(xrCreateSwapchain);
    RESOLVE(xrDestroySwapchain);
    RESOLVE(xrEnumerateSwapchainFormats);
    RESOLVE(xrEnumerateSwapchainImages);
    RESOLVE(xrAcquireSwapchainImage);
    RESOLVE(xrWaitSwapchainImage);
    RESOLVE(xrReleaseSwapchainImage);
    RESOLVE(xrEnumerateViewConfigurationViews);
    RESOLVE(xrLocateViews);
    RESOLVE(xrApplyHapticFeedback);
    RESOLVE(xrGetReferenceSpaceBoundsRect);
    RESOLVE(xrCreateActionSpace);
    RESOLVE(xrEnumerateReferenceSpaces);
    RESOLVE(xrGetViewConfigurationProperties);
    RESOLVE(xrGetInputSourceLocalizedName);
    RESOLVE(xrGetActionStatePose);
    RESOLVE(xrStopHapticFeedback);
    RESOLVE(xrEnumerateEnvironmentBlendModes);
    RESOLVE(xrStructureTypeToString);

    LOGD("All xr* functions resolved successfully");
    return 1;
}

// ============================================================
// Define wrapper functions that export as real symbols
// Macro: pass signature in (params) and args in (call_args)
// ============================================================
#define XR_FUNC_WRAPPER(ret, name, params, call_args) \
    ret name params { return g_##name call_args; }

// Define all xr* wrapper functions
XR_FUNC_WRAPPER(XrResult, xrGetInstanceProcAddr, (XrInstance instance, const char* name, void** functionPtr), (instance, name, functionPtr))
XR_FUNC_WRAPPER(XrResult, xrCreateInstance, (const void* createInfo, void* instance), (createInfo, instance))
XR_FUNC_WRAPPER(XrResult, xrDestroyInstance, (XrInstance instance), (instance))
XR_FUNC_WRAPPER(XrResult, xrEnumerateApiLayerProperties, (uint32_t propertyCapacityInput, uint32_t* propertyCountOutput, void* properties), (propertyCapacityInput, propertyCountOutput, properties))
XR_FUNC_WRAPPER(XrResult, xrEnumerateInstanceExtensionProperties, (const char* layerName, uint32_t propertyCapacityInput, uint32_t* propertyCountOutput, void* properties), (layerName, propertyCapacityInput, propertyCountOutput, properties))
XR_FUNC_WRAPPER(XrResult, xrGetSystem, (XrInstance instance, const void* getInfo, void* systemId), (instance, getInfo, systemId))
XR_FUNC_WRAPPER(XrResult, xrGetSystemProperties, (XrInstance instance, uint64_t systemId, void* properties), (instance, systemId, properties))
XR_FUNC_WRAPPER(XrResult, xrCreateSession, (XrInstance instance, const void* createInfo, void* session), (instance, createInfo, session))
XR_FUNC_WRAPPER(XrResult, xrDestroySession, (XrSession session), (session))
XR_FUNC_WRAPPER(XrResult, xrBeginSession, (XrSession session, const void* beginInfo), (session, beginInfo))
XR_FUNC_WRAPPER(XrResult, xrEndSession, (XrSession session), (session))
XR_FUNC_WRAPPER(XrResult, xrRequestExitSession, (XrSession session), (session))
XR_FUNC_WRAPPER(XrResult, xrWaitFrame, (XrSession session, const void* frameWaitInfo, void* frameState), (session, frameWaitInfo, frameState))
XR_FUNC_WRAPPER(XrResult, xrBeginFrame, (XrSession session, const void* frameBeginInfo), (session, frameBeginInfo))
XR_FUNC_WRAPPER(XrResult, xrEndFrame, (XrSession session, const void* frameEndInfo), (session, frameEndInfo))
XR_FUNC_WRAPPER(XrResult, xrCreateReferenceSpace, (XrSession session, const void* createInfo, void* space), (session, createInfo, space))
XR_FUNC_WRAPPER(XrResult, xrDestroySpace, (XrSpace space), (space))
XR_FUNC_WRAPPER(XrResult, xrLocateSpace, (XrSpace space, XrSpace baseSpace, XrTime time, void* location), (space, baseSpace, time, location))
XR_FUNC_WRAPPER(XrResult, xrCreateActionSet, (XrInstance instance, const void* createInfo, void* actionSet), (instance, createInfo, actionSet))
XR_FUNC_WRAPPER(XrResult, xrDestroyActionSet, (XrActionSet actionSet), (actionSet))
XR_FUNC_WRAPPER(XrResult, xrCreateAction, (XrActionSet actionSet, const void* createInfo, void* action), (actionSet, createInfo, action))
XR_FUNC_WRAPPER(XrResult, xrDestroyAction, (XrAction action), (action))
XR_FUNC_WRAPPER(XrResult, xrSuggestInteractionProfileBindings, (XrInstance instance, const void* suggestedBindings), (instance, suggestedBindings))
XR_FUNC_WRAPPER(XrResult, xrAttachSessionActionSets, (XrSession session, const void* attachInfo), (session, attachInfo))
XR_FUNC_WRAPPER(XrResult, xrSyncActions, (XrSession session, const void* syncInfo), (session, syncInfo))
XR_FUNC_WRAPPER(XrResult, xrEnumerateBoundSourcesForAction, (XrSession session, const void* enumerateInfo, uint32_t sourceCapacityInput, uint32_t* sourceCountOutput, void* sources), (session, enumerateInfo, sourceCapacityInput, sourceCountOutput, sources))
XR_FUNC_WRAPPER(XrResult, xrGetCurrentInteractionProfile, (XrSession session, XrPath topLevelUserPath, void* interactionProfile), (session, topLevelUserPath, interactionProfile))
XR_FUNC_WRAPPER(XrResult, xrGetActionStateBoolean, (XrSession session, const void* getInfo, void* state), (session, getInfo, state))
XR_FUNC_WRAPPER(XrResult, xrGetActionStateFloat, (XrSession session, const void* getInfo, void* state), (session, getInfo, state))
XR_FUNC_WRAPPER(XrResult, xrGetActionStateVector2f, (XrSession session, const void* getInfo, void* state), (session, getInfo, state))
XR_FUNC_WRAPPER(XrResult, xrStringToPath, (XrInstance instance, const char* pathString, void* path), (instance, pathString, path))
XR_FUNC_WRAPPER(XrResult, xrPathToString, (XrInstance instance, XrPath path, uint32_t bufferCapacityInput, uint32_t* bufferCountOutput, char* buffer), (instance, path, bufferCapacityInput, bufferCountOutput, buffer))
XR_FUNC_WRAPPER(XrResult, xrPollEvent, (XrInstance instance, void* eventData), (instance, eventData))
XR_FUNC_WRAPPER(XrResult, xrResultToString, (XrInstance instance, XrResult value, void* buffer), (instance, value, buffer))
XR_FUNC_WRAPPER(XrResult, xrCreateSwapchain, (XrSession session, const void* createInfo, void* swapchain), (session, createInfo, swapchain))
XR_FUNC_WRAPPER(XrResult, xrDestroySwapchain, (XrSwapchain swapchain), (swapchain))
XR_FUNC_WRAPPER(XrResult, xrEnumerateSwapchainFormats, (XrSession session, uint32_t formatCapacityInput, uint32_t* formatCountOutput, void* formats), (session, formatCapacityInput, formatCountOutput, formats))
XR_FUNC_WRAPPER(XrResult, xrEnumerateSwapchainImages, (XrSwapchain swapchain, uint32_t imageCapacityInput, uint32_t* imageCountOutput, void* images), (swapchain, imageCapacityInput, imageCountOutput, images))
XR_FUNC_WRAPPER(XrResult, xrAcquireSwapchainImage, (XrSwapchain swapchain, const void* acquireInfo, void* index), (swapchain, acquireInfo, index))
XR_FUNC_WRAPPER(XrResult, xrWaitSwapchainImage, (XrSwapchain swapchain, const void* waitInfo), (swapchain, waitInfo))
XR_FUNC_WRAPPER(XrResult, xrReleaseSwapchainImage, (XrSwapchain swapchain, const void* releaseInfo), (swapchain, releaseInfo))
XR_FUNC_WRAPPER(XrResult, xrEnumerateViewConfigurationViews, (XrInstance instance, uint64_t systemId, XrViewConfigurationType viewConfigurationType, uint32_t viewCapacityInput, uint32_t* viewCountOutput, void* views), (instance, systemId, viewConfigurationType, viewCapacityInput, viewCountOutput, views))
XR_FUNC_WRAPPER(XrResult, xrLocateViews, (XrSession session, const void* viewLocateInfo, void* viewState, uint32_t viewCapacityInput, uint32_t* viewCountOutput, void* views), (session, viewLocateInfo, viewState, viewCapacityInput, viewCountOutput, views))
XR_FUNC_WRAPPER(XrResult, xrApplyHapticFeedback, (XrSession session, const void* hapticActionInfo, const void* hapticFeedback), (session, hapticActionInfo, hapticFeedback))
XR_FUNC_WRAPPER(XrResult, xrGetReferenceSpaceBoundsRect, (XrSession session, XrReferenceSpaceType referenceSpaceType, void* bounds), (session, referenceSpaceType, bounds))
XR_FUNC_WRAPPER(XrResult, xrCreateActionSpace, (XrSession session, const void* createInfo, void* space), (session, createInfo, space))
XR_FUNC_WRAPPER(XrResult, xrEnumerateReferenceSpaces, (XrSession session, uint32_t spaceCapacityInput, uint32_t* spaceCountOutput, void* spaces), (session, spaceCapacityInput, spaceCountOutput, spaces))
XR_FUNC_WRAPPER(XrResult, xrGetViewConfigurationProperties, (XrInstance instance, uint64_t systemId, XrViewConfigurationType viewConfigurationType, void* configurationProperties), (instance, systemId, viewConfigurationType, configurationProperties))
XR_FUNC_WRAPPER(XrResult, xrGetInputSourceLocalizedName, (XrSession session, const void* getInfo, uint32_t bufferCapacityInput, uint32_t* bufferCountOutput, char* buffer), (session, getInfo, bufferCapacityInput, bufferCountOutput, buffer))
XR_FUNC_WRAPPER(XrResult, xrGetActionStatePose, (XrSession session, const void* getInfo, void* state), (session, getInfo, state))
XR_FUNC_WRAPPER(XrResult, xrStopHapticFeedback, (XrSession session, const void* hapticActionInfo), (session, hapticActionInfo))
XR_FUNC_WRAPPER(XrResult, xrEnumerateEnvironmentBlendModes, (XrInstance instance, uint64_t systemId, XrViewConfigurationType viewConfigurationType, uint32_t blendModeCapacityInput, uint32_t* blendModeCountOutput, void* blendModes), (instance, systemId, viewConfigurationType, blendModeCapacityInput, blendModeCountOutput, blendModes))
XR_FUNC_WRAPPER(XrResult, xrStructureTypeToString, (XrInstance instance, void* structureType, void* buffer), (instance, structureType, buffer))

// ============================================================
// Android extern variables - set by OpenComposite at runtime
// ============================================================
XrInstanceCreateInfoAndroidKHR* OpenComposite_Android_Create_Info = NULL;
XrGraphicsBindingOpenGLESAndroidKHR* OpenComposite_Android_GLES_Binding_Info = NULL;

// ============================================================
// pojav_environ struct (from environ.h)
// ============================================================
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

// ============================================================
// Android OpenXR data instances
// ============================================================
static XrInstanceCreateInfoAndroidKHR g_android_create_info = {0};
static XrGraphicsBindingOpenGLESAndroidKHR g_android_gles_binding = {0};

// Forward declarations
extern void* JavaVM_CreateJavaVM(void* vm, void* activity);

// Saved JavaVM pointer
static JavaVM* g_jvm = NULL;

// Static EGL values for display/config/context
static EGLDisplay g_egl_display = EGL_NO_DISPLAY;
static EGLConfig g_egl_config = NULL;
static EGLContext g_egl_context = EGL_NO_CONTEXT;
static int g_openxr_loader_initialized = 0;

// ============================================================
// xrInitializeLoaderKHR - special handling
// The function has C++ name mangling in the loader, so we find
// it via xrGetInstanceProcAddr (which is an unmangled C symbol)
// ============================================================
static void initialize_openxr_loader(void* jvm, void* activity) {
    if (g_openxr_loader_initialized) return;
    g_openxr_loader_initialized = 1;

    LOGD("Initializing OpenXR loader via xrGetInstanceProcAddr...");

    // Use xrGetInstanceProcAddr with null instance to find xrInitializeLoaderKHR
    void* initLoaderFunc = NULL;
    XrResult res = xrGetInstanceProcAddr((XrInstance)0, "xrInitializeLoaderKHR", &initLoaderFunc);

    if (res == 0 && initLoaderFunc != NULL) {
        LOGD("Found xrInitializeLoaderKHR via xrGetInstanceProcAddr");

        // Set up XrLoaderInitInfoAndroidKHR struct
        // Layout:
        //   offset 0: type (uint64_t) = XR_TYPE_LOADER_INIT_INFO_ANDROID_KHR (1000295000)
        //   offset 8: next (void*) = NULL
        //   offset 16: applicationVM (void*)
        //   offset 24: applicationActivity (void*)
        void* loaderInfo[4];
        loaderInfo[0] = (void*)(intptr_t)1000295000; // XR_TYPE_LOADER_INIT_INFO_ANDROID_KHR
        loaderInfo[1] = NULL;
        loaderInfo[2] = jvm;
        loaderInfo[3] = activity;

        // Call xrInitializeLoaderKHR
        XrResult initRes = ((XrResult (*)(void*))initLoaderFunc)(loaderInfo);
        LOGD("xrInitializeLoaderKHR returned: %d", initRes);
    } else {
        LOGE("xrInitializeLoaderKHR not found via xrGetInstanceProcAddr (res=%d, func=%p)", res, initLoaderFunc);
    }
}

// ============================================================
// JNI functions for net.kdt.pojavlaunch.MCXRLoader
// ============================================================

// JNI_OnLoad: Called when library is loaded via System.loadLibrary
JNIEXPORT jint JNICALL
JNI_OnLoad(JavaVM* vm, void* reserved)
{
    (void)reserved;
    LOGD("JNI_OnLoad called");

    // Load all OpenXR functions first
    if (!load_openxr_functions()) {
        LOGE("Failed to load OpenXR functions");
        // Continue anyway - the app may work without VR
    }

    g_jvm = vm;

    // Store JavaVM in the pojav_environ struct
    g_pojav_environ.dalvikJavaVMPtr = vm;
    g_pojav_environ.runtimeJavaVMPtr = vm;

    // Store JavaVM in the android create info
    g_android_create_info.applicationVM = vm;

    return JNI_VERSION_1_4;
}

// Launch thread function
struct launch_thread_args {
    JavaVM* jvm;
    jobject activity_ref;
    jmethodID method_id;
};

static void* launch_thread_func(void* arg) {
    struct launch_thread_args* args = (struct launch_thread_args*)arg;
    JavaVM* jvm = args->jvm;
    JNIEnv* env = NULL;

    if (jvm == NULL) {
        LOGD("launch_thread_func: jvm is NULL, aborting");
        free(args);
        return NULL;
    }

    // Attach this thread to the JVM to get a valid JNIEnv
    jint attach_result = (*jvm)->AttachCurrentThread(jvm, &env, NULL);
    if (attach_result != JNI_OK || env == NULL) {
        LOGE("launch_thread_func: AttachCurrentThread failed: %d", attach_result);
        free(args);
        return NULL;
    }

    LOGD("launch_thread_func: calling activity.runCraft()");
    // Call activity.runCraft()
    (*env)->CallVoidMethod(env, args->activity_ref, args->method_id);

    // Clean up global references
    (*env)->DeleteGlobalRef(env, args->activity_ref);

    // Detach from JVM
    (*jvm)->DetachCurrentThread(jvm);
    free(args);
    LOGD("launch_thread_func: completed");
    return NULL;
}

// JNI function: net.kdt.pojavlaunch.MCXRLoader.launch(android.app.Activity)
JNIEXPORT void JNICALL
Java_net_kdt_pojavlaunch_MCXRLoader_launch(
    JNIEnv* env, jclass clazz, jobject activity)
{
    (void)clazz;
    LOGD("launch called");

    // Create a global reference to the activity
    jobject activityRef = (*env)->NewGlobalRef(env, activity);

    // Get the activity class
    jclass activityClass = (*env)->GetObjectClass(env, activityRef);

    // Find the "runCraft" method with signature "()V"
    jmethodID runCraftMethod = (*env)->GetMethodID(env, activityClass, "runCraft", "()V");

    if (runCraftMethod == NULL) {
        LOGE("launch: runCraft method not found");
        (*env)->DeleteGlobalRef(env, activityRef);
        return;
    }

    // Create thread args
    struct launch_thread_args* args = malloc(sizeof(struct launch_thread_args));
    if (args == NULL) {
        LOGE("launch: malloc failed");
        (*env)->DeleteGlobalRef(env, activityRef);
        return;
    }
    args->jvm = g_jvm;
    args->activity_ref = activityRef;
    args->method_id = runCraftMethod;

    // Create a new thread to call activity.runCraft()
    pthread_t thread;
    int ret = pthread_create(&thread, NULL, launch_thread_func, args);
    if (ret != 0) {
        LOGE("launch: pthread_create failed: %d", ret);
        (*env)->DeleteGlobalRef(env, activityRef);
        free(args);
        return;
    }
    pthread_detach(thread);
    LOGD("launch: thread created successfully");
}

// JNI function: net.kdt.pojavlaunch.MCXRLoader.setEGLGlobal(long, long, long)
JNIEXPORT void JNICALL
Java_net_kdt_pojavlaunch_MCXRLoader_setEGLGlobal(
    JNIEnv* env, jclass clazz, jlong display, jlong config, jlong context)
{
    (void)clazz;
    (void)env;
    LOGD("setEGLGlobal called: display=%p, config=%p, context=%p",
         (void*)(intptr_t)display, (void*)(intptr_t)config, (void*)(intptr_t)context);

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

// JNI function: net.kdt.pojavlaunch.MCXRLoader.setAndroidInitInfo(android.app.Activity)
JNIEXPORT void JNICALL
Java_net_kdt_pojavlaunch_MCXRLoader_setAndroidInitInfo(
    JNIEnv* env, jclass clazz, jobject activity)
{
    (void)clazz;
    LOGD("setAndroidInitInfo called");

    // Get JavaVM pointer from env
    JavaVM* jvm_local = NULL;
    (*env)->GetJavaVM(env, &jvm_local);
    if (jvm_local != NULL) {
        g_jvm = jvm_local;
        g_pojav_environ.dalvikJavaVMPtr = jvm_local;
        g_pojav_environ.runtimeJavaVMPtr = jvm_local;
        LOGD("setAndroidInitInfo: JavaVM=%p", jvm_local);
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

    // Initialize the OpenXR loader
    initialize_openxr_loader((void*)g_jvm, (void*)activityRef);
}

// ============================================================
// Vulkan extension wrapper
// ============================================================
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