// OpenXR JNI Bridge for PojavLauncher MCXRLoader
// Matches original libopenvr_api.so layout:
// - Directly links libopenxr_loader.so (all xr* are UND imports, GLOBAL visible)
// - Exports 3 OpenComposite_Android_* pointers (8 bytes each)
// - Minimal JNI_OnLoad, init in setAndroidInitInfo/setEGLGlobal
// - Also links libGLESv3.so, libvulkan.so, libm.so (same NEEDED as original)

#include <stdint.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <android/log.h>
#include <pthread.h>
#include <jni.h>
#include <EGL/egl.h>
#include <EGL/eglext.h>
#include <dlfcn.h>

#define LOG_TAG "OpenXR-Wrapper"
#define LOGD(...) __android_log_print(ANDROID_LOG_DEBUG, LOG_TAG, __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)

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

typedef struct XrLoaderInitInfoBaseHeaderKHR {
    int32_t type;
    const void* next;
} XrLoaderInitInfoBaseHeaderKHR;

typedef struct XrLoaderInitInfoAndroidKHR {
    XrLoaderInitInfoBaseHeaderKHR base;
    void* applicationVM;
    void* applicationActivity;
} XrLoaderInitInfoAndroidKHR;

// These types must match XrInstanceCreateInfoAndroidKHR / XrGraphicsBinding...
// which are also used by OpenComposite via pointer chain
typedef struct XrInstanceCreateInfoAndroidKHR {
    int32_t type;
    const void* next;
    uint64_t createFlags;  // Actually XrInstanceCreateFlags (uint64_t)
    void* applicationVM;
    void* applicationActivity;
} XrInstanceCreateInfoAndroidKHR;

typedef struct XrGraphicsBindingOpenGLESAndroidKHR {
    int32_t type;
    const void* next;
    void* display;   // EGLDisplay
    void* config;    // EGLConfig
    void* context;   // EGLContext
} XrGraphicsBindingOpenGLESAndroidKHR;

// ============================================================
// OpenXR function declarations - imported from libopenxr_loader.so
// Must match OpenXR C API exactly - these will be GLOBAL UND
// ============================================================
#define XR_API
typedef XrResult (XR_API *PFN_xrVoidFunction)(void);

