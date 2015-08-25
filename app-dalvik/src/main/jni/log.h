//
// Created by liangwei on 15/8/14.
//

#ifndef ANDROID_HOOK_DEMO_LOG_H
#define ANDROID_HOOK_DEMO_LOG_H

#include "android/log.h"
#include "Dalvik.h"

#define  LOG_TAG    "[---hookdemo---]"

#define  LOGE(...)  __android_log_print(ANDROID_LOG_ERROR,LOG_TAG,__VA_ARGS__)
#define  LOGW(...)  __android_log_print(ANDROID_LOG_WARN,LOG_TAG,__VA_ARGS__)
#define  LOGD(...)  __android_log_print(ANDROID_LOG_DEBUG,LOG_TAG,__VA_ARGS__)
#define  LOGI(...)  __android_log_print(ANDROID_LOG_INFO,LOG_TAG,__VA_ARGS__)

#endif //ANDROID_HOOK_DEMO_LOG_H
