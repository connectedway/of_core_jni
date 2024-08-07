/* Copyright (c) 2021 Connected Way, LLC. All rights reserved.
 * Use of this source code is governed by a Creative Commons 
 * Attribution-NoDerivatives 4.0 International license that can be
 * found in the LICENSE file.
 */
#define __OFC_CORE_DLL__
#include <jni.h>

#include "ofc/config.h"
#include "ofc/types.h"
#include "ofc/heap.h"
#include "ofc/libc.h"
#include "ofc/path.h"
#include "ofc/handle.h"
#include "ofc/process.h"

#include "ofc_jni/com_connectedway_io_Utils.h"

OFC_SIZET 
jstrlen (const jchar *jstr)
{
  OFC_SIZET ret ;
  const jchar *p ;

  if (jstr == OFC_NULL)
    ret = 0 ;
  else
    for (ret = 0, p = jstr ; *p != (jchar)0 ; ret++, p++) ;
  return (ret) ;
}

jchar *
tchar2jchar (OFC_CTCHAR *tstr)
{
  OFC_SIZET len ;
  jchar *jstr ;
  OFC_INT i ;
  jchar *pjstr ;
  OFC_CTCHAR *ptstr ;

  jstr = OFC_NULL ;
  if (tstr != OFC_NULL)
    {
      len = ofc_tstrlen (tstr) ;
      jstr = ofc_malloc ((len + 1) * sizeof (jchar)) ;
      pjstr = jstr ;
      ptstr = tstr ;
      for (i = 0 ; i < len ; i++)
	*pjstr++ = (jchar) (*ptstr++) ;
      *pjstr = (jchar) '\0' ;
    }
  return (jstr) ;
}

OFC_LPTSTR jstr2tchar (JNIEnv *env, jstring jstrPath)
{
  const jchar *jcharPath ;
  OFC_LPTSTR tstrPath ;

  jcharPath = (*env)->GetStringChars (env, jstrPath, NULL) ;
  tstrPath = jchar2tchar (jcharPath, (*env)->GetStringLength (env, jstrPath)) ;
  (*env)->ReleaseStringChars (env, jstrPath, jcharPath) ;
  return (tstrPath) ;
}

OFC_LPSTR jstr2char (JNIEnv *env, jstring jstrPath)
{
  const jchar *jcharPath ;
  OFC_LPSTR strPath;

  jcharPath = (*env)->GetStringChars (env, jstrPath, NULL) ;
  strPath = jchar2char (jcharPath, (*env)->GetStringLength (env, jstrPath)) ;
  (*env)->ReleaseStringChars (env, jstrPath, jcharPath) ;
  return (strPath) ;
}

jstring tchar2jstr (JNIEnv *env, OFC_LPCTSTR tstrPath)
{
  jchar *jcharPath ;
  jstring jstrPath ;

  jcharPath = tchar2jchar (tstrPath) ;

  jstrPath = (*env)->NewString (env, jcharPath, (jsize) jstrlen (jcharPath)) ;
  ofc_free (jcharPath) ;

  return (jstrPath) ;
}

OFC_TCHAR *
jchar2tchar (const jchar *jstr, jsize len)
{
  OFC_TCHAR *tstr ;
  OFC_INT i ;
  const jchar *pjstr ;
  OFC_TCHAR *ptstr ;

  tstr = OFC_NULL ;
  if (jstr != OFC_NULL)
    {
      tstr = ofc_malloc ((len + 1) * sizeof (OFC_TCHAR)) ;
      ptstr = tstr ;
      pjstr = jstr ;
      for (i = 0 ; i < len ; i++)
	{
	  *ptstr++ = (OFC_TCHAR) *pjstr++ ;
	}
      *ptstr = TCHAR_EOS ;
    }
  return (tstr) ;
}

OFC_CHAR *
jchar2char (const jchar *jstr, jsize len)
{
  OFC_CHAR *str ;
  OFC_INT i ;
  const jchar *pjstr ;
  OFC_CHAR *pstr ;

  str = OFC_NULL ;
  if (jstr != OFC_NULL)
    {
      str = ofc_malloc ((len + 1) * sizeof (OFC_CHAR)) ;
      pstr = str ;
      pjstr = jstr ;
      for (i = 0 ; i < len ; i++)
	{
	  *pstr++ = (OFC_CHAR) *pjstr++ ;
	}
      *pstr = '\0' ;
    }
  return (str) ;
}

OFC_LPTSTR file_get_path (JNIEnv *env, jobject objFile)
{
  jmethodID midToString ;
  jstring jstringFile ;
  OFC_LPTSTR tstrFile ;
  jclass clsOfcFile ;

  clsOfcFile = (*env)->FindClass
    (env, "com/connectedway/io/File") ;
  midToString = (*env)->GetMethodID 
    (env, clsOfcFile, "toString", "()Ljava/lang/String;") ;
  (*env)->DeleteLocalRef (env, clsOfcFile) ;

  jstringFile = (*env)->CallObjectMethod (env, objFile, midToString) ;

  tstrFile = jstr2tchar (env, jstringFile) ;
  (*env)->DeleteLocalRef (env, jstringFile) ;

  return (tstrFile) ;
}

OFC_VOID file_free_path (OFC_LPTSTR path)
{
  ofc_free (path) ;
}