extern XrResult xrGetInstanceProcAddr(XrInstance instance, const char* name, PFN_xrVoidFunction* function);
extern XrResult xrCreateInstance(const void* createInfo, XrInstance* instance);
extern XrResult xrDestroyInstance(XrInstance instance);
extern XrResult xrEnumerateApiLayerProperties(uint32_t propertyCapacityInput, uint32_t* propertyCountOutput, void* properties);
extern XrResult xrEnumerateInstanceExtensionProperties(const char* layerName, uint32_t propertyCapacityInput, uint32_t* propertyCountOutput, void* properties);
extern XrResult xrGetSystem(XrInstance instance, const void* getInfo, uint64_t* systemId);
extern XrResult xrGetSystemProperties(XrInstance instance, uint64_t systemId, void* properties);
extern XrResult xrCreateSession(XrInstance instance, const void* createInfo, XrSession* session);
extern XrResult xrDestroySession(XrSession session);
extern XrResult xrBeginSession(XrSession session, const void* beginInfo);
extern XrResult xrEndSession(XrSession session);
extern XrResult xrRequestExitSession(XrSession session);
extern XrResult xrWaitFrame(XrSession session, const void* frameWaitInfo, void* frameState);
extern XrResult xrBeginFrame(XrSession session, const void* frameBeginInfo);
extern XrResult xrEndFrame(XrSession session, const void* frameEndInfo);
extern XrResult xrCreateReferenceSpace(XrSession session, const void* createInfo, XrSpace* space);
extern XrResult xrDestroySpace(XrSpace space);
extern XrResult xrLocateSpace(XrSpace space, XrSpace baseSpace, XrTime time, void* location);
extern XrResult xrCreateActionSet(XrInstance instance, const void* createInfo, XrActionSet* actionSet);
extern XrResult xrDestroyActionSet(XrActionSet actionSet);
extern XrResult xrCreateAction(XrActionSet actionSet, const void* createInfo, XrAction* action);
extern XrResult xrDestroyAction(XrAction action);
extern XrResult xrSuggestInteractionProfileBindings(XrInstance instance, const void* suggestedBindings);
extern XrResult xrAttachSessionActionSets(XrSession session, const void* attachInfo);
extern XrResult xrSyncActions(XrSession session, const void* syncInfo);
extern XrResult xrEnumerateBoundSourcesForAction(XrSession session, const void* enumerateInfo, uint32_t sourceCapacityInput, uint32_t* sourceCountOutput, void* sources);
extern XrResult xrGetCurrentInteractionProfile(XrSession session, XrPath topLevelUserPath, void* interactionProfile);
extern XrResult xrGetActionStateBoolean(XrSession session, const void* getInfo, void* state);
extern XrResult xrGetActionStateFloat(XrSession session, const void* getInfo, void* state);
extern XrResult xrGetActionStateVector2f(XrSession session, const void* getInfo, void* state);
extern XrResult xrStringToPath(XrInstance instance, const char* pathString, XrPath* path);
extern XrResult xrPathToString(XrInstance instance, XrPath path, uint32_t bufferCapacityInput, uint32_t* bufferCountOutput, char* buffer);
extern XrResult xrPollEvent(XrInstance instance, void* eventData);
extern XrResult xrResultToString(XrInstance instance, XrResult value, void* buffer);
extern XrResult xrCreateSwapchain(XrSession session, const void* createInfo, XrSwapchain* swapchain);
extern XrResult xrDestroySwapchain(XrSwapchain swapchain);
extern XrResult xrEnumerateSwapchainFormats(XrSession session, uint32_t formatCapacityInput, uint32_t* formatCountOutput, int64_t* formats);
extern XrResult xrEnumerateSwapchainImages(XrSwapchain swapchain, uint32_t imageCapacityInput, uint32_t* imageCountOutput, void* images);
extern XrResult xrAcquireSwapchainImage(XrSwapchain swapchain, const void* acquireInfo, uint32_t* index);
extern XrResult xrWaitSwapchainImage(XrSwapchain swapchain, const void* waitInfo);
extern XrResult xrReleaseSwapchainImage(XrSwapchain swapchain, const void* releaseInfo);
extern XrResult xrEnumerateViewConfigurationViews(XrInstance instance, uint64_t systemId, uint32_t viewConfigurationType, uint32_t viewCapacityInput, uint32_t* viewCountOutput, void* views);
extern XrResult xrLocateViews(XrSession session, const void* viewLocateInfo, void* viewState, uint32_t viewCapacityInput, uint32_t* viewCountOutput, void* views);
extern XrResult xrApplyHapticFeedback(XrSession session, const void* hapticActionInfo, const void* hapticFeedback);
extern XrResult xrGetReferenceSpaceBoundsRect(XrSession session, uint32_t referenceSpaceType, void* bounds);
extern XrResult xrCreateActionSpace(XrSession session, const void* createInfo, XrSpace* space);
extern XrResult xrEnumerateReferenceSpaces(XrSession session, uint32_t spaceCapacityInput, uint32_t* spaceCountOutput, uint32_t* spaces);
extern XrResult xrGetViewConfigurationProperties(XrInstance instance, uint64_t systemId, uint32_t viewConfigurationType, void* configurationProperties);
extern XrResult xrGetInputSourceLocalizedName(XrSession session, const void* getInfo, uint32_t bufferCapacityInput, uint32_t* bufferCountOutput, char* buffer);
extern XrResult xrGetActionStatePose(XrSession session, const void* getInfo, void* state);
extern XrResult xrStopHapticFeedback(XrSession session, const void* hapticActionInfo);
extern XrResult xrEnumerateEnvironmentBlendModes(XrInstance instance, uint64_t systemId, uint32_t viewConfigurationType, uint32_t blendModeCapacityInput, uint32_t* blendModeCountOutput, uint32_t* blendModes);
extern XrResult xrStructureTypeToString(XrInstance instance, int32_t structureType, void* buffer);

