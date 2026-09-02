// OpenXR JNI Bridge for PojavLauncher MCXRLoader
// Matches original libopenvr_api.so layout:
// - Uses dlopen+dlsym to load the SYSTEM's libopenxr_loader.so instead of the APK's
// - Exports 3 OpenComposite_Android_* pointers (8 bytes each)
// - Minimal JNI_OnLoad, init in setAndroidInitInfo/setEGLGlobal
// - Also links libGLESv3.so, libvulkan.so, libm.so (same NEEDED as original)

#include <stdint.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <android/log.h>
#include <pthread.h>
#include <jni.h>
#include <EGL/egl.h>
#include <EGL/eglext.h>
#include <dlfcn.h>

#define LOG_TAG "OpenXR-Wrapper"
#define LOGD(...) __android_log_print(ANDROID_LOG_DEBUG, LOG_TAG, __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)
#define LOGW(...) __android_log_print(ANDROID_LOG_WARN, LOG_TAG, __VA_ARGS__)

// ============================================================
// OpenXR types (minimal for structs)
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
typedef int32_t  XrBool32;
typedef void (*PFN_xrVoidFunction)(void);

typedef struct XrLoaderInitInfoBaseHeaderKHR {
    int32_t type;
    const void* next;
} XrLoaderInitInfoBaseHeaderKHR;

typedef struct XrLoaderInitInfoAndroidKHR {
    XrLoaderInitInfoBaseHeaderKHR base;
    void* applicationVM;
    void* applicationActivity;
} XrLoaderInitInfoAndroidKHR;

typedef struct XrInstanceCreateInfoAndroidKHR {
    int32_t type;
    const void* next;
    void* applicationVM;
    void* applicationActivity;
} XrInstanceCreateInfoAndroidKHR;

typedef struct XrGraphicsBindingOpenGLESAndroidKHR {
    int32_t type;
    const void* next;
    void* display;
    void* config;
    void* context;
} XrGraphicsBindingOpenGLESAndroidKHR;

