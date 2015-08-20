#include <jni.h>
#include "log.h"
#include "Dalvik.h"

/**
 * dvmCallMethod 需要的是 ArrayObject*, 而 nativeFunction 传入的参数是 const u4*
 * 做个转换
 */
ArrayObject* argsToArrayObject(const char* argsTypeDesc, const u4* args)
{
    //全部参数都转成Object型
    ClassObject* objectArrayClass = dvmFindArrayClass("[Ljava/lang/Object;", NULL);

    // 参数个数, 类型描述符都是一个字符, 所以 argsTypeDesc 字符串长度就是参数个数
    int argsSize = strlen(argsTypeDesc);

    // 分配空间
    ArrayObject* argArray = dvmAllocArrayByClass(objectArrayClass,argsSize ,ALLOC_DEFAULT);

    // 挨个赋值
    // 因为 args 并不是一个真正意义上的"数组", 当long 或者 double 形, 会占用 args[i]和args[i+1]
    // 所以需要多一个下标来指向args内存
    int pArgs = 0;
    for(int i=0;i<argsSize;i++)
    {
        // 当前参数类型描述符
        const char descChar = argsTypeDesc[i];

        JValue value;
        Object* obj;

        switch(descChar)
        {

            case 'Z':
            case 'C':
            case 'F':
            case 'B':
            case 'S':
            case 'I':

                //如果是基本数据类型, 则使用java的"自动装配", 也就是 int 转 Integer, long 转 Long, 这样就都转成Object了
                value.i = args[pArgs++];
                obj = (Object*) dvmBoxPrimitive(value, dvmFindPrimitiveClass(descChar));
                dvmReleaseTrackedAlloc(obj, dvmThreadSelf());
                break;
            case 'D':
            case 'J':

                // 基本数据类型中, double 和 long 占用两个字节, 所以 pArgs 需要 +2
                value.j = dvmGetArgLong(args, pArgs++);
                pArgs++;
                obj = (Object*) dvmBoxPrimitive(value, dvmFindPrimitiveClass(descChar));
                dvmReleaseTrackedAlloc(obj, dvmThreadSelf());
                break;
            case '[':
            case 'L':
                obj  = (Object*) args[pArgs++];
                break;
            default:
                LOGE("Unknown method signature description character: %c\n", descChar);
                obj = NULL;
        }

        // 塞到ArrayObject 中
        dvmSetObjectArrayElement(argArray,i,obj);
    }

    return argArray;
}

/**
 * 被hook的函数统一调用这个native方法, 由他来装配之后跳转到Handler
 */
static void forwardToHander(const u4* args, JValue* pResult,
                          const Method* method, struct Thread* self) {

    // 之前放进去的 "附加参数"
    Object* handlerObject = const_cast<Object*>(reinterpret_cast<const Object*>(method->insns));

    // 在原方法执行之前, 我们是要调用 Handler 的 before 这个hook
    Method* handlerBeforeMethod = dvmFindInterfaceMethodHierByDescriptor(handlerObject->clazz,"before","(Ljava/lang/Object;[Ljava/lang/Object;)V");

    if(nullptr != handlerBeforeMethod)
    {
        LOGD("handlerBeforeMethod name:%s,desc:%s",handlerBeforeMethod->name,handlerBeforeMethod->shorty);
        JValue* result = new JValue();

        // method->shorty[0] 表示返回值类型, 比如"ILJ" 说明原函数有一个Object型参数和一个long型参数, 返回值为 整形
        // 注意自动装配类型, int long double, 如果是Integer, Long, Double, 则描述符为Ljava/lang/Integer,Ljava/lang/Long和Ljava/lang/Double
        const char returnTypeDesc = method->shorty[0];
        const char* argsTypeDesc = &(method->shorty[1]);

        // 转一下参数格式
        ArrayObject* argsArray ;
        Object* originalInstance;

        // 同样的, 如果原函数不是static 的, args[0] 函数是所在类的实例
        if(dvmIsStaticMethod(method))
        {
            argsArray = argsToArrayObject(argsTypeDesc,&(args[0]));
            originalInstance = nullptr;
        }else{
            argsArray = argsToArrayObject(argsTypeDesc,&(args[1]));
            originalInstance = (Object*)(args[0]);
        }


        dvmCallMethod(self,handlerBeforeMethod, handlerObject, result, originalInstance ,argsArray);
    }else{
        LOGE("method no found....");
    }

    // before 前置调用先不修改原返回值了
    pResult->l = nullptr;


}


/**
 * 设置hook, 将原函数调用转发给nativeFunc forwardToHander 进行处理
 */
extern "C" JNIEXPORT void JNICALL
Java_com_zhaoxiaodan_hookdemo_Demo2_hook(JNIEnv *env, jobject instance, jobject clazzToHook,
                                                 jstring methodName_, jstring methodSig_,
                                                 jobject handler) {
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
        Method* method = reinterpret_cast<Method*>(methodIDToHook);

        LOGD("hook function %s, args desc:%s",method->name,method->shorty);

        SET_METHOD_FLAG(method, ACC_NATIVE);
        //转发
        method->nativeFunc = &forwardToHander;
        method->registersSize=method->insSize;
        method->outsSize=0;

        // "附加参数" 功能
        // 在hook 的时候传进来的 handler 实现实例, 我们放到Method 结构体的insns 属性中
        // 到时候dvm 调用 nativeFunc 的时候会带回到 nativeFunc 中, 我们就能取到了
        method->insns = reinterpret_cast<const u2*>(dvmDecodeIndirectRef(dvmThreadSelf(),handler));
    }

    env->ReleaseStringUTFChars(methodName_, methodName);
    env->ReleaseStringUTFChars(methodSig_, methodSig);
}