/*
 * utils.cpp
 *
 *  Created on: Jul 17, 2015
 *      Author: thiago
 */

#include <jni.h>
#include "utils.h"
#include <iostream>
#include <string>

void _append_exception_trace_messages(JNIEnv& env, std::string& a_error_msg,
		jthrowable a_exception, jmethodID a_mid_throwable_getCause,
		jmethodID a_mid_throwable_getStackTrace, jmethodID a_mid_throwable_toString, jmethodID a_mid_frame_toString) {

	// Get the array of StackTraceElements.
	jobjectArray frames = (jobjectArray) env.CallObjectMethod(a_exception, a_mid_throwable_getStackTrace);
	jsize frames_length = env.GetArrayLength(frames);

	// Add Throwable.toString() before descending
	// stack trace messages.
	if (0 != frames) {

		jstring msg_obj = (jstring) env.CallObjectMethod(a_exception, a_mid_throwable_toString);
		const char* msg_str = env.GetStringUTFChars(msg_obj, 0);

		// If this is not the top-of-the-trace then
		// this is a cause.
		if (!a_error_msg.empty()) {
			a_error_msg += "\nCaused by: ";
			a_error_msg += msg_str;
		} else {
			a_error_msg = msg_str;
		}

		env.ReleaseStringUTFChars(msg_obj, msg_str);
		env.DeleteLocalRef(msg_obj);
	}

	// Append stack trace messages if there are any.
	if (frames_length > 0) {

		jsize i = 0;

		for (i = 0; i < frames_length; i++) {

			// Get the string returned from the 'toString()'
			// method of the next frame and append it to
			// the error message.
			jobject frame = env.GetObjectArrayElement(frames, i);
			jstring msg_obj = (jstring) env.CallObjectMethod(frame, a_mid_frame_toString);

			const char* msg_str = env.GetStringUTFChars(msg_obj, 0);

			a_error_msg += "\n    ";
			a_error_msg += msg_str;

			env.ReleaseStringUTFChars(msg_obj, msg_str);
			env.DeleteLocalRef(msg_obj);
			env.DeleteLocalRef(frame);
		}
	}

	// If 'a_exception' has a cause then append the
	// stack trace messages from the cause.
	if (0 != frames) {

		jthrowable cause = (jthrowable) env.CallObjectMethod(a_exception, a_mid_throwable_getCause);

		if (0 != cause) {

			_append_exception_trace_messages(env,
					a_error_msg,
					cause,
					a_mid_throwable_getCause,
					a_mid_throwable_getStackTrace,
					a_mid_throwable_toString,
					a_mid_frame_toString);
		}
	}
}

void append_exception_trace_messages(JNIEnv& env, std::string& a_error_msg, jthrowable a_exception) {

	jclass throwable_class;
	jmethodID mid_throwable_getCause;
	jmethodID mid_throwable_getStackTrace;
	jmethodID mid_throwable_toString;
	jclass frame_class;
	jmethodID mid_frame_toString;

	throwable_class = env.FindClass("java/lang/Throwable");
	mid_throwable_getCause = env.GetMethodID(throwable_class, "getCause",
			"()Ljava/lang/Throwable;");
	mid_throwable_getStackTrace = env.GetMethodID(throwable_class,
			"getStackTrace", "()[Ljava/lang/StackTraceElement;");
	mid_throwable_toString = env.GetMethodID(throwable_class, "toString",
			"()Ljava/lang/String;");

	frame_class = env.FindClass("java/lang/StackTraceElement");
	mid_frame_toString = env.GetMethodID(frame_class, "toString",
			"()Ljava/lang/String;");

	_append_exception_trace_messages(env, a_error_msg, a_exception,
			mid_throwable_getCause, mid_throwable_getStackTrace,
			mid_throwable_toString, mid_frame_toString);
}

const char* get_exception_trace_messages(JNIEnv& env, jthrowable a_exception) {

	std::string s;
	append_exception_trace_messages(env, s, a_exception);

	return s.c_str();
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
		printf("%s", get_exception_trace_messages(*env, t));
	}
}
