#include <jni.h>
#include "log.h"
#include "Dalvik.h"


void showMethodInfo(const Method* method)
{
    //看看method的各个属性都是啥:
    LOGD("accessFlags:%d",method->accessFlags);
    LOGD("clazz->descriptor:%s",method->clazz->descriptor);
    LOGD("clazz->sourceFile:%s",method->clazz->sourceFile);
    LOGD("methodIndex:%d",method->methodIndex);
    LOGD("name:%s",method->name);
    LOGD("shorty:%s",method->shorty);
}

/**
 * 替换原来java类中test方法的 本地c 函数
 * args : 原来函数的参数数组
 * pResult: 返回值
 */
static void newTestMethod(const u4* args, JValue* pResult,
                          const Method* method, struct Thread* self) {

    showMethodInfo(method);

    // args 是原来函数的参数数组, 原来test函数只有一个String型参数
    // 并且要注意, 如果是不是static函数, 下标0 是函数所在类的实例obj
    // 在dvm中Object,  jni 中的jobject 和 java 中的 Object类 都不是同一个东西
    // String类对应StringObject
    // 取出参数打印出来看看
    StringObject* param1 = NULL;

    if(dvmIsStaticMethod(method))
        param1 = (StringObject*)args[0];
    else
        param1 = (StringObject*)args[1];
    LOGD("param1:%s",dvmCreateCstrFromString(param1));

    //JValue 是个union ,要返回int 就 pResult->i=1; 返回Object对象就 pResult->l = ojb;
    // 但是, 在dvm中的Object,  jni 中的jobject 和 java 中的 Object类 都不是同一个东西
    // 所以, 我们这里使用dvm的函数来创建一个StringObject*
    pResult->l = dvmCreateStringFromCstr("newTestMethod");

    // 一般情况应该使用宏 : RETURN_XXX(result);
    return;
}

/**
 * hook 的jni 函数
 */
extern "C" JNIEXPORT void JNICALL
Java_com_zhaoxiaodan_hookdemo_MainActivity_hook(JNIEnv *env, jobject instance, jobject clazzToHook,
                                                jstring methodName_, jstring methodSig_) {

    const char *methodName = env->GetStringUTFChars(methodName_, 0);
    const char *methodSig = env->GetStringUTFChars(methodSig_, 0);


    jmethodID methodIDToHook = env->GetMethodID((jclass) clazzToHook,methodName,methodSig);

    // 找不到有可能是个static
    if(nullptr == methodIDToHook){
        env->ExceptionClear();
        methodIDToHook = env->GetStaticMethodID((jclass) clazzToHook,methodName,methodSig);
    }


    if(methodIDToHook != nullptr)
    {
        //主要在这里替换
        //jmethodID 在dvm里实际上就是个Method 结构体
        Method* method = (Method*) methodIDToHook;

        //看看method的各个属性都是啥:
        showMethodInfo(method);

        //设置Method 的 accessFlags 为 枚举型
        // ACC_NATIVE 表示 这个method 切换成了一个native 方法
        // 这个枚举 在 dalvik/libdex/DexFile.h
        // 类似:
        // ACC_PUBLIC       = 0x00000001,       // class, field, method, ic
        // ACC_PRIVATE      = 0x00000002,       // field, method, ic
        // ACC_PROTECTED    = 0x00000004,       // field, method, ic
        SET_METHOD_FLAG(method, ACC_NATIVE);

        //既然是一个native方法, 那就把 nativeFunc 指针指向我们的hook, 用来替换test的新方法
        method->nativeFunc = &newTestMethod;

        // registersSize是函数栈大小, insSize是参数占用大小
        // 如果是native方法, 就没有额外开销了
        // 所有开销就是参数占用, 所以把它设置成跟参数占用空间
        method->registersSize=method->insSize;

        //未知
        method->outsSize=0;
    }

    env->ReleaseStringUTFChars(methodName_, methodName);
    env->ReleaseStringUTFChars(methodSig_, methodSig);
}

extern "C" JNIEXPORT jint JNI_OnLoad(JavaVM* vm, void* reserved)
{

    JNIEnv *env = nullptr;
    if (vm->GetEnv((void**) &env, JNI_VERSION_1_6) != JNI_OK) {
        return -1;
    }

    return JNI_VERSION_1_6;
}
