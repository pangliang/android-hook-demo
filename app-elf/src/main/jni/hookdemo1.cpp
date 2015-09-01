//
// Created by liangwei on 15/8/24.
//

#include <jni.h>
#include <dlfcn.h>


#include "log.h"

#ifdef create_test_function
void testFunction(const char* p1, int p2)
{
    LOGD("==============in testFunction p1=%s, p2=%d",p1,p2);
}


extern "C" JNIEXPORT void JNICALL
Java_com_zhaoxiaodan_hookdemo_elf_Demo1_callTestFunction(JNIEnv *env, jobject instance, jstring p1_,
                                                         jint p2) {
    const char *p1 = env->GetStringUTFChars(p1_, 0);

    testFunction(p1,p2);

    env->ReleaseStringUTFChars(p1_, p1);
}
#else

void replaceFunction()
{
    LOGD("==============replaceFunction");
}


extern "C" JNIEXPORT void JNICALL
Java_com_zhaoxiaodan_hookdemo_elf_Demo1_hook(JNIEnv *env, jobject instance, jstring soname_,
                                             jstring functionName_) {
    const char *soname = env->GetStringUTFChars(soname_, 0);
    const char *functionName = env->GetStringUTFChars(functionName_, 0);

    void (*hookFunction)(const char*,int);
    *(void **)(hookFunction) = dlsym(RTLD_DEFAULT,functionName);

    if(nullptr == hookFunction)
    {
        LOGE("can't find func %s, error:%s",functionName,dlerror());
    } else{
        LOGD("invokeOriginalMethod");
        hookFunction("hook",222);
    }

//    void* handle = dlopen(soname,RTLD_NOW);
//    if(nullptr == handle)
//    {
//        LOGE("open lib %s error:%s",soname,dlerror());
//    }else{
//        void (*hookFunction)(const char*,int);
//        *(void **)(hookFunction) = dlsym(handle,functionName);
//
//        if(nullptr == hookFunction)
//        {
//            LOGE("can't find func %s, error:%s",functionName,dlerror());
//        } else{
//            LOGD("invokeOriginalMethod");
//            hookFunction("hook",222);
//        }
//
//    }

    env->ReleaseStringUTFChars(soname_, soname);
    env->ReleaseStringUTFChars(functionName_, functionName);
}

#endif