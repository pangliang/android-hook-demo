//
// Created by liangwei on 15/8/24.
//

#include <jni.h>
#include "log.h"

#include <runtime/thread.h>
#include <mirror/art_method.h>
#include <mirror/array.h>
#include <mirror/class.h>
#include <well_known_classes.h>
#include <mirror/object-inl.h>
#include <scoped_thread_state_change.h>
#include <entrypoints/entrypoint_utils.h>
#include <runtime/reflection.h>

using art::mirror::ArtMethod;
using art::mirror::Array;
using art::mirror::ObjectArray;
using art::mirror::Class;
using art::mirror::Object;
using art::PrettyMethod;
using art::Runtime;
using art::Thread;

using art::WellKnownClasses;
using art::Primitive;
using art::BoxPrimitive;
using art::UnboxPrimitiveForResult;
using art::UnboxPrimitiveForField;
using art::JValue;
using art::ScopedObjectAccess;
using art::ScopedObjectAccessUnchecked;
using art::ScopedObjectAccessAlreadyRunnable;
using art::StackHandleScope;
using art::Handle;
using art::MethodHelper;
using art::StackReference;
using art::ClassLinker;
using art::ScopedJniEnvLocalRefState;
using art::ThrowLocation;

extern "C" void art_quick_dexposed_invoke_handler();

extern "C" uint64_t artQuickDexposedInvokeHandler(ArtMethod *proxy_method,
                                                  Object *receiver, Thread *self,
                                                  StackReference<ArtMethod> *sp)
SHARED_LOCKS_REQUIRED(Locks::mutator_lock_) {

    LOG(INFO) << "dexposed >>> artQuickDexposedInvokeHandler " << PrettyMethod(proxy_method);;

    return 0;
}


extern "C" JNIEXPORT void JNICALL
Java_com_zhaoxiaodan_hookdemo_art_Demo1_hook(JNIEnv *env, jobject instance, jobject clazzToHook,
jstring methodName_, jstring methodSig_) {
    const char *methodName = env->GetStringUTFChars(methodName_, 0);
    const char *methodSig = env->GetStringUTFChars(methodSig_, 0);

    ScopedObjectAccess soa(env);
    art::Thread *self = art::Thread::Current();

    jmethodID methodIDToHook = env->GetMethodID((jclass) clazzToHook,methodName,methodSig);

    // 找不到有可能是个static
    if(nullptr == methodIDToHook){
        env->ExceptionClear();
        methodIDToHook = env->GetStaticMethodID((jclass) clazzToHook,methodName,methodSig);
    }


    if(methodIDToHook != nullptr)
    {
        // 跟delvik 一样, art中的 jmethodID 实际上也是一个 art中的 ArtMethod 类
        // delvik 是结构体, art 中是类, art 使用得更多的是c++11的特性
        ArtMethod *artmeth = reinterpret_cast<ArtMethod *>(methodIDToHook);
        artmeth->SetEntryPointFromQuickCompiledCode((const void *)art_quick_dexposed_invoke_handler);
    }


    env->ReleaseStringUTFChars(methodName_, methodName);
    env->ReleaseStringUTFChars(methodSig_, methodSig);
}