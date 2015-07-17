/*
  * c-jni.cpp
 *
 *  Created on: Jul 16, 2015
 *      Author: thiago
 */

#include <jni.h>
#include <stdio.h>
#include <iostream>
#include <string>

#define TRUE 1
#define FALSE 0

typedef struct {
	jclass clazz;
	jmethodID methodId;
} method;

jclass throwable_class;
jmethodID mid_throwable_getCause;
jmethodID mid_throwable_getStackTrace;
jmethodID mid_throwable_toString;
jclass frame_class;
jmethodID mid_frame_toString;

void _append_exception_trace_messages(JNIEnv& a_jni_env, std::string& a_error_msg,
		jthrowable a_exception, jmethodID a_mid_throwable_getCause,
		jmethodID a_mid_throwable_getStackTrace, jmethodID a_mid_throwable_toString, jmethodID a_mid_frame_toString) {

	// Get the array of StackTraceElements.
	jobjectArray frames = (jobjectArray) a_jni_env.CallObjectMethod(a_exception, a_mid_throwable_getStackTrace);
	jsize frames_length = a_jni_env.GetArrayLength(frames);

	// Add Throwable.toString() before descending
	// stack trace messages.
	if (0 != frames) {

		jstring msg_obj = (jstring) a_jni_env.CallObjectMethod(a_exception, a_mid_throwable_toString);
		const char* msg_str = a_jni_env.GetStringUTFChars(msg_obj, 0);

		// If this is not the top-of-the-trace then
		// this is a cause.
		if (!a_error_msg.empty()) {
			a_error_msg += "\nCaused by: ";
			a_error_msg += msg_str;
		} else {
			a_error_msg = msg_str;
		}

		a_jni_env.ReleaseStringUTFChars(msg_obj, msg_str);
		a_jni_env.DeleteLocalRef(msg_obj);
	}

	// Append stack trace messages if there are any.
	if (frames_length > 0) {

		jsize i = 0;

		for (i = 0; i < frames_length; i++) {

			// Get the string returned from the 'toString()'
			// method of the next frame and append it to
			// the error message.
			jobject frame = a_jni_env.GetObjectArrayElement(frames, i);
			jstring msg_obj = (jstring) a_jni_env.CallObjectMethod(frame, a_mid_frame_toString);

			const char* msg_str = a_jni_env.GetStringUTFChars(msg_obj, 0);

			a_error_msg += "\n    ";
			a_error_msg += msg_str;

			a_jni_env.ReleaseStringUTFChars(msg_obj, msg_str);
			a_jni_env.DeleteLocalRef(msg_obj);
			a_jni_env.DeleteLocalRef(frame);
		}
	}

	// If 'a_exception' has a cause then append the
	// stack trace messages from the cause.
	if (0 != frames) {

		jthrowable cause = (jthrowable) a_jni_env.CallObjectMethod(a_exception, a_mid_throwable_getCause);

		if (0 != cause) {

			_append_exception_trace_messages(a_jni_env,
					a_error_msg,
					cause,
					a_mid_throwable_getCause,
					a_mid_throwable_getStackTrace,
					a_mid_throwable_toString,
					a_mid_frame_toString);
		}
	}
}


void append_exception_trace_messages(JNIEnv& a_jni_env, std::string& a_error_msg, jthrowable a_exception) {

	_append_exception_trace_messages(a_jni_env, a_error_msg, a_exception,
			mid_throwable_getCause, mid_throwable_getStackTrace,
			mid_throwable_toString, mid_frame_toString);
}


method getMethod(JNIEnv *env, const char* classname, const char* methodname, const char* methodsig) {

	method method;
	method.clazz = 0;
	method.methodId = 0;

	jclass clazz = env->FindClass(classname);

	if (clazz == 0) {
		printf("Class %s not found\n", classname);
		return method;
	}

	method.clazz = clazz;

	//											"(argument-types)return-type"
	jmethodID methodId = env->GetMethodID(clazz, methodname, methodsig);

	if (methodId == 0) {
		printf("Method %s.%s%s not found\n", classname, methodname, methodsig);
		return method;
	}

	method.methodId = methodId;

	return method;
}

method getStaticMethod(JNIEnv *env, const char* classname, const char* methodname, const char* methodsig) {

	method method;
	method.clazz = 0;
	method.methodId = 0;

	jclass clazz = env->FindClass(classname);

	if (clazz == 0) {
		printf("Class %s not found\n", classname);
		return method;
	}

	//											"(argument-types)return-type"
	jmethodID methodId = env->GetStaticMethodID(clazz, methodname, methodsig);

	if (methodId == 0) {
		printf("Method %s.%s%s not found\n", classname, methodname, methodsig);
		return method;
	}

	method.clazz = clazz;
	method.methodId = methodId;

	return method;
}

