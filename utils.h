/*
 * utils.h
 *
 *  Created on: Jul 17, 2015
 *      Author: thiago
 */

#include <jni.h>
#include <string>

#ifndef UTILS_H_
#define UTILS_H_

const char* get_exception_trace_messages(JNIEnv& a_jni_env, jthrowable a_exception);

void _append_exception_trace_messages(JNIEnv& a_jni_env, std::string& a_error_msg,
		jthrowable a_exception, jmethodID a_mid_throwable_getCause,
		jmethodID a_mid_throwable_getStackTrace, jmethodID a_mid_throwable_toString, jmethodID a_mid_frame_toString);


void append_exception_trace_messages(JNIEnv& a_jni_env, std::string& a_error_msg, jthrowable a_exception);

typedef struct {
	jclass clazz;
	jmethodID methodId;
} method;

method getMethod(JNIEnv *env, const char* classname, const char* methodname, const char* methodsig);
method getStaticMethod(JNIEnv *env, const char* classname, const char* methodname, const char* methodsig);

jobject fullexecStaticObject(JNIEnv *env, const char* classname, const char* methodname, const char* methodsig, ...);
jobject execStaticObject(JNIEnv *env, method method, ...);

jobject fullexecObject(JNIEnv *env, jobject object, const char* classname, const char* methodname, const char* methodsig, ...);
jobject execObject(JNIEnv *env, jobject object, method method, ...);

jobject fullnewObject(JNIEnv *env, const char* classname, const char* methodsig, ...);
jobject newObject(JNIEnv *env, method method, ...);

void handleEx(JNIEnv *env);

#endif /* UTILS_H_ */