// ============================================================
// OpenXR function pointer table - loaded via dlsym from system libopenxr_loader.so
// ============================================================
typedef XrResult (*PFN_xrGetInstanceProcAddr_t)(XrInstance, const char*, PFN_xrVoidFunction*);
typedef XrResult (*PFN_xrCreateInstance_t)(const void*, XrInstance*);
typedef XrResult (*PFN_xrDestroyInstance_t)(XrInstance);
typedef XrResult (*PFN_xrEnumerateApiLayerProperties_t)(uint32_t, uint32_t*, void*);
typedef XrResult (*PFN_xrEnumerateInstanceExtensionProperties_t)(const char*, uint32_t, uint32_t*, void*);
typedef XrResult (*PFN_xrGetSystem_t)(XrInstance, const void*, uint64_t*);
typedef XrResult (*PFN_xrGetSystemProperties_t)(XrInstance, uint64_t, void*);
typedef XrResult (*PFN_xrCreateSession_t)(XrInstance, const void*, XrSession*);
typedef XrResult (*PFN_xrDestroySession_t)(XrSession);
typedef XrResult (*PFN_xrBeginSession_t)(XrSession, const void*);
typedef XrResult (*PFN_xrEndSession_t)(XrSession);
typedef XrResult (*PFN_xrRequestExitSession_t)(XrSession);
typedef XrResult (*PFN_xrWaitFrame_t)(XrSession, const void*, void*);
typedef XrResult (*PFN_xrBeginFrame_t)(XrSession, const void*);
typedef XrResult (*PFN_xrEndFrame_t)(XrSession, const void*);
typedef XrResult (*PFN_xrCreateReferenceSpace_t)(XrSession, const void*, XrSpace*);
typedef XrResult (*PFN_xrDestroySpace_t)(XrSpace);
typedef XrResult (*PFN_xrLocateSpace_t)(XrSpace, XrSpace, XrTime, void*);
typedef XrResult (*PFN_xrCreateActionSet_t)(XrInstance, const void*, XrActionSet*);
typedef XrResult (*PFN_xrDestroyActionSet_t)(XrActionSet);
typedef XrResult (*PFN_xrCreateAction_t)(XrActionSet, const void*, XrAction*);
typedef XrResult (*PFN_xrDestroyAction_t)(XrAction);
typedef XrResult (*PFN_xrSuggestInteractionProfileBindings_t)(XrInstance, const void*);
typedef XrResult (*PFN_xrAttachSessionActionSets_t)(XrSession, const void*);
typedef XrResult (*PFN_xrSyncActions_t)(XrSession, const void*);
typedef XrResult (*PFN_xrEnumerateBoundSourcesForAction_t)(XrSession, const void*, uint32_t, uint32_t*, void*);
typedef XrResult (*PFN_xrGetCurrentInteractionProfile_t)(XrSession, XrPath, void*);
typedef XrResult (*PFN_xrGetActionStateBoolean_t)(XrSession, const void*, void*);
typedef XrResult (*PFN_xrGetActionStateFloat_t)(XrSession, const void*, void*);
typedef XrResult (*PFN_xrGetActionStateVector2f_t)(XrSession, const void*, void*);
typedef XrResult (*PFN_xrStringToPath_t)(XrInstance, const char*, XrPath*);
typedef XrResult (*PFN_xrPathToString_t)(XrInstance, XrPath, uint32_t, uint32_t*, char*);
typedef XrResult (*PFN_xrPollEvent_t)(XrInstance, void*);
typedef XrResult (*PFN_xrResultToString_t)(XrInstance, XrResult, void*);
typedef XrResult (*PFN_xrCreateSwapchain_t)(XrSession, const void*, XrSwapchain*);
typedef XrResult (*PFN_xrDestroySwapchain_t)(XrSwapchain);
typedef XrResult (*PFN_xrEnumerateSwapchainFormats_t)(XrSession, uint32_t, uint32_t*, int64_t*);
typedef XrResult (*PFN_xrEnumerateSwapchainImages_t)(XrSwapchain, uint32_t, uint32_t*, void*);
typedef XrResult (*PFN_xrAcquireSwapchainImage_t)(XrSwapchain, const void*, uint32_t*);
typedef XrResult (*PFN_xrWaitSwapchainImage_t)(XrSwapchain, const void*);
typedef XrResult (*PFN_xrReleaseSwapchainImage_t)(XrSwapchain, const void*);
typedef XrResult (*PFN_xrEnumerateViewConfigurationViews_t)(XrInstance, uint64_t, uint32_t, uint32_t, uint32_t*, void*);
typedef XrResult (*PFN_xrLocateViews_t)(XrSession, const void*, void*, uint32_t, uint32_t*, void*);
typedef XrResult (*PFN_xrApplyHapticFeedback_t)(XrSession, const void*, const void*);
typedef XrResult (*PFN_xrGetReferenceSpaceBoundsRect_t)(XrSession, uint32_t, void*);
typedef XrResult (*PFN_xrCreateActionSpace_t)(XrSession, const void*, XrSpace*);
typedef XrResult (*PFN_xrEnumerateReferenceSpaces_t)(XrSession, uint32_t, uint32_t*, uint32_t*);
typedef XrResult (*PFN_xrGetViewConfigurationProperties_t)(XrInstance, uint64_t, uint32_t, void*);
typedef XrResult (*PFN_xrGetInputSourceLocalizedName_t)(XrSession, const void*, uint32_t, uint32_t*, char*);
typedef XrResult (*PFN_xrGetActionStatePose_t)(XrSession, const void*, void*);
typedef XrResult (*PFN_xrStopHapticFeedback_t)(XrSession, const void*);
typedef XrResult (*PFN_xrEnumerateEnvironmentBlendModes_t)(XrInstance, uint64_t, uint32_t, uint32_t, uint32_t*, uint32_t*);
typedef XrResult (*PFN_xrStructureTypeToString_t)(XrInstance, int32_t, void*);

