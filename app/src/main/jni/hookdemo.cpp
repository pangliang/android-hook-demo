#include <jni.h>
#include "log.h"
#include "vm/oo/Object.h"
#include "vm/Common.h"
#include "dlfcn.h"


StringObject* dvmCreateStringFromCstr(const char* utf8Str);
extern "C"{

typedef StringObject* (*p_dvmCreateStringFromCstr)(const char* utf8Str);
p_dvmCreateStringFromCstr _dvmCreateStringFromCstr;
}


JNIEnv *_env;

static void newTestMethod(const u4* args, JValue* pResult,
                          const Method* method, struct Thread* self) {

    void *ldvm = (void*)dlopen("libdvm.so", RTLD_LAZY);
    _dvmCreateStringFromCstr = (p_dvmCreateStringFromCstr)dlsym(ldvm, "dvmCreateStringFromCstr");
    if(_dvmCreateStringFromCstr != nullptr)
    {
        pResult->l = dvmCreateStringFromCstr("2222222");
    }else{
        pResult->l = dvmCreateStringFromCstr("2222222");
        LOGE("dlerror:%s", dlerror());
//        pResult->l = nullptr;
    }


    return;
}


extern "C" JNIEXPORT void JNICALL
Java_com_zhaoxiaodan_hookdemo_MainActivity_hook2(JNIEnv *env, jobject instance, jobject clazzToHook,
                                                 jstring methodName_, jobject proxyClazz,
                                                 jstring proxyMethod_, jstring methodSig_) {
    const char *methodName = env->GetStringUTFChars(methodName_, 0);
    const char *proxyMethod = env->GetStringUTFChars(proxyMethod_, 0);
    const char *methodSig = env->GetStringUTFChars(methodSig_, 0);

    jmethodID methodIDToHook = env->GetMethodID((jclass) clazzToHook,methodName,methodSig);
    jmethodID proxyMethodId = env->GetMethodID((jclass) proxyClazz,proxyMethod,methodSig);

    if(methodIDToHook != nullptr and proxyMethodId != nullptr)
    {
        ((Method*)methodIDToHook)->nativeFunc = ((Method*)proxyMethodId)->nativeFunc;
        ((Method*)methodIDToHook)->insSize = ((Method*)proxyMethodId)->insSize;

    }else{
        LOGE("............method null");
    }

    env->ReleaseStringUTFChars(methodName_, methodName);
    env->ReleaseStringUTFChars(proxyMethod_, proxyMethod);
    env->ReleaseStringUTFChars(methodSig_, methodSig);
}

extern "C" JNIEXPORT void JNICALL
Java_com_zhaoxiaodan_hookdemo_MainActivity_hook(JNIEnv *env, jobject instance, jobject clazzToHook,
                                                jstring methodName_, jstring methodSig_) {

    _env=env;
    const char *methodName = env->GetStringUTFChars(methodName_, 0);
    const char *methodSig = env->GetStringUTFChars(methodSig_, 0);


    jmethodID methodIDToHook = env->GetMethodID((jclass) clazzToHook,methodName,methodSig);

    if(methodIDToHook != nullptr)
    {
        Method* method = (Method*) methodIDToHook;

        SET_METHOD_FLAG(method, ACC_NATIVE);
        method->nativeFunc = &newTestMethod;
        method->registersSize=method->insSize;
        method->outsSize=0;
    }


    env->ReleaseStringUTFChars(methodName_, methodName);
    env->ReleaseStringUTFChars(methodSig_, methodSig);
}

