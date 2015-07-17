/*
 *
 *  Created on: Jul 16, 2015
 *      Author: thiago
 */

#include "jni.h"
#include "utils.h"
#include <stdio.h>

#define TRUE 1
#define FALSE 0

jobject getKey(JNIEnv *env, const char* key_cstr) {

	jstring desedejstring = env->NewStringUTF("DESede");

//    SecretKeyFactory factory = SecretKeyFactory.getInstance("DESede");
	jobject secretKeyFactory = fullexecStaticObject(env, "javax/crypto/SecretKeyFactory",
								"getInstance", "(Ljava/lang/String;)Ljavax/crypto/SecretKeyFactory;", desedejstring);

	handleEx(env);

	// string creation
	jstring keyjstring = env->NewStringUTF(key_cstr);

//    byte[] keyBytes = "any string".getBytes();
	jobject keyBytes = fullexecObject(env, keyjstring, "java/lang/String", "getBytes", "()[B");

	handleEx(env);

//    DESedeKeySpec desedeKeySpec = new DESedeKeySpec(keyBytes);
	jobject secretKeySpec = fullnewObject(env, "javax/crypto/spec/DESedeKeySpec", "([B)V", keyBytes);

	handleEx(env);

//    SecretKey key = factory.generateSecret(desedeKeySpec);
	jobject skey = fullexecObject(env, secretKeyFactory, "javax/crypto/SecretKeyFactory", "generateSecret",
						"(Ljava/security/spec/KeySpec;)Ljavax/crypto/SecretKey;", secretKeySpec);

	handleEx(env);

	return skey;
}

int main(int argc, char** argv) {

	JavaVMOption option[2];
    option[0].optionString = (char*) "-Djava.class.path=token-gen-1.0.jar:/usr/lib/jvm/java-7-oracle/jre/lib/jce.jar";
    option[1].optionString = (char*) ""; //"-verbose:jni";
    option[2].optionString = (char*) "-Djava.library.path=/usr/lib/jvm/java-7-oracle/jre/lib/";  /* set native library path */
    option[3].optionString = (char*) "-Djava.compiler=NONE";

    JavaVM *vm;
    JNIEnv *env;
    JavaVMInitArgs vm_args;
    vm_args.version = JNI_VERSION_1_6;
    vm_args.nOptions = 4;
    vm_args.options = option;
    vm_args.ignoreUnrecognized = TRUE;

    // Construct a VM
    jint res = JNI_CreateJavaVM(&vm, (void **)&env, &vm_args);

    if (res == JNI_ERR) {
    	return JNI_ERR;
    }

    // Construct a String
    jstring jstr = env->NewStringUTF("Hello World");

    // First get the class that contains the method you need to call
    jclass StringClass = env->FindClass("java/lang/String");
    jclass ObjectClass = env->FindClass("java/lang/Object");

    // Get the method that you want to call
    jmethodID to_lower = env->GetMethodID(StringClass, "toLowerCase", "()Ljava/lang/String;");

    // Call the method on the object
    jobject result = env->CallObjectMethod(jstr, to_lower);

    // Get a C-style string
    const char* str = env->GetStringUTFChars((jstring) result, NULL);
    printf("%s\n", str);

//    jclass EncryptorClass = env->FindClass("br/com/facilit/portal/security/Encryptor");
//    const char* keyc_str = env->GetStringUTFChars((jstring) keyToStringValue, NULL);
//    printf("%s\n", keyc_str);

    jobject key = getKey(env, "The fox jumped over the lazy dog");
    jstring v = (jstring) fullexecObject(env, key, "java/lang/Object", "toString", "()Ljava/lang/String;");
    const char* sv = env->GetStringUTFChars(v, NULL);
    printf("out is %s\n", sv);

    // Clean up
    env->ReleaseStringUTFChars(jstr, str);

    // Shutdown the VM.
    vm->DestroyJavaVM();

    return 0;
}