// dispatch table
typedef struct {
    PFN_xrGetInstanceProcAddr_t GetInstanceProcAddr;
    PFN_xrCreateInstance_t CreateInstance;
    PFN_xrDestroyInstance_t DestroyInstance;
    PFN_xrEnumerateApiLayerProperties_t EnumerateApiLayerProperties;
    PFN_xrEnumerateInstanceExtensionProperties_t EnumerateInstanceExtensionProperties;
    PFN_xrGetSystem_t GetSystem;
    PFN_xrGetSystemProperties_t GetSystemProperties;
    PFN_xrCreateSession_t CreateSession;
    PFN_xrDestroySession_t DestroySession;
    PFN_xrBeginSession_t BeginSession;
    PFN_xrEndSession_t EndSession;
    PFN_xrRequestExitSession_t RequestExitSession;
    PFN_xrWaitFrame_t WaitFrame;
    PFN_xrBeginFrame_t BeginFrame;
    PFN_xrEndFrame_t EndFrame;
    PFN_xrCreateReferenceSpace_t CreateReferenceSpace;
    PFN_xrDestroySpace_t DestroySpace;
    PFN_xrLocateSpace_t LocateSpace;
    PFN_xrCreateActionSet_t CreateActionSet;
    PFN_xrDestroyActionSet_t DestroyActionSet;
    PFN_xrCreateAction_t CreateAction;
    PFN_xrDestroyAction_t DestroyAction;
    PFN_xrSuggestInteractionProfileBindings_t SuggestInteractionProfileBindings;
    PFN_xrAttachSessionActionSets_t AttachSessionActionSets;
    PFN_xrSyncActions_t SyncActions;
    PFN_xrEnumerateBoundSourcesForAction_t EnumerateBoundSourcesForAction;
    PFN_xrGetCurrentInteractionProfile_t GetCurrentInteractionProfile;
    PFN_xrGetActionStateBoolean_t GetActionStateBoolean;
    PFN_xrGetActionStateFloat_t GetActionStateFloat;
    PFN_xrGetActionStateVector2f_t GetActionStateVector2f;
    PFN_xrStringToPath_t StringToPath;
    PFN_xrPathToString_t PathToString;
    PFN_xrPollEvent_t PollEvent;
    PFN_xrResultToString_t ResultToString;
    PFN_xrCreateSwapchain_t CreateSwapchain;
    PFN_xrDestroySwapchain_t DestroySwapchain;
    PFN_xrEnumerateSwapchainFormats_t EnumerateSwapchainFormats;
    PFN_xrEnumerateSwapchainImages_t EnumerateSwapchainImages;
    PFN_xrAcquireSwapchainImage_t AcquireSwapchainImage;
    PFN_xrWaitSwapchainImage_t WaitSwapchainImage;
    PFN_xrReleaseSwapchainImage_t ReleaseSwapchainImage;
    PFN_xrEnumerateViewConfigurationViews_t EnumerateViewConfigurationViews;
    PFN_xrLocateViews_t LocateViews;
    PFN_xrApplyHapticFeedback_t ApplyHapticFeedback;
    PFN_xrGetReferenceSpaceBoundsRect_t GetReferenceSpaceBoundsRect;
    PFN_xrCreateActionSpace_t CreateActionSpace;
    PFN_xrEnumerateReferenceSpaces_t EnumerateReferenceSpaces;
    PFN_xrGetViewConfigurationProperties_t GetViewConfigurationProperties;
    PFN_xrGetInputSourceLocalizedName_t GetInputSourceLocalizedName;
    PFN_xrGetActionStatePose_t GetActionStatePose;
    PFN_xrStopHapticFeedback_t StopHapticFeedback;
    PFN_xrEnumerateEnvironmentBlendModes_t EnumerateEnvironmentBlendModes;
    PFN_xrStructureTypeToString_t StructureTypeToString;
} xr_dispatch;

static xr_dispatch xr = {NULL};