jobject fullexecStaticObject(JNIEnv *env, const char* classname, const char* methodname, const char* methodsig, ...) {

	va_list args;
	va_start(args, methodsig);
	method method = getStaticMethod(env, classname, methodname, methodsig);

	if (method.methodId == 0)
		return 0;

	jobject result = env->CallStaticObjectMethodV(method.clazz, method.methodId, args);
	va_end(args);

	return result;
}

jobject execStaticObject(JNIEnv *env, method method, ...) {

	va_list args;
	va_start(args, method);
	jobject result = env->CallStaticObjectMethodV(method.clazz, method.methodId, args);
	va_end(args);

	return result;
}

jobject fullexecObject(JNIEnv *env, jobject object, const char* classname, const char* methodname, const char* methodsig, ...) {

	va_list args;
	va_start(args, methodsig);
	method method = getMethod(env, classname, methodname, methodsig);

	if (method.methodId == 0)
		return 0;

	jobject result = env->CallObjectMethodV(object, method.methodId, args);
	va_end(args);

	return result;
}

jobject execObject(JNIEnv *env, jobject object, method method, ...) {

	va_list args;
	va_start(args, method);
	jobject result = env->CallObjectMethodV(object, method.methodId, args);
	va_end(args);

	return result;
}

jobject fullnewObject(JNIEnv *env, const char* classname, const char* methodsig, ...) {

	va_list args;
	va_start(args, methodsig);
	const char* methodname = "<init>";
	method method = getMethod(env, classname, methodname, methodsig);

	if (method.methodId == 0) {
		printf("Method %s not found\n", methodname);
		return 0;
	}

	jobject result = env->NewObjectV(method.clazz, method.methodId, args);
	va_end(args);

	return result;
}

jobject newObject(JNIEnv *env, method method, ...) {

	va_list args;
	va_start(args, method);
	jobject result = env->NewObjectV(method.clazz, method.methodId, args);
	va_end(args);

	return result;
}

void handleEx(JNIEnv *env) {
	jthrowable t = env->ExceptionOccurred();

	if (t) {
		env->ExceptionDescribe();
		std::string s;
		append_exception_trace_messages(*env, s, t);
		std::cerr << s;
	}
}

jobject getKey(JNIEnv *env, const char* key_cstr) {

	jthrowable t;
	jstring desedejstring = env->NewStringUTF("DESede");

//    SecretKeyFactory factory = SecretKeyFactory.getInstance("DESede");
	jobject secretKeyFactory = fullexecStaticObject(env, "javax/crypto/SecretKeyFactory",
								"getInstance", "(Ljava/lang/String;)Ljavax/crypto/SecretKeyFactory;", desedejstring);

	t = env->ExceptionOccurred();

	if (t) {
		env->ExceptionDescribe();
		return 0;
	}

	// string creation
	jstring keyjstring = env->NewStringUTF(key_cstr);

//    byte[] keyBytes = "any string".getBytes();
	jobject keyBytes = fullexecObject(env, keyjstring, "java/lang/String", "getBytes", "()[B");

	return 0;

//    DESedeKeySpec desedeKeySpec = new DESedeKeySpec(keyBytes);
	jobject secretKeySpec = fullnewObject(env, "javax/crypto/spec/DESedeKeySpec", "([B)V", keyBytes);


//    SecretKey key = factory.generateSecret(desedeKeySpec);
	jobject skey = fullexecObject(env, secretKeyFactory, "javax/crypto/SecretKeyFactory", "generateSecret",
						"(Ljava/security/spec/KeySpec)Ljavax/crypto/SecretKey", secretKeySpec);

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

    // init

	throwable_class = env->FindClass("java/lang/Throwable");
	mid_throwable_getCause = env->GetMethodID(throwable_class, "getCause",
			"()Ljava/lang/Throwable;");
	mid_throwable_getStackTrace = env->GetMethodID(throwable_class,
			"getStackTrace", "()[Ljava/lang/StackTraceElement;");
	mid_throwable_toString = env->GetMethodID(throwable_class, "toString",
			"()Ljava/lang/String;");

	frame_class = env->FindClass("java/lang/StackTraceElement");
	mid_frame_toString = env->GetMethodID(frame_class, "toString",
			"()Ljava/lang/String;");

    // end init






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

//    jclass EncryptorClass = env->FindClass("br/com/facilit/portal/security/Encryptor");
//    const char* keyc_str = env->GetStringUTFChars((jstring) keyToStringValue, NULL);
//    printf("%s\n", keyc_str);

    jobject key = getKey(env, "The fox jumped over the lazy dog");

    // Clean up
    env->ReleaseStringUTFChars(jstr, str);

    // Shutdown the VM.
    vm->DestroyJavaVM();

    return 0;
}

