/* Copyright (c) 2021 Connected Way, LLC. All rights reserved.
 * Use of this source code is governed by a Creative Commons 
 * Attribution-NoDerivatives 4.0 International license that can be
 * found in the LICENSE file.
 */
#if !defined(__OFC_JNIUTILS_H__)
#define __OFC_JNIUTILS_H__

#include <jni.h>

#include "ofc/core.h"
#include "ofc/types.h"
#include "ofc/config.h"
#include "ofc/handle.h"

OFC_SIZET jstrlen (const jchar *jstr) ;
jchar *tchar2jchar (OFC_CTCHAR *tstr) ;
OFC_LPTSTR jstr2tchar (JNIEnv *env, jstring jstrPath) ;
OFC_LPSTR jstr2char (JNIEnv *env, jstring jstrPath) ;
jstring tchar2jstr (JNIEnv *env, OFC_LPCTSTR tstrPath) ;
OFC_TCHAR *jchar2tchar (const jchar *jstr, jsize len) ;
OFC_CHAR *jchar2char (const jchar *jstr, jsize len) ;
OFC_LPTSTR file_get_path (JNIEnv *env, jobject objFile) ;
OFC_VOID file_free_path (OFC_LPTSTR path) ;
jobject new_file (JNIEnv *env, jstring jstrPath) ;
jobject new_child_file (JNIEnv *env, jobject objParent, jstring jstrPath) ;
jobject new_file_uri (JNIEnv *env, jobject objURI) ;
jobject new_file_from_uri (JNIEnv *env, OFC_LPCTSTR path) ;
jobject new_file_from_path (JNIEnv *env, OFC_LPCTSTR path) ;
jobject new_uri (JNIEnv *env, OFC_LPCTSTR path) ;
jobject new_fd (JNIEnv *env, jlong hFile) ;
OFC_HANDLE file_descriptor_get_handle (JNIEnv *env, jobject objFd) ;
#endif