// ============================================================
// Load OpenXR dispatch table from system's libopenxr_loader.so
// ============================================================
// Store the dlopen handle for fallback dlsym
static void* g_openxr_handle = NULL;
#define XR_LOAD_SYM(handle, name) do { \
    xr.name = (PFN_xr##name##_t)dlsym(handle, "xr" #name); \
    if (!xr.name) { \
        LOGE("Failed to load xr" #name ": %s", dlerror()); \
        return -1; \
    } \
} while(0)

static int s_load_xr_dispatch(void) {
    void* handle = NULL;
    const char* paths[] = {
        "/system/lib64/libopenxr_loader.so",  // Quest 3S system loader
        "/vendor/lib64/libopenxr_loader.so",  // alternative vendor path
        "libopenxr_loader.so",                // fallback to default
        NULL
    };

    for (int i = 0; paths[i]; i++) {
        handle = dlopen(paths[i], RTLD_LAZY | RTLD_LOCAL);
        if (handle) {
            LOGI("Loaded OpenXR loader from: %s", paths[i]);
            break;
        }
        LOGD("dlopen(%s) failed: %s", paths[i], dlerror());
    }

    if (!handle) {
        LOGE("Could not load any libopenxr_loader.so");
        return -1;
    }

    g_openxr_handle = handle;

    // Load all OpenXR function pointers
    XR_LOAD_SYM(handle, GetInstanceProcAddr);
    XR_LOAD_SYM(handle, CreateInstance);
    XR_LOAD_SYM(handle, DestroyInstance);
    XR_LOAD_SYM(handle, EnumerateApiLayerProperties);
    XR_LOAD_SYM(handle, EnumerateInstanceExtensionProperties);
    XR_LOAD_SYM(handle, GetSystem);
    XR_LOAD_SYM(handle, GetSystemProperties);
    XR_LOAD_SYM(handle, CreateSession);
    XR_LOAD_SYM(handle, DestroySession);
    XR_LOAD_SYM(handle, BeginSession);
    XR_LOAD_SYM(handle, EndSession);
    XR_LOAD_SYM(handle, RequestExitSession);
    XR_LOAD_SYM(handle, WaitFrame);
    XR_LOAD_SYM(handle, BeginFrame);
    XR_LOAD_SYM(handle, EndFrame);
    XR_LOAD_SYM(handle, CreateReferenceSpace);
    XR_LOAD_SYM(handle, DestroySpace);
    XR_LOAD_SYM(handle, LocateSpace);
    XR_LOAD_SYM(handle, CreateActionSet);
    XR_LOAD_SYM(handle, DestroyActionSet);
    XR_LOAD_SYM(handle, CreateAction);
    XR_LOAD_SYM(handle, DestroyAction);
    XR_LOAD_SYM(handle, SuggestInteractionProfileBindings);
    XR_LOAD_SYM(handle, AttachSessionActionSets);
    XR_LOAD_SYM(handle, SyncActions);
    XR_LOAD_SYM(handle, EnumerateBoundSourcesForAction);
    XR_LOAD_SYM(handle, GetCurrentInteractionProfile);
    XR_LOAD_SYM(handle, GetActionStateBoolean);
    XR_LOAD_SYM(handle, GetActionStateFloat);
    XR_LOAD_SYM(handle, GetActionStateVector2f);
    XR_LOAD_SYM(handle, StringToPath);
    XR_LOAD_SYM(handle, PathToString);
    XR_LOAD_SYM(handle, PollEvent);
    XR_LOAD_SYM(handle, ResultToString);
    XR_LOAD_SYM(handle, CreateSwapchain);
    XR_LOAD_SYM(handle, DestroySwapchain);
    XR_LOAD_SYM(handle, EnumerateSwapchainFormats);
    XR_LOAD_SYM(handle, EnumerateSwapchainImages);
    XR_LOAD_SYM(handle, AcquireSwapchainImage);
    XR_LOAD_SYM(handle, WaitSwapchainImage);
    XR_LOAD_SYM(handle, ReleaseSwapchainImage);
    XR_LOAD_SYM(handle, EnumerateViewConfigurationViews);
    XR_LOAD_SYM(handle, LocateViews);
    XR_LOAD_SYM(handle, ApplyHapticFeedback);
    XR_LOAD_SYM(handle, GetReferenceSpaceBoundsRect);
    XR_LOAD_SYM(handle, CreateActionSpace);
    XR_LOAD_SYM(handle, EnumerateReferenceSpaces);
    XR_LOAD_SYM(handle, GetViewConfigurationProperties);
    XR_LOAD_SYM(handle, GetInputSourceLocalizedName);
    XR_LOAD_SYM(handle, GetActionStatePose);
    XR_LOAD_SYM(handle, StopHapticFeedback);
    XR_LOAD_SYM(handle, EnumerateEnvironmentBlendModes);
    XR_LOAD_SYM(handle, StructureTypeToString);

    LOGI("OpenXR dispatch table loaded: %d functions", 53);
    return 0;
}

// ============================================================
// Static storage - backing structs for OpenComposite pointers
// ============================================================
static JavaVM* s_jvm = NULL;

static XrInstanceCreateInfoAndroidKHR s_android_create_info = {
    1000296000,   // XR_TYPE_INSTANCE_CREATE_INFO_ANDROID_KHR
    NULL,         // next
    NULL,         // applicationVM
    NULL,         // applicationActivity
};

static XrGraphicsBindingOpenGLESAndroidKHR s_android_gles_binding = {
    1000296001,   // XR_TYPE_GRAPHICS_BINDING_OPENGL_ES_ANDROID_KHR
    NULL,         // next
    NULL, NULL, NULL,
};

// ============================================================
// EXPORTED 8-byte pointers (OBJECT) - exactly like the original
// OpenComposite reads these directly when doing xrCreateInstance / xrCreateSession
// ============================================================
typedef size_t (*AndroidLoadInputFileFn)(const char* path, char* outBuf, size_t bufSize);

void* OpenComposite_Android_Create_Info = &s_android_create_info;
void* OpenComposite_Android_GLES_Binding_Info = &s_android_gles_binding;
void* OpenComposite_Android_Load_Input_File = NULL;

// ============================================================
// Initialize xrInitializeLoaderKHR - using the system's OpenXR loader
// ============================================================
static void s_call_xrInitializeLoaderKHR(void* jvm, void* activity) {
    LOGD("Calling xrInitializeLoaderKHR...");

    PFN_xrVoidFunction initFunc = NULL;

    // Strategy 1: Try dlsym from the loaded library handle directly (most reliable)
    if (g_openxr_handle) {
        initFunc = (PFN_xrVoidFunction)dlsym(g_openxr_handle, "xrInitializeLoaderKHR");
        if (initFunc) {
            LOGD("Found init via dlsym(libhandle)");
        }
    }

    // Strategy 2: Try dlsym(RTLD_DEFAULT, "xrInitializeLoaderKHR") - plain C name
    if (!initFunc) {
        initFunc = (PFN_xrVoidFunction)dlsym(RTLD_DEFAULT, "xrInitializeLoaderKHR");
        if (initFunc) {
            LOGD("Found init via dlsym(RTLD_DEFAULT)");
        }
    }

    // Strategy 3: Try mangled C++ name via dlsym
    if (!initFunc) {
        initFunc = (PFN_xrVoidFunction)dlsym(RTLD_DEFAULT, "_Z21xrInitializeLoaderKHRPK29XrLoaderInitInfoBaseHeaderKHR");
        if (initFunc) {
            LOGD("Found init via dlsym mangled C++ name");
        }
    }

    // Strategy 4: Try via xrGetInstanceProcAddr from the dispatch table
    if (!initFunc) {
        XrResult res = xr.GetInstanceProcAddr((XrInstance)0, "xrInitializeLoaderKHR", &initFunc);
        if (res == 0 && initFunc) {
            LOGD("Found init via xrGetInstanceProcAddr");
        } else {
            LOGE("xrInitializeLoaderKHR not found via any method");
        }
    }

    if (initFunc) {
        // Prepare the init info struct
        XrLoaderInitInfoAndroidKHR info;
        memset(&info, 0, sizeof(info));
        info.base.type = 1000295000;  // XR_TYPE_LOADER_INIT_INFO_ANDROID_KHR
        info.base.next = NULL;
        info.applicationVM = jvm;
        info.applicationActivity = activity;

        // Try calling with the struct cast to base header
        typedef XrResult (*FnTy)(const XrLoaderInitInfoBaseHeaderKHR*);
        XrResult r = ((FnTy)initFunc)(&info.base);
        LOGI("xrInitializeLoaderKHR result: %d", r);

        if (r == 0) {
            LOGI("OpenXR loader initialized successfully!");
        } else {
            LOGW("xrInitializeLoaderKHR returned %d - continuing anyway", r);
        }
    }

    // TEST: Try to create an OpenXR instance to see if the runtime is accessible
    LOGD("--- TEST: xrCreateInstance with Android create info ---");
    
    typedef struct XrApplicationInfo {
        char applicationName[128];
        uint32_t applicationVersion;
        char engineName[128];
        uint32_t engineVersion;
        uint32_t apiVersion;
    } XrApplicationInfo;
    
    XrApplicationInfo appInfo;
    memset(&appInfo, 0, sizeof(appInfo));
    strncpy(appInfo.applicationName, "TestVR", sizeof(appInfo.applicationName) - 1);
    appInfo.applicationVersion = 1;
    strncpy(appInfo.engineName, "Test", sizeof(appInfo.engineName) - 1);
    appInfo.engineVersion = 1;
    appInfo.apiVersion = 4194304;  // XR_MAKE_API_VERSION(1, 0, 0) = 0x400000
    
    // Full XrInstanceCreateInfo struct matching OpenXR spec
    typedef struct XrInstanceCreateInfo {
        int32_t type;
        const void* next;
        uint32_t createFlags;
        XrApplicationInfo applicationInfo;
        uint32_t enabledApiLayerCount;
        const void* enabledApiLayerNames;
        uint32_t enabledExtensionCount;
        const void* enabledExtensionNames;
    } XrInstanceCreateInfo;
    
    XrInstanceCreateInfo createInfo;
    memset(&createInfo, 0, sizeof(createInfo));
    createInfo.type = 1;  // XR_TYPE_INSTANCE_CREATE_INFO
    createInfo.next = &s_android_create_info;
    createInfo.createFlags = 0;
    createInfo.applicationInfo = appInfo;
    
    XrInstance instance = 0;
    XrResult createRes = xr.CreateInstance(&createInfo, &instance);
    LOGI("--- TEST: xrCreateInstance result: %d, instance=%p ---", createRes, (void*)instance);
    
    if (createRes == 0) {
        LOGI("--- TEST: OpenXR instance created SUCCESSFULLY! ---");
        
        // Enumerate system
        uint64_t systemId = 0;
        typedef struct XrSystemGetInfo {
            int32_t type;
            const void* next;
            uint32_t formFactor;
        } XrSystemGetInfo;
        
        XrSystemGetInfo sysInfo;
        memset(&sysInfo, 0, sizeof(sysInfo));
        sysInfo.type = 5;  // XR_TYPE_SYSTEM_GET_INFO
        sysInfo.next = NULL;
        sysInfo.formFactor = 1;  // XR_FORM_FACTOR_HEAD_MOUNTED_DISPLAY
        
        XrResult sysRes = xr.GetSystem(instance, &sysInfo, &systemId);
        LOGI("--- TEST: xrGetSystem result: %d, systemId=%lu ---", sysRes, systemId);
        
        xr.DestroyInstance(instance);
    } else {
        LOGE("--- TEST: xrCreateInstance FAILED - OpenXR runtime not accessible! ---");
    }
}

// ============================================================
// JNI_OnLoad - load OpenXR dispatch table and init
// ============================================================
JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM* vm, void* reserved) {
    (void)reserved;
    s_jvm = vm;
    LOGD("JNI_OnLoad: JavaVM=%p", (void*)vm);

    // Load the system OpenXR loader dispatch table
    if (s_load_xr_dispatch() != 0) {
        LOGE("Failed to load OpenXR dispatch table - VR will not work!");
    } else {
        LOGI("OpenXR dispatch table loaded successfully");
    }

    return JNI_VERSION_1_4;
}