jobject new_file (JNIEnv *env, jstring jstrPath)
{
  jobject objFile ;
  jmethodID midNewFile ;
  jclass clsOfcFile ;

  clsOfcFile = (*env)->FindClass
    (env, "com/connectedway/io/File") ;
  midNewFile = (*env)->GetMethodID 
    (env, clsOfcFile, "<init>", "(Ljava/lang/String;)V") ;

  objFile = (*env)->NewObject (env, clsOfcFile, midNewFile, jstrPath) ;
  (*env)->DeleteLocalRef (env, clsOfcFile) ;

  return (objFile) ;
}

jobject new_child_file (JNIEnv *env, jobject objParent, jstring jstrPath)
{
  jobject objFile ;
  jmethodID midNewFile ;
  jclass clsOfcFile ;

  clsOfcFile = (*env)->FindClass
    (env, "com/connectedway/io/File") ;
  midNewFile = (*env)->GetMethodID 
    (env, clsOfcFile, "<init>", "(Lcom/connectedway/io/File;Ljava/lang/String;)V") ;

  objFile = (*env)->NewObject (env, clsOfcFile, midNewFile, objParent, 
			       jstrPath) ;
  (*env)->DeleteLocalRef (env, clsOfcFile) ;

  return (objFile) ;
}

jobject new_file_uri (JNIEnv *env, jobject objURI)
{
  jobject objFile ;
  jmethodID midNewFile ;
  jclass clsOfcFile ;

  clsOfcFile = (*env)->FindClass
    (env, "com/connectedway/io/File") ;
  midNewFile = (*env)->GetMethodID 
    (env, clsOfcFile, "<init>", "(Ljava/net/URI;)V") ;

  objFile = (*env)->NewObject (env, clsOfcFile, midNewFile, objURI) ;
  (*env)->DeleteLocalRef (env, clsOfcFile) ;

  return (objFile) ;
}

jobject new_file_from_uri (JNIEnv *env, OFC_LPCTSTR path) 
{
  jobject objFile ;
  jobject objURI ;

  objURI = new_uri (env, path) ;
  objFile = new_file_uri (env, objURI) ;
  (*env)->DeleteLocalRef (env, objURI) ;

  return (objFile) ;
}

jobject new_file_from_path (JNIEnv *env, OFC_LPCTSTR path) 
{
  jobject objFile ;
  jstring jstrPath ;
  jchar *jPath ;

  jPath = tchar2jchar (path) ;
  jstrPath = (*env)->NewString (env, jPath, (jsize) jstrlen (jPath)) ;
  ofc_free (jPath) ;

  objFile =  new_file(env, jstrPath) ;
  (*env)->DeleteLocalRef (env, jstrPath) ;

  return (objFile) ;
}

jobject new_uri (JNIEnv *env, OFC_LPCTSTR path)
{
  jstring str ;
  jobject objURI ;
  jmethodID midNewURI ;
  jclass clsURI ;

  str = tchar2jstr (env, path) ;

  clsURI = (*env)->FindClass
    (env, "java/net/URI") ;
  midNewURI = (*env)->GetMethodID 
    (env, clsURI, "<init>", "(Ljava/lang/String;)V") ;

  objURI = (*env)->NewObject (env, clsURI, midNewURI, str) ;
  (*env)->DeleteLocalRef (env, clsURI) ;
  (*env)->DeleteLocalRef (env, str) ;

  return (objURI) ;
}

jobject new_fd (JNIEnv *env, jlong hFile)
{
  jobject objFd ;
  jmethodID midNewFileDescriptor ;
  jclass clsOfcFileDescriptor ;

  clsOfcFileDescriptor = (*env)->FindClass
    (env, "com/connectedway/io/FileDescriptor") ;
  midNewFileDescriptor = (*env)->GetMethodID 
    (env, clsOfcFileDescriptor, "<init>", "(J)V") ;

  objFd = (*env)->NewObject (env, clsOfcFileDescriptor, 
			     midNewFileDescriptor, hFile) ;
  (*env)->DeleteLocalRef (env, clsOfcFileDescriptor) ;

  return (objFd) ;
}

OFC_HANDLE file_descriptor_get_handle (JNIEnv *env, jobject objFd)
{
  OFC_HANDLE hFile ;

  jmethodID midGetHandle ;
  jclass clsOfcFileDescriptor ;

  clsOfcFileDescriptor = (*env)->FindClass
    (env, "com/connectedway/io/FileDescriptor") ;
  midGetHandle = (*env)->GetMethodID 
    (env, clsOfcFileDescriptor, "getHandle", "()J") ;
  (*env)->DeleteLocalRef (env, clsOfcFileDescriptor) ;

  hFile = (OFC_HANDLE) (*env)->CallLongMethod (env, objFd, midGetHandle) ;
  return (hFile) ;
}

#if defined(__ANDROID__)
static JavaVM *g_jvm = OFC_NULL ;

jint JNI_OnLoad(JavaVM *jvm, void *reerved)
{
  g_jvm = jvm;
  return (JNI_VERSION_1_6);
}

void JNI_OnUnload(JavaVM *vm, void *reserved)
{
}

OFC_VOID ofc_attach_java_thread(OFC_VOID)
{
  int status;
#if defined(__APPLE__)
  void *envx ;
#else
  JNIEnv *envx ;
#endif

  ofc_assert(g_jvm != OFC_NULL, "No Java VM to attach to\n");

  status = (*g_jvm)->AttachCurrentThread (g_jvm, &envx, NULL);

  ofc_assert(status == JNI_OK, "Could not attach java thread\n");
}
  
OFC_VOID ofc_detach_java_thread(OFC_VOID)
{
  ofc_assert(g_jvm != OFC_NULL, "No Java VM to detach from\n");

  (*g_jvm)->DetachCurrentThread(g_jvm);
}
#endif
