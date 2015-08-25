#!/bin/bash

/server/android-ndk-r10e/toolchains/x86-4.9/prebuilt/darwin-x86_64/bin/i686-linux-android-g++ \
  --sysroot=/server/android-ndk-r10e/platforms/android-21/arch-x86 \
  -I../../jni/include/art/runtime \
  -I../../jni/include/art/runtime/arch/x86 \
  ../../jni/art_quick_dispatcher.S -o libart_quick_dispatcher.so -c