// ============================================================
// Launch thread - call runCraft() in a detached thread
// ============================================================
typedef struct {
    JavaVM* jvm;
    jobject activity;
    jmethodID runCraft;
} launch_args;

static void* launch_thread(void* arg) {
    launch_args* a = (launch_args*)arg;
    JNIEnv* env = NULL;
    if ((*a->jvm)->AttachCurrentThread(a->jvm, &env, NULL) == JNI_OK && env) {
        LOGD("launch_thread: calling runCraft");
        (*env)->CallVoidMethod(env, a->activity, a->runCraft);
        (*env)->DeleteGlobalRef(env, a->activity);
        (*a->jvm)->DetachCurrentThread(a->jvm);
    } else {
        LOGE("launch_thread: AttachCurrentThread failed");
    }
    free(a);
    return NULL;
}

// ============================================================
// JNI: MCXRLoader.launch(Activity)
// ============================================================
JNIEXPORT void JNICALL Java_net_kdt_pojavlaunch_MCXRLoader_launch(
    JNIEnv* env, jclass cls, jobject activity)
{
    (void)cls;
    LOGD("launch called");

    jclass actCls = (*env)->GetObjectClass(env, activity);
    jmethodID runCraft = (*env)->GetMethodID(env, actCls, "runCraft", "()V");
    if (!runCraft) { LOGE("launch: runCraft not found"); return; }

    launch_args* a = (launch_args*)malloc(sizeof(*a));
    if (!a) return;
    a->jvm = s_jvm;
    a->activity = (*env)->NewGlobalRef(env, activity);
    a->runCraft = runCraft;

    pthread_t t;
    if (pthread_create(&t, NULL, launch_thread, a) == 0) {
        pthread_detach(t);
        LOGD("launch: thread started");
    } else {
        LOGE("launch: pthread_create failed");
        (*env)->DeleteGlobalRef(env, a->activity);
        free(a);
    }
}

