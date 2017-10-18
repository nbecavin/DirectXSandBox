#include <jni.h>
#include <string>

extern "C"
jstring
Java_fr_becavin_gijoe_sandboxdroid_MainActivity_stringFromJNI(
        JNIEnv* env,
        jobject /* this */) {
    std::string hello = "Hello from C++";
    return env->NewStringUTF(hello.c_str());
}
