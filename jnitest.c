/*
 * jnitest.c
 *
 *  Created on: Jul 16, 2015
 *      Author: thiago
 */

#include<jni.h>
#include<stdio.h>

int main(int argc, char** argv) {

	JavaVMOption option;
    option.optionString = "-Djava.class.path=token-gen-1.0.jar";

    JavaVM *vm;
    JNIEnv *env;
    JavaVMInitArgs vm_args;
    vm_args.version = JNI_VERSION_1_6;
    vm_args.nOptions = 1;
    vm_args.options = &option;
    vm_args.ignoreUnrecognized = 1;

    // Construct a VM
    jint res = JNI_CreateJavaVM(&vm, (void **)&env, &vm_args);

    if (res == JNI_ERR) {
    	return JNI_ERR;
    }

    // Construct a String
    jstring jstr = env->NewStringUTF("Hello World");

    // First get the class that contains the method you need to call
    jclass clazz = env->FindClass("java/lang/String");

    // Get the method that you want to call
    jmethodID to_lower = env->GetMethodID(clazz, "toLowerCase", "()Ljava/lang/String;");
    // Call the method on the object
    jobject result = env->CallObjectMethod(jstr, to_lower);

    // Get a C-style string
    const char* str = env->GetStringUTFChars((jstring) result, NULL);
    printf("%s\n", str);

    jclass encriptor = env->FindClass("br/com/facilit/portal/security/Encryptor");

    //											"(argument-types)return-type"
    jmethodID generateKey = env->GetStaticMethodID(encriptor, "generateKey", "()Ljava/security/Key;");
    jobject key = env->CallStaticObjectMethod(encriptor, generateKey);

    jclass keyClass = env->FindClass("java/security/Key");
    jmethodID keyToString = env->GetMethodID(keyClass, "toString", "()Ljava/lang/String;");
    jobject keyToStringValue = env->CallObjectMethod(key, keyToString);

    const char* keyc_str = env->GetStringUTFChars((jstring) keyToStringValue, NULL);
    printf("%s\n", keyc_str);


    // Clean up
    env->ReleaseStringUTFChars(jstr, str);

    // Shutdown the VM.
    vm->DestroyJavaVM();

    return 0;
}

typedef struct {
	jclass clazz;
	jmethodID methodId;
} method;

method getMethod(JNIEnv *env, const char* classname, const char* methodname, const char* methodsig) {

	jclass clazz = env->FindClass(classname);
	//											"(argument-types)return-type"
	jmethodID methodId = env->GetStaticMethodID(clazz, methodname, methodsig);

	method method;
	method.clazz = clazz;
	method.methodId = methodId;

	return method;
}

jobject fullexecStaticObject(JNIEnv *env, const char* classname, const char* methodname, const char* methodsig, ...) {

	va_list args;
	va_start(args, methodsig);
	method method = getMethod(env, classname, methodname, methodsig);
	jobject result = env->CallStaticObjectMethod(method.clazz, method.methodId, args);
	va_end(args);

	return result;
}

jobject execStaticObject(JNIEnv *env, method method, ...) {

	va_list args;
	va_start(args,method);
	jobject result = env->CallStaticObjectMethod(method.clazz, method.methodId, args);
	va_end(args);

	return result;
}