// ============================================================
// Force all xr* symbols to be referenced so they appear as GLOBAL UND imports
// This ensures OpenComposite (loaded later) can resolve them through our library
// ============================================================
static const void* _xr_symbol_refs[] = {
    (const void*)&xrGetInstanceProcAddr,
    (const void*)&xrCreateInstance,
    (const void*)&xrDestroyInstance,
    (const void*)&xrEnumerateApiLayerProperties,
    (const void*)&xrEnumerateInstanceExtensionProperties,
    (const void*)&xrGetSystem,
    (const void*)&xrGetSystemProperties,
    (const void*)&xrCreateSession,
    (const void*)&xrDestroySession,
    (const void*)&xrBeginSession,
    (const void*)&xrEndSession,
    (const void*)&xrRequestExitSession,
    (const void*)&xrWaitFrame,
    (const void*)&xrBeginFrame,
    (const void*)&xrEndFrame,
    (const void*)&xrCreateReferenceSpace,
    (const void*)&xrDestroySpace,
    (const void*)&xrLocateSpace,
    (const void*)&xrCreateActionSet,
    (const void*)&xrDestroyActionSet,
    (const void*)&xrCreateAction,
    (const void*)&xrDestroyAction,
    (const void*)&xrSuggestInteractionProfileBindings,
    (const void*)&xrAttachSessionActionSets,
    (const void*)&xrSyncActions,
    (const void*)&xrEnumerateBoundSourcesForAction,
    (const void*)&xrGetCurrentInteractionProfile,
    (const void*)&xrGetActionStateBoolean,
    (const void*)&xrGetActionStateFloat,
    (const void*)&xrGetActionStateVector2f,
    (const void*)&xrStringToPath,
    (const void*)&xrPathToString,
    (const void*)&xrPollEvent,
    (const void*)&xrResultToString,
    (const void*)&xrCreateSwapchain,
    (const void*)&xrDestroySwapchain,
    (const void*)&xrEnumerateSwapchainFormats,
    (const void*)&xrEnumerateSwapchainImages,
    (const void*)&xrAcquireSwapchainImage,
    (const void*)&xrWaitSwapchainImage,
    (const void*)&xrReleaseSwapchainImage,
    (const void*)&xrEnumerateViewConfigurationViews,
    (const void*)&xrLocateViews,
    (const void*)&xrApplyHapticFeedback,
    (const void*)&xrGetReferenceSpaceBoundsRect,
    (const void*)&xrCreateActionSpace,
    (const void*)&xrEnumerateReferenceSpaces,
    (const void*)&xrGetViewConfigurationProperties,
    (const void*)&xrGetInputSourceLocalizedName,
    (const void*)&xrGetActionStatePose,
    (const void*)&xrStopHapticFeedback,
    (const void*)&xrEnumerateEnvironmentBlendModes,
    (const void*)&xrStructureTypeToString,
};

// Touch the array in a function called at JNI_OnLoad to prevent dead stripping
static void _touch_xr_refs(void) {
    volatile const void* p = _xr_symbol_refs[0];
    (void)p;
    // Make sure the array isn't optimized away
    __asm__ __volatile__("" : : "r"(_xr_symbol_refs) : "memory");
}

// ============================================================
// Vulkan functions (imported from libvulkan.so - for GLOBAL UND entries)
// Matches original's 3 UND vk symbols
// ============================================================
extern void vkGetPhysicalDeviceProperties2(void* physicalDevice, void* pProperties);
extern void vkGetDeviceQueue(void* device, uint32_t queueFamilyIndex, uint32_t queueIndex, void* pQueue);
extern void vkGetPhysicalDeviceQueueFamilyProperties(void* physicalDevice, uint32_t* pQueueFamilyPropertyCount, void* pQueueFamilyProperties);

static const void* _vk_refs[] = {
    (const void*)&vkGetPhysicalDeviceProperties2,
    (const void*)&vkGetDeviceQueue,
    (const void*)&vkGetPhysicalDeviceQueueFamilyProperties,
};
static void _touch_vk_refs(void) {
    __asm__ __volatile__("" : : "r"(_vk_refs) : "memory");
}

