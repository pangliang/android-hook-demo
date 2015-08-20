#include <jni.h>

extern "C" JNIEXPORT jint JNI_OnLoad(JavaVM* vm, void* reserved)
{

    JNIEnv *env = nullptr;
    if (vm->GetEnv((void**) &env, JNI_VERSION_1_6) != JNI_OK) {
        return -1;
    }

    return JNI_VERSION_1_6;
}