// ============================================================
// JNI: MCXRLoader.setEGLGlobal(display, config, context)
// Update exported GLES binding struct for OpenComposite
// ============================================================
JNIEXPORT void JNICALL Java_net_kdt_pojavlaunch_MCXRLoader_setEGLGlobal(
    JNIEnv* env, jclass cls, jlong display, jlong config, jlong context)
{
    (void)env; (void)cls;
    LOGD("setEGLGlobal: d=%p c=%p ctx=%p",
         (void*)(intptr_t)display, (void*)(intptr_t)config, (void*)(intptr_t)context);

    s_android_gles_binding.display = (void*)(intptr_t)display;
    s_android_gles_binding.config  = (void*)(intptr_t)config;
    s_android_gles_binding.context = (void*)(intptr_t)context;
}

// Compatibility version with parameter signature suffix
JNIEXPORT void JNICALL Java_net_kdt_pojavlaunch_MCXRLoader_setEGLGlobal__JJJ(
    JNIEnv* env, jclass cls, jlong display, jlong config, jlong context)
{
    Java_net_kdt_pojavlaunch_MCXRLoader_setEGLGlobal(env, cls, display, config, context);
}

// ============================================================
// JNI: MCXRLoader.setAndroidInitInfo(Activity)
// Update exported Create_Info and call xrInitializeLoaderKHR
// ============================================================
JNIEXPORT void JNICALL Java_net_kdt_pojavlaunch_MCXRLoader_setAndroidInitInfo(
    JNIEnv* env, jclass cls, jobject activity)
{
    (void)cls;
    LOGD("setAndroidInitInfo called");

    JavaVM* vm = NULL;
    (*env)->GetJavaVM(env, &vm);
    if (vm) {
        s_jvm = vm;
        LOGD("setAndroidInitInfo: JavaVM=%p", (void*)vm);
    }

    jobject actRef = (*env)->NewGlobalRef(env, activity);

    // Update exported create info backing struct
    s_android_create_info.type              = 1000296000;  // XR_TYPE_INSTANCE_CREATE_INFO_ANDROID_KHR
    s_android_create_info.next              = NULL;
    s_android_create_info.applicationVM     = (void*)s_jvm;
    s_android_create_info.applicationActivity = (void*)actRef;

    LOGD("Create_Info{type=%d vm=%p act=%p}",
         s_android_create_info.type,
         s_android_create_info.applicationVM,
         s_android_create_info.applicationActivity);

    // Now try to initialize the OpenXR loader
    s_call_xrInitializeLoaderKHR((void*)s_jvm, (void*)actRef);
}