// ============================================================
// Static storage - backing structs for OpenComposite pointers
// ============================================================
static JavaVM* s_jvm = NULL;

// XrInstanceCreateInfoAndroidKHR - stored here, pointer exported
static XrInstanceCreateInfoAndroidKHR s_android_create_info = {
    1000296000,   // type: XR_TYPE_INSTANCE_CREATE_INFO_ANDROID_KHR
    NULL,         // next
    0,            // createFlags
    NULL,         // applicationVM
    NULL,         // applicationActivity
};

// XrGraphicsBindingOpenGLESAndroidKHR - stored here, pointer exported
static XrGraphicsBindingOpenGLESAndroidKHR s_android_gles_binding = {
    1000296001,   // type: XR_TYPE_GRAPHICS_BINDING_OPENGL_ES_ANDROID_KHR
    NULL,         // next
    NULL, NULL, NULL,
};

// ============================================================
// EXPORTED 8-byte pointers (OBJECT) - exactly like the original
// OpenComposite reads these directly when doing xrCreateInstance / xrCreateSession
// "Load Input File" callback (Android file loader for OpenComposite)
typedef size_t (*AndroidLoadInputFileFn)(const char* path, char* outBuf, size_t bufSize);

void* OpenComposite_Android_Create_Info = &s_android_create_info;
void* OpenComposite_Android_GLES_Binding_Info = &s_android_gles_binding;
void* OpenComposite_Android_Load_Input_File = NULL;

// ============================================================
// Initialize xrInitializeLoaderKHR - using xrGetInstanceProcAddr
// ============================================================
static void s_call_xrInitializeLoaderKHR(void* jvm, void* activity) {
    LOGD("Calling xrInitializeLoaderKHR via xrGetInstanceProcAddr...");

    PFN_xrVoidFunction initFunc = NULL;
    XrResult res = xrGetInstanceProcAddr((XrInstance)0, "xrInitializeLoaderKHR", &initFunc);
    if (res != 0 || initFunc == NULL) {
        // Try mangled name via dlsym
        initFunc = (PFN_xrVoidFunction)dlsym(RTLD_DEFAULT, "_Z21xrInitializeLoaderKHRPK29XrLoaderInitInfoBaseHeaderKHR");
        if (initFunc) LOGD("Found init via dlsym mangled");
    } else {
        LOGD("Found init via xrGetInstanceProcAddr");
    }

    if (initFunc) {
        XrLoaderInitInfoAndroidKHR info;
        info.base.type = 1000295000;  // XR_TYPE_LOADER_INIT_INFO_ANDROID_KHR
        info.base.next = NULL;
        info.applicationVM = jvm;
        info.applicationActivity = activity;

        typedef XrResult (*FnTy)(const XrLoaderInitInfoBaseHeaderKHR*);
        XrResult r = ((FnTy)initFunc)(&info.base);
        LOGD("xrInitializeLoaderKHR result: %d", r);
    } else {
        LOGE("xrInitializeLoaderKHR could not be resolved");
    }
}

// ============================================================
// JNI_OnLoad - minimal like original (~56 bytes equivalent logic)
// ============================================================
JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM* vm, void* reserved) {
    (void)reserved;
    s_jvm = vm;

    // Touch ref arrays to ensure symbols aren't stripped
    _touch_xr_refs();
    _touch_vk_refs();

    LOGD("JNI_OnLoad: JavaVM=%p", (void*)vm);
    return JNI_VERSION_1_4;
}

// ============================================================
// Launch thread - call runCraft() in a detached thread via AttachCurrentThread
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
// Update exported GLES binding struct so OpenComposite reads it during session create
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
    s_android_create_info.createFlags       = 0;
    s_android_create_info.applicationVM     = (void*)s_jvm;
    s_android_create_info.applicationActivity = (void*)actRef;

    LOGD("Create_Info{type=%d vm=%p act=%p}",
         s_android_create_info.type,
         s_android_create_info.applicationVM,
         s_android_create_info.applicationActivity);

    // Now try to initialize the OpenXR loader
    s_call_xrInitializeLoaderKHR((void*)s_jvm, (void*)actRef);
}