#include <jni.h>
#include <string>

#include "cpu/CPUUtils.h"

extern "C"
JNIEXPORT jboolean JNICALL
Java_com_example_bindbigcorecpu_MainActivity_bringToBigCore(JNIEnv *env, jobject thiz) {
    if (CPUUtils::bingToBigCore()) {
        return JNI_TRUE;
    }
    return JNI_FALSE;
}