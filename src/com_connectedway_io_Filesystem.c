/* Copyright (c) 2021 Connected Way, LLC. All rights reserved.
 * Use of this source code is governed by a Creative Commons 
 * Attribution-NoDerivatives 4.0 International license that can be
 * found in the LICENSE file.
 */
#define __OFC_CORE_DLL__

#include <jni.h>

#include "ofc/config.h"
#include "ofc/types.h"
#include "ofc/framework.h"
#include "ofc/persist.h"
#include "ofc/net.h"
#include "ofc/net_internal.h"
#include "ofc/heap.h"
#include "ofc/libc.h"
#include "ofc/path.h"
#include "ofc/time.h"
#include "ofc/queue.h"
#include "ofc/thread.h"
#include "ofc/file.h"
#include "ofc/fstype.h"
#include "ofc/handle.h"
#include "ofc/event.h"
#include "ofc/waitset.h"
#include "ofc/process.h"

#include "ofc_jni/com_connectedway_io_Utils.h"
#include "ofc_jni/com_connectedway_io_FileSystem.h"

#define OVERLAPPED_IO

/*
 * Class:     com_connectedway_io_FileSystem
 * Method:    isRemoteFile
 * Signature: (Ljava/lang/String;)Z
 */
JNIEXPORT jboolean JNICALL Java_com_connectedway_io_FileSystem_isRemoteFile
(JNIEnv *env, jclass clsFs, jstring jstrPathName)
{
  jboolean ret ;
  OFC_LPTSTR tstrPathName ;
  OFC_FST_TYPE fsType ;

  ret = JNI_FALSE ;
  tstrPathName = jstr2tchar (env, jstrPathName) ;

#if 0
  ofc_printf ("%s:%s:%d %S\n", __FILE__, __func__, __LINE__, tstrPathName) ;
#endif

  if (ofc_path_map_deviceW (tstrPathName) != OFC_NULL)
    {
      ret = JNI_TRUE ;
    }
  else
    {
      ofc_path_mapW (tstrPathName, OFC_NULL, &fsType) ;

      if (fsType == OFC_FST_SMB)
	ret = JNI_TRUE ;
    }

  ofc_free (tstrPathName) ;

  return (ret) ;
}

/*
 * Class:     com_connectedway_io_FileSystem
 * Method:    normalize
 * Signature: (Ljava/lang/String;)Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL Java_com_connectedway_io_FileSystem_normalize
(JNIEnv *env, jobject objFs, jstring jstrPathName) {

  jstring jstrNormal ;
  OFC_LPTSTR tstrPathName ;
  OFC_LPTSTR tstrNormalName ;
  OFC_LPTSTR tstrCursor ;
  OFC_PATH *path ;
  OFC_SIZET len ;
  OFC_SIZET rem ;

  tstrPathName = jstr2tchar (env, jstrPathName) ;
#if 0
  ofc_printf ("%s:%s:%d %S\n", __FILE__, __func__, __LINE__, tstrPathName) ;
#endif

  /*
   * Make the path absolute
   */
  path = ofc_path_createW (tstrPathName) ;

  rem = 0 ;
  len = ofc_path_printW (path, NULL, &rem) ;
  
  rem = len + 1 ;
  tstrNormalName = ofc_malloc (sizeof (OFC_TCHAR) * rem) ;

  tstrCursor = tstrNormalName ;
  ofc_path_printW (path, &tstrCursor, &rem) ;
  ofc_path_delete (path) ;

  if (ofc_tstrlen (tstrNormalName) == 0)
    {
      /*
       * No normalized representation, just use the path name
       */
      ofc_free (tstrNormalName) ;
      tstrNormalName = ofc_tstrdup (tstrPathName) ;
    }

  jstrNormal = tchar2jstr (env, tstrNormalName) ;
  ofc_free (tstrPathName) ;
  ofc_free (tstrNormalName) ;

  return (jstrNormal) ;
}

/*
 * Class:     com_connectedway_io_FileSystem
 * Method:    prefixLength
 * Signature: (Ljava/lang/String;)I
 */
JNIEXPORT jint JNICALL Java_com_connectedway_io_FileSystem_prefixLength
(JNIEnv *env, jobject objFs, jstring jstrPathName) {
  jint ret ;
  OFC_LPTSTR tstrPathName ;
  OFC_PATH *path ;

  /*
   * The prefix is the drive specifier (device), absolute specifier, or 
   * network specifier
   */

  ret = 0 ;
  tstrPathName = jstr2tchar (env, jstrPathName) ;

#if 0
  ofc_printf ("%s:%s:%d %S\n", __FILE__, __func__, __LINE__, tstrPathName) ;
#endif

  path = ofc_path_createW (tstrPathName) ;
  ofc_free (tstrPathName) ;

  if (ofc_path_device(path) != OFC_NULL)
    {
      /*
       * Plus 1 for the colon 
       */
      ret += ofc_tstrlen (ofc_path_device(path)) + 1 ;
      if (ofc_path_absolute(path))
	/* Plus 2 for double separators */
	ret += 2 ;
    }
  else if (ofc_path_remote(path))
    {
      /*
       * Plus 2 for the double separator ('//')
       */
      ret += 2 ;
    }
  else if (ofc_path_absolute(path))
    {
      ret ++ ;
    }

  ofc_path_delete (path) ;

  return (ret) ;
}

/*
 * Class:     com_connectedway_io_FileSystem
 * Method:    resolve
 * Signature: (Ljava/lang/String;Ljava/lang/String;)Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL 
Java_com_connectedway_io_FileSystem_resolve__Ljava_lang_String_2Ljava_lang_String_2
(JNIEnv *env, jobject objFs, jstring jstrParentName, jstring jstrChildName) {

  OFC_LPTSTR tstrParentName ;
  OFC_PATH *pathParent ;

  OFC_LPTSTR tstrChildName ;
  OFC_PATH *pathChild ;

  OFC_SIZET rem ;
  OFC_SIZET len ;

  OFC_LPTSTR tstrResolveName ;
  jstring jstrResolveName ;
  OFC_LPTSTR tstrCursor ;

  tstrParentName = jstr2tchar (env, jstrParentName) ;
  pathParent = ofc_path_createW (tstrParentName) ;

  tstrChildName = jstr2tchar (env, jstrChildName) ;
  pathChild = ofc_path_createW (tstrChildName) ;

#if 0
  ofc_printf ("%s:%s:%d %S %S\n", __FILE__, __func__, __LINE__, 
	       tstrParentName, tstrChildName) ;
#endif
  ofc_free (tstrParentName) ;
  ofc_free (tstrChildName) ;

  /*
   * map the parent ontop of the child.  This means that the parent's device
   * server, credentials, share and parent directies will be mapped (prepended
   * to) the child.
   */
  ofc_path_update (pathChild, pathParent) ;
  ofc_path_delete (pathParent) ;

  rem = 0 ;
  len = ofc_path_printW (pathChild, NULL, &rem) ;
  
  rem = len + 1 ;
  tstrResolveName = ofc_malloc (sizeof (OFC_TCHAR) * rem) ;
  tstrCursor = tstrResolveName ;
  ofc_path_printW (pathChild, &tstrCursor, &rem) ;
  ofc_path_delete (pathChild) ;

  jstrResolveName = tchar2jstr (env, tstrResolveName) ;
  ofc_free (tstrResolveName) ;

  return (jstrResolveName) ;
}

/*
 * Signature: (Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;
 * Ljava/lang/String)Z
 */
JNIEXPORT void JNICALL Java_com_connectedway_io_FileSystem_authenticate
(JNIEnv *env, jobject objFs, jstring jstrPath, jstring jstrUsername,
 jstring jstrWorkgroup, jstring jstrPassword) 
{
  OFC_LPTSTR tstrPath ;
  OFC_LPTSTR tstrUsername ;
  OFC_LPTSTR tstrWorkgroup ;
  OFC_LPTSTR tstrPassword ;

  tstrPath = jstr2tchar (env, jstrPath) ;
  tstrUsername = jstr2tchar (env, jstrUsername) ;
  tstrWorkgroup = jstr2tchar (env, jstrWorkgroup) ;
  tstrPassword = jstr2tchar (env, jstrPassword) ;

  ofc_path_update_credentialsW (tstrPath, tstrUsername, tstrPassword,
				tstrWorkgroup) ;

  ofc_free (tstrPath) ;
  ofc_free (tstrUsername) ;
  ofc_free (tstrWorkgroup) ;
  ofc_free (tstrPassword) ;

}

/*
 * Class:     com_connectedway_io_FileSystem
 * Method:    isAbsolute
 * Signature: (Lcom/connectedway/io/File;)Z
 */
JNIEXPORT jboolean JNICALL Java_com_connectedway_io_FileSystem_isAbsolute
(JNIEnv *env, jobject objFs, jobject objFile) {

  /*
   * Resolve is used to create a pathname relative to the current directory.
   * We do not support relative paths, so the resolved file simply the
   * abolute path
   */
  jclass clsFile ;
  jmethodID midGetPath ;

  jstring jstrAbsoluteName ;
  OFC_LPTSTR tstrAbsoluteName ;

  OFC_PATH *path ;
  jboolean ret ;

  clsFile = (*env)->FindClass (env, "com/connectedway/io/File") ;
  midGetPath = (*env)->GetMethodID(env, clsFile, "getPath",
				   "()Ljava/lang/String;") ;
  (*env)->DeleteLocalRef (env, clsFile) ;

  jstrAbsoluteName = (*env)->CallObjectMethod (env, objFile, midGetPath) ;

  tstrAbsoluteName = jstr2tchar (env, jstrAbsoluteName) ;
#if 0
  ofc_printf ("%s:%s:%d %S\n", __FILE__, __func__, __LINE__, 
	       tstrAbsoluteName) ;
#endif
  (*env)->DeleteLocalRef (env, jstrAbsoluteName) ;
  /*
   * Parse the path
   */
  path = ofc_path_createW (tstrAbsoluteName) ;
  ofc_free (tstrAbsoluteName) ;

  ret = JNI_FALSE ;
  if (ofc_path_absolute(path))
    ret = JNI_TRUE ;

  ofc_path_delete (path) ;

  return (ret) ;
}

/*
 * Class:     com_connectedway_io_FileSystem
 * Method:    resolve
 * Signature: (Lcom/connectedway/io/File;)Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL Java_com_connectedway_io_FileSystem_resolve__Lcom_connectedway_io_File_2
  (JNIEnv *env, jobject objFs, jobject objFile)
{
  /*
   * Resolve is used to create a pathname relative to the current directory.
   * We do not support relative paths, so the resolved file simply the
   * abolute path
   */

  jclass clsFile ;
  jmethodID midGetPath ;
  jstring jstrResolveName ;
  OFC_LPTSTR tstrResolveName ;
  OFC_LPTSTR tstrCursor ;

  OFC_PATH *path ;
  OFC_SIZET rem ;
  OFC_SIZET len ;

  clsFile = (*env)->FindClass (env, "com/connectedway/io/File") ;
  midGetPath = (*env)->GetMethodID(env, clsFile, "getPath",
				   "()Ljava/lang/String;") ;
  (*env)->DeleteLocalRef (env, clsFile) ;

  jstrResolveName = (*env)->CallObjectMethod (env, objFile, midGetPath) ;

  tstrResolveName = jstr2tchar (env, jstrResolveName) ;
#if 0
  ofc_printf ("%s:%s:%d %S\n", __FILE__, __func__, __LINE__, 
	       tstrResolveName) ;
#endif
  (*env)->DeleteLocalRef (env, jstrResolveName) ;

  /*
   * Make the path absolute
   */
  path = ofc_path_createW (tstrResolveName) ;
  ofc_free (tstrResolveName) ;

  rem = 0 ;
  len = ofc_path_printW (path, NULL, &rem) ;
  
  rem = len + 1 ;
  tstrResolveName = ofc_malloc (sizeof (OFC_TCHAR) * rem) ;

  tstrCursor = tstrResolveName ;
  ofc_path_printW (path, &tstrCursor, &rem) ;
  ofc_path_delete (path) ;

  jstrResolveName = tchar2jstr (env, tstrResolveName) ;
  ofc_free (tstrResolveName) ;

  return (jstrResolveName) ;
}

/*
 * Class:     com_connectedway_io_FileSystem
 * Method:    canonicalize
 * Signature: (Ljava/lang/String;)Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL Java_com_connectedway_io_FileSystem_canonicalize
(JNIEnv * env, jobject objFs, jstring jstrPathName) {

  OFC_LPTSTR tstrPathName ;

  OFC_LPTSTR tstrResolveName ;
  jstring jstrResolveName ;
  OFC_FST_TYPE fsType ;

  tstrPathName = jstr2tchar (env, jstrPathName) ;

#if 0
  ofc_printf ("%s:%s:%d %S\n", __FILE__, __func__, __LINE__, 
	       tstrPathName) ;
#endif
  ofc_path_mapW (tstrPathName, &tstrResolveName, &fsType) ;
  ofc_free (tstrPathName) ;

  jstrResolveName = tchar2jstr (env, tstrResolveName) ;
  ofc_free (tstrResolveName) ;

  return (jstrResolveName) ;
}

OFC_LPTSTR FileURIToString (JNIEnv *env, jobject objFile) {
  jclass clsFile ;
  jmethodID midToURI ;
  jobject objURI ;
  jclass clsURI ;
  jmethodID midToString ;
  jstring jstrPath ;
  OFC_LPTSTR tstrPath ;

  clsFile = (*env)->FindClass (env, "com/connectedway/io/File") ;
  midToURI = (*env)->GetMethodID(env, clsFile, "toURI",
				   "()Ljava/net/URI;") ;
  (*env)->DeleteLocalRef (env, clsFile) ;

  objURI = (*env)->CallObjectMethod (env, objFile, midToURI) ;

  clsURI = (*env)->FindClass (env, "java/net/URI") ;
  midToString = (*env)->GetMethodID(env, clsURI, "toString",
				    "()Ljava/lang/String;") ;
  (*env)->DeleteLocalRef (env, clsURI) ;

  jstrPath = (*env)->CallObjectMethod (env, objURI, midToString) ;

  (*env)->DeleteLocalRef (env, objURI) ;

  tstrPath = jstr2tchar (env, jstrPath) ;
  (*env)->DeleteLocalRef (env, jstrPath) ;

  return (tstrPath) ;
}

jint get_boolean_attributes (OFC_LPCTSTR tstrPath)
{
  jint booleanAttributes ;
  OFC_WIN32_FILE_ATTRIBUTE_DATA fadFile ;
  OFC_BOOL ret ;

  booleanAttributes = 0 ;

  ret = OfcGetFileAttributesExW (tstrPath, 
				  OfcGetFileExInfoStandard,
				  &fadFile) ;
  if (ret == OFC_TRUE)
    {
      booleanAttributes |= com_connectedway_io_FileSystem_BA_EXISTS ;

      if (fadFile.dwFileAttributes & OFC_FILE_ATTRIBUTE_DIRECTORY)
	booleanAttributes |= com_connectedway_io_FileSystem_BA_DIRECTORY ;
      else if (fadFile.dwFileAttributes & OFC_FILE_ATTRIBUTE_NORMAL ||
	       fadFile.dwFileAttributes & OFC_FILE_ATTRIBUTE_ARCHIVE)
	booleanAttributes |= com_connectedway_io_FileSystem_BA_REGULAR ;
      if (fadFile.dwFileAttributes & OFC_FILE_ATTRIBUTE_BOOKMARK)
	booleanAttributes |= com_connectedway_io_FileSystem_BA_BOOKMARK ;
      if (fadFile.dwFileAttributes & OFC_FILE_ATTRIBUTE_HIDDEN)
	booleanAttributes |= com_connectedway_io_FileSystem_BA_HIDDEN ;
      if (fadFile.dwFileAttributes & OFC_FILE_FLAG_SHARE)
	booleanAttributes |= com_connectedway_io_FileSystem_BA_SHARE ;
      if (fadFile.dwFileAttributes & OFC_FILE_FLAG_SERVER)
	booleanAttributes |= com_connectedway_io_FileSystem_BA_SERVER ;
      if (fadFile.dwFileAttributes & OFC_FILE_FLAG_WORKGROUP)
	booleanAttributes |= com_connectedway_io_FileSystem_BA_WORKGROUP ;
    }
  else
    {
      ofc_log (OFC_LOG_WARN,
	       "%s: OfcGetFileAttributesExW failed for %S, Last Error %d\n",
	       __func__, tstrPath, OfcGetLastError()) ;
    }

  return (booleanAttributes) ;
}

/*
 * Class:     com_connectedway_io_FileSystem
 * Method:    getBooleanAttributes
 * Signature: (Lcom/connectedway/io/File;)I
 */
JNIEXPORT jint JNICALL Java_com_connectedway_io_FileSystem_getBooleanAttributes
(JNIEnv *env, jobject objFs, jobject objFile) 
{
  jint booleanAttributes ;
  OFC_LPTSTR tstrPath ;

  ofc_thread_set_variable (OfcLastError, 
			 (OFC_DWORD_PTR) OFC_ERROR_SUCCESS) ;

  tstrPath = file_get_path (env, objFile) ;
#if 0
  ofc_printf ("%s:%s:%d %S\n", __FILE__, __func__, __LINE__, 
	       tstrPath) ;
#endif
  booleanAttributes = get_boolean_attributes (tstrPath) ;

  ofc_free (tstrPath) ;

  return (booleanAttributes) ;
}

/*
 * Class:     com_connectedway_io_FileSystem
 * Method:    checkAccess
 * Signature: (Lcom/connectedway/io/File;I)Z
 */
JNIEXPORT jboolean JNICALL Java_com_connectedway_io_FileSystem_checkAccess
(JNIEnv *env, jobject objFs, jobject objFile, jint access) 
{

  /**
   * Right now we don't support reading access writes in the CIFS client
   * This is a hole.  For now, return access is ok
   */
  return (JNI_TRUE) ;
}

/*
 * Class:     com_connectedway_io_FileSystem
 * Method:    setPermission
 * Signature: (Lcom/connectedway/io/File;IZZ)Z
 */
JNIEXPORT jboolean JNICALL Java_com_connectedway_io_FileSystem_setPermission
(JNIEnv *env , jobject objFs, jobject objFile, jint perm, 
 jboolean enabled, jboolean owner) 
{
  /*
   * We also don't support setting file permissions after a file has been
   * created.  This will have to be added
   */
  return (JNI_TRUE) ;
}


/*
 * Class:     com_connectedway_io_FileSystem
 * Method:    getLastModifiedTime
 * Signature: (Lcom/connectedway/io/File;)J
 */
JNIEXPORT jlong JNICALL Java_com_connectedway_io_FileSystem_getLastModifiedTime
(JNIEnv *env, jobject objFs, jobject objFile) 
{

  jlong modifiedTime ;
  OFC_LPTSTR tstrPath ;
  OFC_ULONG tv_sec ;
  OFC_ULONG tv_nsec ;

  OFC_WIN32_FILE_ATTRIBUTE_DATA fadFile ;

  tv_sec = 0 ;
  tv_nsec = 0 ;
  tstrPath = file_get_path (env, objFile) ;

#if 0
  ofc_printf ("%s:%s:%d %S\n", __FILE__, __func__, __LINE__, 
	       tstrPath) ;
#endif
  if (OfcGetFileAttributesExW (tstrPath, 
				OfcGetFileExInfoStandard,
				&fadFile) == OFC_TRUE)
    {
      file_time_to_epoch_time (&fadFile.ftLastWriteTime,
			   &tv_sec, &tv_nsec) ;
    }
  ofc_free (tstrPath) ;

  modifiedTime = ((jlong) tv_sec * 1000) + ((jlong) tv_nsec / (1000 * 1000)) ;

  return (modifiedTime) ;
}

/*
 * Class:     com_connectedway_io_FileSystem
 * Method:    getLength
 * Signature: (Lcom/connectedway/io/File;)J
 */
JNIEXPORT jlong JNICALL Java_com_connectedway_io_FileSystem_getLength
(JNIEnv *env, jobject objFs, jobject objFile) 
{

  OFC_LPTSTR tstrPath ;
  jlong size ;

  OFC_WIN32_FILE_ATTRIBUTE_DATA fadFile ;

  size = 0 ;
  tstrPath = file_get_path (env, objFile) ;

#if 0
  ofc_printf ("%s:%s:%d %S\n", __FILE__, __func__, __LINE__, 
	       tstrPath) ;
#endif
  if (OfcGetFileAttributesExW (tstrPath, 
				OfcGetFileExInfoStandard,
				&fadFile) == OFC_TRUE)
    {
      size = fadFile.nFileSizeHigh ;
      size = size << 32 ;
      size = size | fadFile.nFileSizeLow ;
    }
  ofc_free (tstrPath) ;

  return (size) ;
}

void throwio (JNIEnv *env)
{
  jclass newExcCls ;
  char code[10] ;

  ofc_snprintf (code, 10, "%d", OfcGetLastError()) ;
  newExcCls = (*env)->FindClass(env, "java/io/IOException");
  if (newExcCls != NULL)
    {
      (*env)->ThrowNew(env, newExcCls, code) ;
      (*env)->DeleteLocalRef (env, newExcCls) ;
    }
}

/*
 * Class:     com_connectedway_io_FileSystem
 * Method:    createFileExclusively
 * Signature: (Ljava/lang/String;)Z
 */
JNIEXPORT jboolean JNICALL 
Java_com_connectedway_io_FileSystem_createFileExclusively
(JNIEnv *env, jobject objFs, jstring strPath) 
{

  OFC_HANDLE hFile ;
  OFC_LPTSTR tstrPath ;
  jboolean ret ;

  ret = JNI_FALSE ;
  tstrPath = jstr2tchar (env, strPath) ;

#if 0
  ofc_printf ("%s:%s:%d %S\n", __FILE__, __func__, __LINE__, 
	       tstrPath) ;
#endif
  hFile = OfcCreateFileW (tstrPath, 
			   OFC_GENERIC_WRITE,
			   OFC_FILE_SHARE_NONE,
			   OFC_NULL,
			   OFC_CREATE_NEW,
			   OFC_FILE_ATTRIBUTE_NORMAL,
			   OFC_HANDLE_NULL) ;

  if (hFile != OFC_INVALID_HANDLE_VALUE)
    {
      OfcCloseHandle (hFile) ;
      ret = JNI_TRUE ;
    }
  else
    throwio (env) ;

  ofc_free (tstrPath) ;

  return (ret) ;
}

/*
 * Class:     com_connectedway_io_FileSystem
 * Method:    delete
 * Signature: (Lcom/connectedway/io/File;)Z
 */
JNIEXPORT jboolean JNICALL Java_com_connectedway_io_FileSystem_delete
(JNIEnv *env, jobject objFs, jobject objFile) 
{
  OFC_LPTSTR tstrPath ;
  OFC_BOOL retDelete ;
  OFC_DWORD dwLastError;
  jboolean ret ;
  jint booleanAttributes ;

  ret = JNI_FALSE ;

  tstrPath = file_get_path (env, objFile) ;
#if 0
  ofc_printf ("%s:%s:%d %S\n", __FILE__, __func__, __LINE__, 
	       tstrPath) ;
#endif
  booleanAttributes = get_boolean_attributes (tstrPath) ;

  if (booleanAttributes & com_connectedway_io_FileSystem_BA_DIRECTORY)
    {
      OFC_HANDLE dirHandle;
      retDelete = OfcRemoveDirectoryW (tstrPath) ;

      dirHandle = OfcCreateFile (tstrPath,
				 OFC_FILE_DELETE,
				 OFC_FILE_SHARE_DELETE,
				 OFC_NULL,
				 OFC_OPEN_EXISTING,
				 OFC_FILE_FLAG_DELETE_ON_CLOSE |
				 OFC_FILE_ATTRIBUTE_DIRECTORY,
				 OFC_HANDLE_NULL) ;
        
      if (dirHandle == OFC_INVALID_HANDLE_VALUE)
	{
	  ofc_log(OFC_LOG_WARN,
		  "Failed to create delete on close dir %A, "
		  "Error Code %d\n",
		  tstrPath,
		  OfcGetLastError ()) ;
	  retDelete = OFC_FALSE ;
	}
      else
	{
	  /* Close file
	   */
	  retDelete = OfcCloseHandle (dirHandle) ;
	  if (retDelete != OFC_TRUE)
	    {
	      dwLastError = OfcGetLastError () ;
	      ofc_log (OFC_LOG_WARN,
		       "Close of Delete on close dir "
		       "Failed with Error %d\n",
		       dwLastError) ;
	    }
	}
    }
  else
    retDelete = OfcDeleteFileW (tstrPath) ;

  ofc_free (tstrPath) ;

  if (retDelete == OFC_TRUE)
    ret = JNI_TRUE ;

  return (ret) ;
}

/*
 * Class:     com_connectedway_io_FileSystem
 * Method:    listFiles
 * Signature: (Lcom/Connectedway/io/File;)[Lcom/connectedway/io/File;
 */
JNIEXPORT jobjectArray JNICALL Java_com_connectedway_io_FileSystem_listFiles
(JNIEnv *env, jobject objFs, jobject objFile) 
{

  OFC_BOOL status ;
  OFC_HANDLE hList ;
  OFC_INT depth ;

  OFC_LPTSTR tstrPath ;

  jobjectArray jarrayFiles ;
  OFC_HANDLE list_handle ;

  OFC_WIN32_FIND_DATAW *find_data ;
  OFC_BOOL more ;

  jstring jstrFile ;
  OFC_INT i ;

  jclass clsOfcFile ;
  jobject objFile2 ;
  jint booleanAttributes ;

  jmethodID midSetAttributes ;

  clsOfcFile = (*env)->FindClass (env, "com/connectedway/io/File") ;

  status = OFC_FALSE ;
  hList = ofc_queue_create() ;
  depth = 0 ;

  ofc_thread_set_variable (OfcLastError, 
			 (OFC_DWORD_PTR) OFC_ERROR_SUCCESS) ;

  tstrPath = file_get_path (env, objFile) ;
#if 0
  ofc_printf ("%s:%s:%d %S\n", __FILE__, __func__, __LINE__, 
	       tstrPath) ;
#endif
  jarrayFiles = NULL ;
  jint attributes ;

  booleanAttributes = get_boolean_attributes (tstrPath) ;

  if (booleanAttributes & com_connectedway_io_FileSystem_BA_DIRECTORY)
    {
      OFC_SIZET len = ofc_tstrlen (tstrPath) ;
      
      if (len > 0 && tstrPath[len-1] != TCHAR_SLASH &&
	  tstrPath[len-1] != TCHAR_BACKSLASH)
	{
	  tstrPath = ofc_realloc (tstrPath, 
				      (len + 3) * sizeof (OFC_TCHAR)) ;
	  ofc_tstrcpy (tstrPath + ofc_tstrlen(tstrPath), TSTR("/*")) ;
	}
      else
	{
	  /*
	   * Reallocate bigger so we can add "*" and include EOS
	   */
	  tstrPath = ofc_realloc (tstrPath, 
				      (len + 2) * sizeof (OFC_TCHAR)) ;
	  ofc_tstrcpy (tstrPath + ofc_tstrlen(tstrPath), TSTR("*")) ;
	}
    }

  find_data = ofc_malloc (sizeof (OFC_WIN32_FIND_DATAW)) ;
  list_handle = OfcFindFirstFileW (tstrPath, find_data, &more) ;
  ofc_free (tstrPath) ;
        
  jarrayFiles = OFC_NULL ;

  if (list_handle == OFC_INVALID_HANDLE_VALUE)
    ofc_free (find_data) ;
  else
    {
      if (ofc_tstrcmp (find_data->cFileName, TSTR("..")) != 0 &&
	  ofc_tstrcmp (find_data->cFileName, TSTR(".")) != 0 &&
          !(find_data->dwFileAttributes & OFC_FILE_ATTRIBUTE_HIDDEN))
	{
	  ofc_enqueue (hList, find_data) ;
	  depth++ ;
	}
      else
	ofc_free (find_data) ;

      status = OFC_TRUE ;
      while (more && status == OFC_TRUE )
	{
	  find_data = ofc_malloc (sizeof (OFC_WIN32_FIND_DATAW)) ;
	  status = OfcFindNextFileW (list_handle,
				      find_data,
				      &more) ;
	  if (status == OFC_FALSE)
	    {
	      ofc_free (find_data) ;
	      if (OfcGetLastError () == OFC_ERROR_NO_MORE_FILES)
		{
		  status = OFC_TRUE ;
		  more = OFC_FALSE ;
		}
	    }
	  else
	    {
	      if (ofc_tstrcmp (find_data->cFileName, TSTR("..")) != 0 &&
		  ofc_tstrcmp (find_data->cFileName, TSTR(".")) != 0 &&
                  !(find_data->dwFileAttributes & OFC_FILE_ATTRIBUTE_HIDDEN))
		{
		  ofc_enqueue (hList, find_data) ;
		  depth++ ;
		}
	      else
		ofc_free (find_data) ;
	    }
	}
      OfcFindClose (list_handle) ;

      if (status == OFC_TRUE)
	{
	  jarrayFiles = (*env)->NewObjectArray (env, depth, clsOfcFile, NULL) ;
	  for (find_data = (OFC_WIN32_FIND_DATAW *) ofc_dequeue (hList), i = 0 ;
	       find_data != OFC_NULL ;
	       find_data = (OFC_WIN32_FIND_DATAW *) ofc_dequeue (hList), i++)
	    {
	      if (i < depth)
		{
		  jstrFile = tchar2jstr (env, find_data->cFileName) ;

		  if (find_data->dwFileAttributes & OFC_FILE_ATTRIBUTE_BOOKMARK)
		    {
		      objFile2 = new_file (env, jstrFile) ;
		    }
		  else
		    {
		      objFile2 = new_child_file (env, objFile, jstrFile) ;
		    }
		  (*env)->DeleteLocalRef (env, jstrFile) ;
		  /*
		   * Set attributes that we already have
		   */
		  midSetAttributes = (*env)->GetMethodID(env, clsOfcFile, 
							 "setAttributes",
							 "(I)V") ;
		  attributes = 0 ;

		  attributes |= com_connectedway_io_FileSystem_BA_EXISTS ;
		  if (find_data->dwFileAttributes & OFC_FILE_ATTRIBUTE_BOOKMARK)
		    attributes |= com_connectedway_io_FileSystem_BA_BOOKMARK ;
		  if (find_data->dwFileAttributes & OFC_FILE_ATTRIBUTE_DIRECTORY)
		    attributes |= com_connectedway_io_FileSystem_BA_DIRECTORY ;
		  if (find_data->dwFileAttributes & OFC_FILE_ATTRIBUTE_NORMAL ||
		      find_data->dwFileAttributes & OFC_FILE_ATTRIBUTE_ARCHIVE)
		    attributes |= com_connectedway_io_FileSystem_BA_REGULAR ;
		  if (find_data->dwFileAttributes & OFC_FILE_ATTRIBUTE_HIDDEN)
		    attributes |= com_connectedway_io_FileSystem_BA_HIDDEN ;
		  if (find_data->dwFileAttributes & OFC_FILE_FLAG_SHARE)
		    attributes |= com_connectedway_io_FileSystem_BA_SHARE ;
		  if (find_data->dwFileAttributes & OFC_FILE_FLAG_SERVER)
		    attributes |= com_connectedway_io_FileSystem_BA_SERVER ;
		  if (find_data->dwFileAttributes & OFC_FILE_FLAG_WORKGROUP)
		    attributes |= com_connectedway_io_FileSystem_BA_WORKGROUP ;

		  (*env)->CallVoidMethod (env, objFile2, midSetAttributes, 
					  attributes) ;

		  jmethodID midSetLength = (*env)->GetMethodID(env, clsOfcFile, 
							       "setLength",
							       "(J)V") ;
		  jlong size = ((jlong) find_data->nFileSizeHigh << 32) | (jlong) find_data->nFileSizeLow ;
		  (*env)->CallVoidMethod (env, objFile2, midSetLength, size) ;

#if 0
		  jmethodID midSetDate = (*env)->GetMethodID(env, clsOfcFile, 
							     "setDate",
							     "(J)V") ;
		  jlong date = ((jlong) find_data->ftLastWriteTime.dwHighDateTime << 32) | 
		    (jlong) find_data->ftLastWriteTime.dwLowDateTime ;

		  (*env)->CallVoidMethod (env, objFile2, midSetDate, date) ;
#endif
		  (*env)->SetObjectArrayElement (env, jarrayFiles, i, objFile2) ;
		  (*env)->DeleteLocalRef (env, objFile2) ;
		}
	      ofc_free (find_data) ;
	    }
	}
    }

  for (find_data = (OFC_WIN32_FIND_DATAW *) ofc_dequeue (hList) ;
       find_data != OFC_NULL ;
       find_data = (OFC_WIN32_FIND_DATAW *) ofc_dequeue (hList))
    ofc_free (find_data) ;

  ofc_queue_destroy (hList) ;

  (*env)->DeleteLocalRef (env, clsOfcFile) ;

  return (jarrayFiles) ;
}

/*
 * Class:     com_connectedway_io_FileSystem
 * Method:    list
 * Signature: (Lcom/connectedway/io/File;)[Ljava/lang/String;
 */
JNIEXPORT jobjectArray JNICALL Java_com_connectedway_io_FileSystem_list
(JNIEnv *env, jobject objFs, jobject objFile) 
{

  OFC_BOOL status ;
  OFC_HANDLE hList ;
  OFC_INT depth ;

  OFC_LPTSTR tstrPath ;

  jobjectArray jarrayStrings ;
  OFC_HANDLE list_handle ;

  OFC_WIN32_FIND_DATAW find_data ;
  OFC_BOOL more ;

  jstring jstrFile ;
  OFC_INT i ;

  jclass clsString ;
  jint booleanAttributes ;

  status = OFC_FALSE ;
  hList = ofc_queue_create() ;
  depth = 0 ;

  ofc_thread_set_variable (OfcLastError, 
			 (OFC_DWORD_PTR) OFC_ERROR_SUCCESS) ;

  tstrPath = file_get_path (env, objFile) ;
#if 0
  ofc_printf ("%s:%s:%d %S\n", __FILE__, __func__, __LINE__, 
	       tstrPath) ;
#endif
  jarrayStrings = NULL ;

  booleanAttributes = get_boolean_attributes (tstrPath) ;

  if (booleanAttributes & com_connectedway_io_FileSystem_BA_DIRECTORY)
    {
      OFC_SIZET len = ofc_tstrlen (tstrPath) ;
      
      if (len > 0 && tstrPath[len-1] != TCHAR_SLASH &&
	  tstrPath[len-1] != TCHAR_BACKSLASH)
	{
	  tstrPath = ofc_realloc (tstrPath, 
				      (len + 3) * sizeof (OFC_TCHAR)) ;
	  ofc_tstrcpy (tstrPath + ofc_tstrlen(tstrPath), TSTR("/*")) ;
	}
      else
	{
	  /*
	   * Reallocate bigger so we can add "*" and include EOS
	   */
	  tstrPath = ofc_realloc (tstrPath, 
				      (len + 2) * sizeof (OFC_TCHAR)) ;
	  ofc_tstrcpy (tstrPath + ofc_tstrlen(tstrPath), TSTR("*")) ;
	}
    }

  list_handle = OfcFindFirstFileW (tstrPath, &find_data, &more) ;
  ofc_free (tstrPath) ;
        
  if (list_handle != OFC_INVALID_HANDLE_VALUE)
    {
      if (!(find_data.dwFileAttributes & OFC_FILE_ATTRIBUTE_HIDDEN) &&
	  ofc_tstrcmp (find_data.cFileName, TSTR("..")) != 0 &&
	  ofc_tstrcmp (find_data.cFileName, TSTR(".")) != 0)
	{
	  ofc_enqueue (hList, 
			(OFC_VOID *) ofc_tstrdup (find_data.cFileName)) ;
	  depth++ ;
	}

      status = OFC_TRUE ;
      while (more && status == OFC_TRUE )
	{
	  status = OfcFindNextFileW (list_handle,
				      &find_data,
				      &more) ;
	  if (status == OFC_TRUE && 
	      !(find_data.dwFileAttributes & OFC_FILE_ATTRIBUTE_HIDDEN) &&
	      ofc_tstrcmp (find_data.cFileName, TSTR("..")) != 0 &&
	      ofc_tstrcmp (find_data.cFileName, TSTR(".")) != 0)
	    {
	      ofc_enqueue (hList, 
			    (OFC_VOID *) ofc_tstrdup (find_data.cFileName)) ;
	      depth++ ;
	    }
	  else if (status == OFC_FALSE && 
		   OfcGetLastError () == OFC_ERROR_NO_MORE_FILES)
	    {
	      status = OFC_TRUE ;
	      more = OFC_FALSE ;
	    }
	}
      OfcFindClose (list_handle) ;
    }

  clsString = (*env)->FindClass (env, "java/lang/String") ;
  jarrayStrings = (*env)->NewObjectArray (env, depth, clsString, NULL) ;
  (*env)->DeleteLocalRef (env, clsString) ;

  if (status == OFC_TRUE)
    {
      for (tstrPath = (OFC_LPTSTR) ofc_dequeue (hList), i = 0 ;
	   tstrPath != OFC_NULL ;
	   tstrPath = (OFC_LPTSTR) ofc_dequeue (hList), i++)
	{
	  jstrFile = tchar2jstr (env, tstrPath) ;

	  if (i < depth)
	    {
	      (*env)->SetObjectArrayElement (env, jarrayStrings, i, jstrFile) ;
	    }
	  ofc_free (tstrPath) ;
	  (*env)->DeleteLocalRef (env, jstrFile) ;
	}
    }

  for (tstrPath = (OFC_LPTSTR) ofc_dequeue (hList) ;
       tstrPath != OFC_NULL ;
       tstrPath = (OFC_LPTSTR) ofc_dequeue (hList))
    {
      ofc_free (tstrPath) ;
    }
  ofc_queue_destroy (hList) ;

  return (jarrayStrings) ;
}

/*
 * Class:     com_connectedway_io_FileSystem
 * Method:    createDirectory
 * Signature: (Lcom/connectedway/io/File;)Z
 */
JNIEXPORT jboolean JNICALL Java_com_connectedway_io_FileSystem_createDirectory
(JNIEnv *env, jobject objFs, jobject objFile) 
{

  OFC_LPTSTR tstrPath ;
  jboolean ret ;
  OFC_BOOL dirRet ;

  ret = JNI_FALSE ;
  tstrPath = file_get_path (env, objFile) ;
#if 0
  ofc_printf ("%s:%s:%d %S\n", __FILE__, __func__, __LINE__, 
	       tstrPath) ;
#endif
  dirRet = OfcCreateDirectoryW (tstrPath, OFC_NULL) ;
  ofc_free (tstrPath) ;

  ret = JNI_FALSE ;
  if (dirRet == OFC_TRUE)
      ret = JNI_TRUE ;

  return (ret) ;
}

/*
 * Class:     com_connectedway_io_FileSystem
 * Method:    rename
 * Signature: (Lcom/connectedway/io/File;Lcom/connectedway/io/File;)Z
 */
JNIEXPORT jboolean JNICALL Java_com_connectedway_io_FileSystem_rename
  (JNIEnv *env, jobject objFs, jobject objFrom, jobject objTo)
{

  OFC_LPTSTR tstrFrom ;
  OFC_LPTSTR tstrTo ;
  jboolean ret ;
  OFC_BOOL moveRet ;

  ret = JNI_FALSE ;
  tstrFrom = file_get_path (env, objFrom) ;
  tstrTo = file_get_path (env, objTo) ;
#if 0
  ofc_printf ("%s:%s:%d %S %S\n", __FILE__, __func__, __LINE__, 
	       tstrFrom, tstrTo) ;
#endif
  moveRet = OfcMoveFileW (tstrFrom, tstrTo) ;
  ofc_free (tstrFrom) ;
  ofc_free (tstrTo) ;

  ret = JNI_FALSE ;
  if (moveRet == OFC_TRUE)
      ret = JNI_TRUE ;

  return (ret) ;
}

/*
 * Class:     com_connectedway_io_FileSystem
 * Method:    setLastModifiedTime
 * Signature: (Lcom/connectedway/io/File;J)Z
 */
JNIEXPORT jboolean JNICALL 
Java_com_connectedway_io_FileSystem_setLastModifiedTime
(JNIEnv *env, jobject objFs, jobject objFile, jlong modifiedTime) 
{
  /*
   * This is not supported
   */
  return (JNI_FALSE) ;
}

/*
 * Class:     com_connectedway_io_FileSystem
 * Method:    setReadOnly
 * Signature: (Lcom/connectedway/io/File;)Z
 */
JNIEXPORT jboolean JNICALL Java_com_connectedway_io_FileSystem_setReadOnly
(JNIEnv *env, jobject objFs, jobject objFile) 
{

  /*
   * This isn't supported either??
   */
  return (JNI_FALSE) ;
}

/*
 * Class:     com_connectedway_io_FileSystem
 * Method:    listRoots
 * Signature: ()[Lcom/connectedway/io/File;
 */
JNIEXPORT jobjectArray JNICALL Java_com_connectedway_io_FileSystem_listRoots
(JNIEnv *env, jobject objFs) 
{

  jclass clsOfcFile ;
  jclass clsFileSystem ;

  jobjectArray jarrayFiles ;
  jmethodID midGetSeparator ;

  jchar sep ;
  char sepstr[3] ;

  jstring jstrFile ;
  jobject objFile ;

  clsOfcFile = (*env)->FindClass (env, "com/connectedway/io/File") ;
  jarrayFiles = (*env)->NewObjectArray (env, 1, clsOfcFile, NULL) ;
  (*env)->DeleteLocalRef (env, clsOfcFile) ;

  clsFileSystem = (*env)->GetObjectClass (env, objFs) ;
  midGetSeparator = (*env)->GetMethodID(env, clsFileSystem, 
					    "getSeparator",
					    "()C") ;
  (*env)->DeleteLocalRef (env, clsFileSystem) ;

  sep = (*env)->CallByteMethod(env, objFs, midGetSeparator) ;
#if 0
  sepstr[0] = (char) sep ;
  sepstr[1] = (char) sep ;
  sepstr[2] = '\0' ;

  jstrFile = (*env)->NewStringUTF (env, sepstr) ;
  objFile = new_file (env, jstrFile) ;
  (*env)->DeleteLocalRef (env, jstrFile) ;
  (*env)->SetObjectArrayElement (env, jarrayFiles, 0, objFile) ;
  (*env)->DeleteLocalRef (env, objFile) ;
#endif
  
  sepstr[0] = sep ;
  sepstr[1] = '\0' ;

  jstrFile = (*env)->NewStringUTF (env, sepstr) ;
  objFile = new_file (env, jstrFile) ;
  (*env)->DeleteLocalRef (env, jstrFile) ;
  (*env)->SetObjectArrayElement (env, jarrayFiles, 0, objFile) ;
  (*env)->DeleteLocalRef (env, objFile) ;

#if 0
  jstrFile = (*env)->NewStringUTF (env, "Bookmarks:") ;
  objFile = new_file (env, jstrFile) ;
  (*env)->DeleteLocalRef (env, jstrFile) ;
  (*env)->SetObjectArrayElement (env, jarrayFiles, 2, objFile) ;
  (*env)->DeleteLocalRef (env, objFile) ;
#endif

  return (jarrayFiles) ;
}


/*
 * Class:     com_connectedway_io_FileSystem
 * Method:    getSpace
 * Signature: (Lcom/connectedway/io/File;I)J
 */
JNIEXPORT jlong JNICALL Java_com_connectedway_io_FileSystem_getSpace
  (JNIEnv *env, jobject objFs, jobject objFile, jint modifier)
{

  OFC_LPTSTR tstrPath ;
  jlong usage ;
  OFC_BOOL ret ;
  
  OFC_DWORD sectorsPerCluster ;
  OFC_DWORD bytesPerSector ;
  OFC_DWORD numberOfFreeClusters ;
  OFC_DWORD totalNumberOfClusters ; 

  usage = 0 ;
  tstrPath = file_get_path (env, objFile) ;
#if 0
  ofc_printf ("%s:%s:%d %S\n", __FILE__, __func__, __LINE__, 
	       tstrPath) ;
#endif
  ret = OfcGetDiskFreeSpaceW (tstrPath,
			       &sectorsPerCluster,
			       &bytesPerSector,
			       &numberOfFreeClusters,
			       &totalNumberOfClusters) ; 

  ofc_free (tstrPath) ;

  if (ret == OFC_TRUE)
    {
      if (modifier == com_connectedway_io_FileSystem_SPACE_TOTAL)
	{
	  usage = (jlong) totalNumberOfClusters * sectorsPerCluster * 
	    bytesPerSector ;
	}
      else if (modifier == com_connectedway_io_FileSystem_SPACE_FREE)
	{
	  usage = (jlong) numberOfFreeClusters * sectorsPerCluster * 
	    bytesPerSector ;
	}
      else if (modifier == com_connectedway_io_FileSystem_SPACE_USABLE)
	{
	  /*
	   * We don't have a notion of usable.  We'll just use free
	   */
	  usage = (jlong) numberOfFreeClusters * sectorsPerCluster * 
	    bytesPerSector ;
	}
      else
	usage = 0 ;
    }

  return (usage) ;
}

/*
 * Class:     com_connectedway_io_FileSystem
 * Method:    open
 * Signature: (Ljava/lang/String;I)Lcom/connectedway/io/FileDescriptor;
 */
JNIEXPORT jobject JNICALL Java_com_connectedway_io_FileSystem_open
(JNIEnv *env, jobject objFs, jstring jstrPathName, jint iMode) 
{
  jobject objFd ;

  OFC_LPTSTR tstrPathName ;
  OFC_HANDLE hFile ;
  OFC_DWORD dwAccess ;
  OFC_DWORD dwShare ;
  OFC_DWORD dwCreate ;
  OFC_DWORD dwLastError ;

  tstrPathName = jstr2tchar (env, jstrPathName) ;
#if 0
  ofc_printf ("%s:%s:%d %S %S\n", __FILE__, __func__, __LINE__, 
	       tstrPathName) ;
#endif
  dwAccess = OFC_GENERIC_READ ;
  dwShare = OFC_FILE_SHARE_READ | OFC_FILE_SHARE_WRITE ;
  dwCreate = OFC_OPEN_EXISTING ;
  if (iMode == com_connectedway_io_FileSystem_OPEN_READ)
    {
      dwAccess = OFC_GENERIC_READ ;
      dwShare = OFC_FILE_SHARE_READ | OFC_FILE_SHARE_WRITE ;
      dwCreate = OFC_OPEN_EXISTING ;
    }
  else if (iMode == com_connectedway_io_FileSystem_OPEN_WRITE)
    {
      dwAccess = OFC_GENERIC_WRITE ;
      dwShare = OFC_FILE_SHARE_READ | OFC_FILE_SHARE_WRITE ;
      dwCreate = OFC_CREATE_ALWAYS ;
    }
  else if (iMode == com_connectedway_io_FileSystem_OPEN_APPEND)
    {
      dwAccess = OFC_GENERIC_WRITE ;
      dwShare = OFC_FILE_SHARE_READ | OFC_FILE_SHARE_WRITE ;
      dwCreate = OFC_OPEN_ALWAYS ;
    }
  else if (iMode == com_connectedway_io_FileSystem_OPEN_RW)
    {
      dwAccess = OFC_GENERIC_READ | OFC_GENERIC_WRITE ;
      dwShare = OFC_FILE_SHARE_READ | OFC_FILE_SHARE_WRITE ;
      dwCreate = OFC_OPEN_ALWAYS ;
    }

  hFile = OfcCreateFileW (tstrPathName, dwAccess, dwShare,
			   OFC_NULL, dwCreate,
			   OFC_FILE_ATTRIBUTE_NORMAL, OFC_HANDLE_NULL) ;
  
  if (hFile == OFC_INVALID_HANDLE_VALUE)
    {
      jclass newExcCls ;
      objFd = NULL ;

      dwLastError = OfcGetLastError() ;
      if (dwLastError == OFC_ERROR_ACCESS_DENIED ||
	  dwLastError == OFC_ERROR_INVALID_PASSWORD)
	newExcCls = (*env)->FindClass(env, "java/lang/SecurityException");
      else
	{
	  newExcCls = (*env)->FindClass(env, "java/io/FileNotFoundException");
	}
      if (newExcCls != NULL)
	{
	  (*env)->ThrowNew(env, newExcCls, "Cannot open file") ;
	  (*env)->DeleteLocalRef (env, newExcCls) ;
	}
    }
  else
    {
      objFd = new_fd (env, hFile) ;
    }

  ofc_free (tstrPathName) ;
  return (objFd) ;
}

/*
 * Class:     com_connectedway_io_FileSystem
 * Method:    read
 * Signature: (Lcom/connectedway/io/FileDescriptor;)I
 */
JNIEXPORT jint JNICALL Java_com_connectedway_io_FileSystem_read__Lcom_connectedway_io_FileDescriptor_2
  (JNIEnv *env, jobject objFs, jobject objFd)
{
  OFC_HANDLE hFile ;
  jint jiByte ;
  OFC_DWORD nRead ;
  /*
   * Read a byte
   */
#if 0
  ofc_printf ("%s:%s:%d\n", __FILE__, __func__, __LINE__) ;
#endif
  hFile = file_descriptor_get_handle (env, objFd) ;

  if (OfcReadFile (hFile, &jiByte, 1, &nRead, OFC_HANDLE_NULL) == 
      OFC_FALSE)
    {
      jiByte = -1 ;
      if (OfcGetLastError() != OFC_ERROR_HANDLE_EOF)
	throwio(env) ;
    }

  return (jiByte) ;
}

/*
 * Class:     com_connectedway_io_FileSystem
 * Method:    read
 * Signature: (Lcom/connectedway/io/FileDescriptor;[B)I
 */
JNIEXPORT jint JNICALL Java_com_connectedway_io_FileSystem_read__Lcom_connectedway_io_FileDescriptor_2_3B
(JNIEnv *env, jobject objFs, jobject objFd, jbyteArray arrayB) {
  OFC_HANDLE hFile ;
  jint jiBytesRead ;
  OFC_DWORD nRead ;
  jbyte *jbBuffer ;
  OFC_CHAR *lpcBuffer ;
  OFC_SIZET bsizeBuffer ;
  jint jiWorkingOffset ;
  OFC_BOOL eof ;
  jint jiLen;

#if 0
  ofc_printf ("%s:%s:%d\n", __FILE__, __func__, __LINE__) ;
#endif
  hFile = file_descriptor_get_handle (env, objFd) ;
  jbBuffer = (*env)->GetByteArrayElements (env, arrayB, NULL) ;

  jiWorkingOffset = 0 ;
  jiBytesRead = 0 ;
  jiLen = (*env)->GetArrayLength(env, arrayB);

  for (eof = OFC_FALSE ; !eof && jiLen > 0 ; )
    {
      lpcBuffer = (OFC_CHAR *) jbBuffer + jiWorkingOffset ;
      bsizeBuffer = OFC_MIN(jiLen, OFC_MAX_IO) ;

      if (OfcReadFile (hFile, lpcBuffer, (OFC_DWORD) bsizeBuffer, &nRead, 
			OFC_HANDLE_NULL) == OFC_FALSE)

	{
	  nRead = 0 ;
	  eof = 1 ;
	  if (OfcGetLastError() != OFC_ERROR_HANDLE_EOF) 
	    throwio(env) ;
	}
      jiBytesRead += nRead ;
      jiWorkingOffset += nRead ;
      jiLen -= nRead ;
    }

  (*env)->ReleaseByteArrayElements (env,arrayB, jbBuffer, 0) ;

  if (jiBytesRead == 0)
    jiBytesRead = -1 ;

  return (jiBytesRead) ;
}

/*
 * Class:     com_connectedway_io_FileSystem
 * Method:    read
 * Signature: (Lcom/connectedway/io/FileDescriptor;[BII)I
 */
#if !defined(OVERLAPPED_IO)
JNIEXPORT jint JNICALL Java_com_connectedway_io_FileSystem_read__Lcom_connectedway_io_FileDescriptor_2_3BII
  (JNIEnv *env, jobject objFs, jobject objFd, jbyteArray arrayB, 
   jint jiOffset, jint jiLen) {

  OFC_HANDLE hFile ;
  jint jiBytesRead ;
  OFC_DWORD nRead ;
  jbyte *jbBuffer ;
  OFC_CHAR *lpcBuffer ;
  OFC_SIZET bsizeBuffer ;
  jint jiWorkingOffset ;
  OFC_BOOL eof ;

#if 0
  ofc_printf ("%s:%s:%d\n", __FILE__, __func__, __LINE__) ;
#endif
  hFile = file_descriptor_get_handle (env, objFd) ;
  jbBuffer = (*env)->GetByteArrayElements (env, arrayB, NULL) ;

  jiWorkingOffset = jiOffset ;
  jiBytesRead = 0 ;

  for (eof = OFC_FALSE ; !eof && jiLen > 0 ; )
    {
      lpcBuffer = (OFC_CHAR *) jbBuffer + jiWorkingOffset ;
      bsizeBuffer = OFC_MIN(jiLen, OFC_MAX_IO) ;

      if (OfcReadFile (hFile, lpcBuffer, (OFC_DWORD) bsizeBuffer, &nRead, 
			OFC_HANDLE_NULL) == OFC_FALSE)

	{
	  nRead = 0 ;
	  eof = 1 ;
	  if (OfcGetLastError() != OFC_ERROR_HANDLE_EOF) 
	    throwio(env) ;
	}
      jiBytesRead += nRead ;
      jiWorkingOffset += nRead ;
      jiLen -= nRead ;
    }

  (*env)->ReleaseByteArrayElements (env,arrayB, jbBuffer, 0) ;

  if (jiBytesRead == 0)
    jiBytesRead = -1 ;

  return (jiBytesRead) ;
}
#else
/*
 * Buffering definitions.  We test using overlapped asynchronous I/O.  This
 * implies multi-buffering
 *
 * The Buffer Size
 */
#define BUFFER_SIZE OFC_MAX_IO
/*
 * And the number of buffers
 */
#define NUM_FILE_BUFFERS 10
/*
 * Define buffer states.
 */
typedef enum {
  BUFFER_STATE_IDLE,        /* There is no I/O active */
  BUFFER_STATE_READ,        /* Data is being read into the buffer */
  BUFFER_STATE_WRITE        /* Data is being written from the buffer */
} BUFFER_STATE;
/*
 * This buffering stuff should be part of a utility library
 */

/*
 * The buffer context
 *
 * Currently, handles to the overlapped i/o context may be platform
 * dependent.  Because of this, an overlapped i/o may not be shared
 * between files unless it is guaranteed that the files are on the
 * same device (using the same type of overlapped context).
 *
 * Ideally, overlapped I/Os should be platform independent. This will
 * require changes to the way overlapped handles are managed.
 */
typedef struct {
  OFC_HANDLE readOverlapped;    /* The handle to the buffer when reading */
  OFC_HANDLE writeOverlapped;    /* The handle to the buffer when writing */
  OFC_CHAR *data;        /* Pointer to the buffer */
  BUFFER_STATE state;        /* Buffer state */
  OFC_LARGE_INTEGER offset;    /* Offset in file for I/O */
} OFC_FILE_BUFFER;
/*
 * Result of Async I/O
 *
 * This essentially is a OFC_BOOL with the addition of a PENDING flag
 */
typedef enum {
  ASYNC_RESULT_DONE,        /* I/O is successful */
  ASYNC_RESULT_ERROR,        /* I/O was in error */
  ASYNC_RESULT_EOF,        /* I/O hit EOF */
  ASYNC_RESULT_PENDING    /* I/O is still pending */
} ASYNC_RESULT;

/*
 * Perform an I/O Read
 *
 * \param wait_set
 * The wait set that this I/O and it's overlapped handles will be part of
 *
 * \param read_file
 * Handle of read file
 *
 * \param buffer
 * Pointer to buffer to read into
 *
 * \param dwLen
 * Length of buffer to read
 *
 * \returns
 * OFC_TRUE if success, OFC_FALSE otherwise
 */
static ASYNC_RESULT
AsyncRead(OFC_HANDLE wait_set, OFC_HANDLE read_file,
          OFC_FILE_BUFFER *buffer, OFC_DWORD dwLen)
{
  ASYNC_RESULT result;
  OFC_BOOL status;

  /*
   * initialize the read buffer using the read file, the read overlapped
   * handle and the current read offset
   */
  ofc_trace ("Reading 0x%08x\n", OFC_LARGE_INTEGER_LOW(buffer->offset));
  OfcSetOverlappedOffset(read_file, buffer->readOverlapped, buffer->offset);
  /*
   * Set the state to reading
   */
  buffer->state = BUFFER_STATE_READ;
  /*
   * Add the buffer to the wait set
   */
  ofc_waitset_add(wait_set, (OFC_HANDLE) buffer, buffer->readOverlapped);
  /*
   * Issue the read (this will be non blocking)
   */
  status = OfcReadFile(read_file, buffer->data, dwLen,
                       OFC_NULL, buffer->readOverlapped);
  /*
   * If it completed, the status will be OFC_TRUE.  We actually expect
   * the status to fail and the last error to be OFC_ERROR_IO_PENDING
   */
  if (status == OFC_TRUE)
    {
      if (*((OFC_ULONG *)(buffer->data)) != buffer->offset)
        ofc_log(OFC_LOG_WARN,
		"got bad buffer in async read 0x%08x, 0x%08x\n",
		*((OFC_ULONG *)(buffer->data)), buffer->offset);
      result = ASYNC_RESULT_DONE;
    }
  else
    {
      OFC_DWORD dwLastError;
      /*
       * Let's check the last error
       */
      dwLastError = OfcGetLastError();
      if (dwLastError == OFC_ERROR_IO_PENDING)
        {
          /*
           * This is what we expect, so say the I/O submission succeeded
           */
          result = ASYNC_RESULT_PENDING;
        }
      else
        {
          if (dwLastError == OFC_ERROR_HANDLE_EOF)
            result = ASYNC_RESULT_EOF;
          else
            result = ASYNC_RESULT_ERROR;
          /*
           * It's not pending
           */
          buffer->state = BUFFER_STATE_IDLE;
          ofc_waitset_remove(wait_set, buffer->readOverlapped);
        }
    }

  return (result);
}

/*
 * Return the state of the read
 *
 * \param wait_set
 * Wait set that the I/O should be part of
 *
 * \param read_file
 * Handle to the read file
 *
 * \param buffer
 * Pointer to the buffer
 *
 * \param dwLen
 * Number of bytes to read / number of bytes read
 *
 * \returns
 * state of the read
 */
static ASYNC_RESULT AsyncReadResult(OFC_HANDLE wait_set,
                                    OFC_HANDLE read_file,
                                    OFC_FILE_BUFFER *buffer,
                                    OFC_DWORD *dwLen)
{
  ASYNC_RESULT result;
  OFC_BOOL status;

  /*
   * Get the overlapped result
   */
  status = OfcGetOverlappedResult(read_file, buffer->readOverlapped,
                                  dwLen, OFC_FALSE);
  /*
   * If the I/O is complete, status will be true and length will be non zero
   */
  if (status == OFC_TRUE)
    {
      if (*dwLen == 0)
        {
          result = ASYNC_RESULT_EOF;
        }
      else
        {
          result = ASYNC_RESULT_DONE;
        }
    }
  else
    {
      OFC_DWORD dwLastError;
      /*
       * I/O may still be pending
       */
      dwLastError = OfcGetLastError();
      if (dwLastError == OFC_ERROR_IO_PENDING)
        result = ASYNC_RESULT_PENDING;
      else
        {
          /*
           * I/O may also be EOF
           */
          if (dwLastError != OFC_ERROR_HANDLE_EOF)
            {
              ofc_log(OFC_LOG_WARN, "Read Error %d\n", dwLastError);
              result = ASYNC_RESULT_ERROR;
            }
          else
            result = ASYNC_RESULT_EOF;
        }
    }

  if (result != ASYNC_RESULT_PENDING)
    {
      /*
       * Finish up the buffer if the I/O is no longer pending
       */
      buffer->state = BUFFER_STATE_IDLE;
      ofc_waitset_remove(wait_set, buffer->readOverlapped);
    }

  return (result);
}

/*
 * Submit an asynchronous Write
 */
static ASYNC_RESULT AsyncWrite(OFC_HANDLE wait_set, OFC_HANDLE write_file,
                               OFC_FILE_BUFFER *buffer, OFC_DWORD dwLen)
{
  OFC_BOOL status;
  ASYNC_RESULT result;

  ofc_trace ("Writing 0x%08x\n", OFC_LARGE_INTEGER_LOW(buffer->offset));
  OfcSetOverlappedOffset(write_file, buffer->writeOverlapped,
                         buffer->offset);

  buffer->state = BUFFER_STATE_WRITE;
  ofc_waitset_add(wait_set, (OFC_HANDLE) buffer, buffer->writeOverlapped);

  status = OfcWriteFile(write_file, buffer->data, dwLen, OFC_NULL,
                        buffer->writeOverlapped);

  result = ASYNC_RESULT_DONE;
  if (status != OFC_TRUE)
    {
      OFC_DWORD dwLastError;

      dwLastError = OfcGetLastError();
      if (dwLastError == OFC_ERROR_IO_PENDING)
        result = ASYNC_RESULT_PENDING;
      else
        {
          result = ASYNC_RESULT_ERROR;
          buffer->state = BUFFER_STATE_IDLE;
          ofc_waitset_remove(wait_set, buffer->writeOverlapped);
        }
    }
  return (result);
}

static ASYNC_RESULT AsyncWriteResult(OFC_HANDLE wait_set,
                                     OFC_HANDLE write_file,
                                     OFC_FILE_BUFFER *buffer,
                                     OFC_DWORD *dwLen)
{
  ASYNC_RESULT result;
  OFC_BOOL status;

  status = OfcGetOverlappedResult(write_file, buffer->writeOverlapped,
                                  dwLen, OFC_FALSE);
  if (status == OFC_TRUE)
    result = ASYNC_RESULT_DONE;
  else
    {
      OFC_DWORD dwLastError;

      dwLastError = OfcGetLastError();
      if (dwLastError == OFC_ERROR_IO_PENDING)
        result = ASYNC_RESULT_PENDING;
      else
        {
          ofc_log(OFC_LOG_WARN, "Write Error %d\n", dwLastError);
          result = ASYNC_RESULT_ERROR;
        }
    }

  if (result != ASYNC_RESULT_PENDING)
    {
      buffer->state = BUFFER_STATE_IDLE;
      ofc_waitset_remove(wait_set, buffer->writeOverlapped);
    }

  return (result);
}
 
JNIEXPORT jint JNICALL Java_com_connectedway_io_FileSystem_read__Lcom_connectedway_io_FileDescriptor_2_3BII
  (JNIEnv *env, jobject objFs, jobject objFd, jbyteArray arrayB, 
   jint jiOffset, jint jiLen) {

  OFC_HANDLE hFile ;
  jint jiBytesRead ;
  jbyte *jbBuffer ;
  OFC_BOOL eof ;
  OFC_LARGE_INTEGER file_offset;
  OFC_OFFT buffer_offset;
  OFC_INT pending;
  OFC_HANDLE buffer_list;
  OFC_FILE_BUFFER *buffer;
  OFC_INT i;
  OFC_HANDLE wait_set;
  OFC_DWORD dwLen;
  ASYNC_RESULT result;
  OFC_HANDLE hEvent;
  
#if 0
  ofc_printf ("%s:%s:%d\n", __FILE__, __func__, __LINE__) ;
#endif
  hFile = file_descriptor_get_handle (env, objFd) ;
  jbBuffer = (*env)->GetByteArrayElements (env, arrayB, NULL) ;

  jiBytesRead = 0 ;

  wait_set = ofc_waitset_create();
  buffer_list = ofc_queue_create();

  file_offset = jiOffset ;
  buffer_offset = 0;
  eof = OFC_FALSE;
  pending = 0;
  
  for (i = 0; i < NUM_FILE_BUFFERS && !eof && buffer_offset < jiLen; i++)
    {
      /*
       * Get the buffer descriptor and the data buffer
       */
      buffer = ofc_malloc(sizeof(OFC_FILE_BUFFER));
      if (buffer == OFC_NULL)
        {
          ofc_log(OFC_LOG_WARN, "test_file: Failed to alloc buffer context\n");
          eof = OFC_TRUE;
        }
      else
        {
          buffer->data = (OFC_CHAR *) jbBuffer + buffer_offset ;
          buffer->offset = file_offset;
          buffer->readOverlapped = OfcCreateOverlapped(hFile);
          if (buffer->readOverlapped == OFC_HANDLE_NULL)
            ofc_process_crash("An Overlapped Handle is NULL");

          /*
           * Add it to our buffer list
           */
          ofc_enqueue(buffer_list, buffer);

          pending++;
          dwLen = OFC_MIN(BUFFER_SIZE, jiLen - buffer_offset);
          result = AsyncRead(wait_set, hFile, buffer, dwLen);
          if (result != ASYNC_RESULT_PENDING)
            {
              /*
               * discount pending and set eof
               */
              pending--;
              /*
               * Set eof either because it really is eof, or we
               * want to clean up.
               */
              eof = OFC_TRUE;
            }
          /*
           * Prepare for the next buffer
           */
          buffer_offset += dwLen;
          file_offset += dwLen;
        }
    }

  /*
   * Now all our buffers should be busy doing reads.  Keep pumping
   * more data to read and service writes
   */
  while (pending > 0)
    {
      /*
       * Wait for some buffer to finish (may be a read if we've
       * just finished priming, but it may be a write also if
       * we've been in this loop a bit
       */
      hEvent = ofc_waitset_wait(wait_set);
      if (hEvent != OFC_HANDLE_NULL)
        {
          /*
           * We use the app of the event as a pointer to the
           * buffer descriptor.  Yeah, this isn't really nice but
           * the alternative is to add a context to each handle.
           * That may be cleaner, but basically unnecessary.  If
           * we did this kind of thing a lot, I'm all for a
           * new property of a handle
           */
          buffer = (OFC_FILE_BUFFER *) ofc_handle_get_app(hEvent);

          if (buffer->state == BUFFER_STATE_READ)
            {
              /*
               * Read, so let's see the result of the read
               */
              result = AsyncReadResult(wait_set, hFile,
                                       buffer, &dwLen);
              if (result == ASYNC_RESULT_DONE)
                {
                  jiBytesRead += dwLen ;

                  dwLen = OFC_MIN(BUFFER_SIZE, jiLen - buffer_offset);
                  if (dwLen > 0)
                    {
                      buffer->data = (OFC_CHAR *) jbBuffer + buffer_offset ;
                      buffer->offset = file_offset;
                      /*
                       * And start a read on the next chunk
                       */
                      result = AsyncRead(wait_set, hFile,
                                         buffer, dwLen);
                      buffer_offset += dwLen;
                      file_offset += dwLen;
                    }
                }

              if (result != ASYNC_RESULT_PENDING)
                {
                  pending--;
                  eof = OFC_TRUE;
                }
            }
        }
    }

  /*
   * The pending count is zero so we've gotten completions
   * either due to errors or eof on all of our outstanding
   * reads and writes.
   */
  for (buffer = ofc_dequeue(buffer_list);
       buffer != OFC_NULL;
       buffer = ofc_dequeue(buffer_list))
    {
      /*
       * Destroy the overlapped I/O handle for each buffer
       */
      OfcDestroyOverlapped(hFile, buffer->readOverlapped);
      /*
       * Free the buffer descriptor
       */
      ofc_free(buffer);
    }
  /*
   * Destroy the buffer list
   */
  ofc_queue_destroy(buffer_list);
  /*
   * And destroy the wait list
   */
  ofc_waitset_destroy(wait_set);
  
  (*env)->ReleaseByteArrayElements (env,arrayB, jbBuffer, 0) ;

  if (jiBytesRead == 0)
    jiBytesRead = -1 ;

  return (jiBytesRead) ;
}
#endif

/*
 * Class:     com_connectedway_io_FileSystem
 * Method:    write
 * Signature: (Lcom/connectedway/io/FileDescriptor;I)V
 */
JNIEXPORT void JNICALL Java_com_connectedway_io_FileSystem_write__Lcom_connectedway_io_FileDescriptor_2I
(JNIEnv *env, jobject objFs, jobject objFd, jint iByte) {
  OFC_HANDLE hFile ;
  OFC_DWORD nWritten ;
  /*
   * Read a byte
   */
#if 0
  ofc_printf ("%s:%s:%d\n", __FILE__, __func__, __LINE__) ;
#endif
  hFile = file_descriptor_get_handle (env,objFd) ;

  if (OfcWriteFile (hFile, &iByte, 1, &nWritten, OFC_HANDLE_NULL) == 
      OFC_FALSE)
    {
      throwio(env) ;
    }

}

/*
 * Class:     com_connectedway_io_FileSystem
 * Method:    write
 * Signature: (Lcom/connectedway/io/FileDescriptor;[B)I
 */
JNIEXPORT void JNICALL Java_com_connectedway_io_FileSystem_write__Lcom_connectedway_io_FileDescriptor_2_3B
(JNIEnv *env, jobject objFs, jobject objFd, jbyteArray arrayB) {
  OFC_HANDLE hFile ;
  OFC_DWORD nWritten ;
  jbyte *jbBuffer ;
  OFC_SIZET bsizeBuffer ;
  OFC_CHAR *lpcBuffer ;
  jint jiWorkingOffset ;
  jint jiBytesWritten ;
  OFC_BOOL eof ;
  jint jiLen;

#if 0
  ofc_printf ("%s:%s:%d\n", __FILE__, __func__, __LINE__) ;
#endif
  hFile = file_descriptor_get_handle (env, objFd) ;
  jbBuffer = (*env)->GetByteArrayElements (env, arrayB, NULL) ;

  jiWorkingOffset = 0 ;
  jiBytesWritten = 0 ;
  jiLen = (*env)->GetArrayLength(env, arrayB);

  for (eof = OFC_FALSE ; !eof && jiLen > 0 ; )
    {
      lpcBuffer = (OFC_CHAR *) jbBuffer + jiWorkingOffset ;
      bsizeBuffer = OFC_MIN(jiLen, OFC_MAX_IO) ;

      if (OfcWriteFile (hFile, lpcBuffer, (OFC_DWORD) bsizeBuffer, &nWritten, 
			 OFC_HANDLE_NULL) == OFC_FALSE)
	{
	  nWritten = 0 ;
	  eof = 1 ;

	  throwio(env) ;
	}
      jiBytesWritten += nWritten ;
      jiWorkingOffset += nWritten ;
      jiLen -= nWritten ;
    }
  (*env)->ReleaseByteArrayElements (env, arrayB, jbBuffer, 0) ;
}

/*
 * Class:     com_connectedway_io_FileSystem
 * Method:    write
 * Signature: (Lcom/connectedway/io/FileDescriptor;[BII)V
 */
#if !defined(OVERLAPPED_IO)
JNIEXPORT void JNICALL Java_com_connectedway_io_FileSystem_write__Lcom_connectedway_io_FileDescriptor_2_3BII
(JNIEnv *env, jobject objFs, jobject objFd, jbyteArray jarrayByte, 
 jint jiOffset, jint jiLen) {
  OFC_HANDLE hFile ;
  OFC_DWORD nWritten ;
  jbyte *jbBuffer ;
  OFC_SIZET bsizeBuffer ;
  OFC_CHAR *lpcBuffer ;
  jint jiWorkingOffset ;
  jint jiBytesWritten ;
  OFC_BOOL eof ;

#if 0
  ofc_printf ("%s:%s:%d\n", __FILE__, __func__, __LINE__) ;
#endif
  hFile = file_descriptor_get_handle (env, objFd) ;
  jbBuffer = (*env)->GetByteArrayElements (env, jarrayByte, NULL) ;

  jiWorkingOffset = jiOffset ;
  jiBytesWritten = 0 ;

  for (eof = OFC_FALSE ; !eof && jiLen > 0 ; )
    {
      lpcBuffer = (OFC_CHAR *) jbBuffer + jiWorkingOffset ;
      bsizeBuffer = OFC_MIN(jiLen, OFC_MAX_IO) ;

      if (OfcWriteFile (hFile, lpcBuffer, (OFC_DWORD) bsizeBuffer, &nWritten, 
			 OFC_HANDLE_NULL) == OFC_FALSE)
	{
	  nWritten = 0 ;
	  eof = 1 ;

	  throwio(env) ;
	}
      jiBytesWritten += nWritten ;
      jiWorkingOffset += nWritten ;
      jiLen -= nWritten ;
    }
  (*env)->ReleaseByteArrayElements (env, jarrayByte, jbBuffer, 0) ;
}
#else
JNIEXPORT void JNICALL Java_com_connectedway_io_FileSystem_write__Lcom_connectedway_io_FileDescriptor_2_3BII
(JNIEnv *env, jobject objFs, jobject objFd, jbyteArray arrayB, 
 jint jiOffset, jint jiLen) {

  OFC_HANDLE hFile ;
  jint jiBytesWritten ;
  jbyte *jbBuffer ;
  OFC_BOOL eof ;
  OFC_LARGE_INTEGER file_offset;
  OFC_OFFT buffer_offset;
  OFC_INT pending;
  OFC_HANDLE buffer_list;
  OFC_FILE_BUFFER *buffer;
  OFC_INT i;
  OFC_HANDLE wait_set;
  OFC_DWORD dwLen;
  ASYNC_RESULT result;
  OFC_HANDLE hEvent;

#if 0
  ofc_printf ("%s:%s:%d\n", __FILE__, __func__, __LINE__) ;
#endif
  hFile = file_descriptor_get_handle (env, objFd) ;
  jbBuffer = (*env)->GetByteArrayElements (env, arrayB, NULL) ;

  jiBytesWritten = 0 ;

  wait_set = ofc_waitset_create();
  buffer_list = ofc_queue_create();

  file_offset = jiOffset;
  buffer_offset = 0;
  eof = OFC_FALSE;
  pending = 0;
      
  for (i = 0; i < NUM_FILE_BUFFERS && !eof && buffer_offset < jiLen; i++)
    {
      /*
       * Get the buffer descriptor and the data buffer
       */
      buffer = ofc_malloc(sizeof(OFC_FILE_BUFFER));
      if (buffer == OFC_NULL)
        {
          ofc_log(OFC_LOG_WARN, "test_file: Failed to alloc buffer context\n");
          eof = OFC_TRUE;
        }
      else
        {
          buffer->data = (OFC_CHAR *) jbBuffer + buffer_offset;
          buffer->offset = file_offset;

          buffer->writeOverlapped = OfcCreateOverlapped(hFile);
          if (buffer->writeOverlapped == OFC_HANDLE_NULL)
            ofc_process_crash("An Overlapped Handle is NULL");

          /*
           * Add it to our buffer list
           */
          ofc_enqueue(buffer_list, buffer);

          pending++;
          dwLen = OFC_MIN(BUFFER_SIZE, jiLen - buffer_offset);
          result = AsyncWrite(wait_set, hFile, buffer, dwLen);
          if (result != ASYNC_RESULT_PENDING)
            {
              /*
               * discount pending and set eof
               */
              pending--;
              /*
               * Set eof either because it really is eof, or we
               * want to clean up.
               */
              eof = OFC_TRUE;
            }
          /*
           * Prepare for the next buffer
           */
          buffer_offset += dwLen;
          file_offset += dwLen;
        }
    }

  /*
   * Now all our buffers should be busy doing writes.  Keep writing
   */
  while (pending > 0)
    {
      /*
       * Wait for some buffer to finish (may be a read if we've
       * just finished priming, but it may be a write also if
       * we've been in this loop a bit
       */
      hEvent = ofc_waitset_wait(wait_set);
      if (hEvent != OFC_HANDLE_NULL)
        {
          /*
           * We use the app of the event as a pointer to the
           * buffer descriptor.  Yeah, this isn't really nice but
           * the alternative is to add a context to each handle.
           * That may be cleaner, but basically unnecessary.  If
           * we did this kind of thing a lot, I'm all for a
           * new property of a handle
           */
          buffer = (OFC_FILE_BUFFER *) ofc_handle_get_app(hEvent);

          if (buffer->state == BUFFER_STATE_WRITE)
            {
              /*
               * Write, so let's see the result of the write
               */
              result = AsyncWriteResult(wait_set, hFile,
                                        buffer, &dwLen);
              if (result == ASYNC_RESULT_DONE)
                {
                  jiBytesWritten += dwLen;

                  dwLen = OFC_MIN(BUFFER_SIZE, jiLen - buffer_offset);
                  if (dwLen > 0)
                    {
                      buffer->data = (OFC_CHAR *) jbBuffer + buffer_offset;
                      buffer->offset = file_offset;
                      
                      result = AsyncWrite(wait_set, hFile,
                                          buffer, dwLen);
                      buffer_offset += dwLen;
                      file_offset += dwLen;
                    }
                }
              if (result != ASYNC_RESULT_PENDING)
                {
                  pending--;
                  eof = OFC_TRUE;
                }
            }
        }
    }

  /*
   * The pending count is zero so we've gotten completions
   * either due to errors or eof on all of our outstanding
   * reads and writes.
   */
  for (buffer = ofc_dequeue(buffer_list);
       buffer != OFC_NULL;
       buffer = ofc_dequeue(buffer_list))
    {
      /*
       * Destroy the overlapped I/O handle for each buffer
       */
      OfcDestroyOverlapped(hFile, buffer->writeOverlapped);
      /*
       * Free the buffer descriptor
       */
      ofc_free(buffer);
    }
  /*
   * Destroy the buffer list
   */
  ofc_queue_destroy(buffer_list);
  /*
   * And destroy the wait list
   */
  ofc_waitset_destroy(wait_set);

  (*env)->ReleaseByteArrayElements (env,arrayB, jbBuffer, 0) ;

  if (jiBytesWritten != jiLen)
    throwio(env) ;
}
#endif

/*
 * Class:     com_connectedway_io_FileSystem
 * Method:    seteof
 * Signature: (Lcom/connectedway/io/FileDescriptor;J)V
 */
JNIEXPORT void JNICALL Java_com_connectedway_io_FileSystem_seteof
  (JNIEnv *env, jobject objFs, jobject objFd, jlong jlPos) 
{
  OFC_HANDLE hFile ;
  OFC_LONG lPos ;
  OFC_DWORD dwLastError ;
  OFC_BOOL bStatus ;
  OFC_LONG lLow ;
  OFC_LONG lHigh ;

#if 0
  ofc_printf ("%s:%s:%d\n", __FILE__, __func__, __LINE__) ;
#endif
  hFile = file_descriptor_get_handle (env, objFd) ;

  lLow = (OFC_LONG) jlPos & 0xFFFFFFFF ;
  lHigh = (OFC_LONG) (jlPos >> 32) & 0xFFFFFFFF ;

  lPos = OfcSetFilePointer (hFile, lLow, &lHigh, OFC_FILE_BEGIN) ;
  dwLastError = OfcGetLastError () ;

  if (lPos == OFC_INVALID_SET_FILE_POINTER &&
      dwLastError != OFC_ERROR_SUCCESS)
    {
      throwio(env) ;
    }
  else
    {
      bStatus = OfcSetEndOfFile (hFile) ;
      if (bStatus != OFC_TRUE)
	{
	  throwio(env) ;
	}
    }

}
	
/*
 * Class:     com_connectedway_io_FileSystem
 * Method:    skip
 * Signature: (Lcom/connectedway/io/FileDescriptor;J)J
 */
JNIEXPORT jlong JNICALL Java_com_connectedway_io_FileSystem_skip
  (JNIEnv *env, jobject objFs, jobject objFd, jlong jlPos)
{
  OFC_HANDLE hFile ;
  OFC_LONG lPos ;
  OFC_DWORD dwLastError ;
  OFC_LONG lLow ;
  OFC_LONG lHigh ;

#if 0
  ofc_printf ("%s:%s:%d\n", __FILE__, __func__, __LINE__) ;
#endif
  hFile = file_descriptor_get_handle (env, objFd) ;

  lLow = (OFC_LONG) jlPos & 0xFFFFFFFF ;
  lHigh = (OFC_LONG) (jlPos >> 32) & 0xFFFFFFFF ;

  lPos = OfcSetFilePointer (hFile, lLow, &lHigh, OFC_FILE_CURRENT) ;
  dwLastError = OfcGetLastError () ;

  if (lPos == OFC_INVALID_SET_FILE_POINTER &&
      dwLastError != OFC_ERROR_SUCCESS)
    {
      jlPos = 0 ;
      throwio(env) ;
    }
  else
    {
      jlPos = ((jlong) lHigh) << 32 | lPos  ;
    }

  return (jlPos) ;
}

/*
 * Class:     com_connectedway_io_FileSystem
 * Method:    flush

 * Signature: (Lcom/connectedway/io/FileDescriptor;)V
 */
JNIEXPORT void JNICALL Java_com_connectedway_io_FileSystem_flush
  (JNIEnv *env, jobject objFs, jobject objFd)
{
  OFC_HANDLE hFile ;
  OFC_BOOL bStatus ;

#if 0
  ofc_printf ("%s:%s:%d\n", __FILE__, __func__, __LINE__) ;
#endif
  hFile = file_descriptor_get_handle (env, objFd) ;
  bStatus = OfcFlushFileBuffers (hFile) ;
  if (bStatus != OFC_TRUE)
    {
      throwio(env) ;
    }
}

/*
 * Class:     com_connectedway_io_FileSystem
 * Method:    available
 * Signature: (Lcom/connectedway/io/FileDescriptor;)I
 */
JNIEXPORT jint JNICALL Java_com_connectedway_io_FileSystem_available
  (JNIEnv *env, jobject objFs, jobject objFd)
{
  /*
   * We have no way of testing whether we'd block so always return 0.
   */
  return 0 ;
}

/*
 * Class:     com_connectedway_io_FileSystem
 * Method:    close
 * Signature: (Lcom/connectedway/io/FileDescriptor;)V
 */
JNIEXPORT void JNICALL Java_com_connectedway_io_FileSystem_close
  (JNIEnv *env, jobject objFs, jobject objFd)
{
  OFC_HANDLE hFile ;
  OFC_BOOL bStatus ;

#if 0
  ofc_printf ("%s:%s:%d\n", __FILE__, __func__, __LINE__) ;
#endif
  hFile = file_descriptor_get_handle (env, objFd) ;
  bStatus = OfcCloseHandle (hFile) ;
  if (bStatus != OFC_TRUE)
    {
      throwio(env) ;
    }
}

/*
 * Class:     com_connectedway_io_FileSystem
 * Method:    seek
 * Signature: (Lcom/connectedway/io/FileDescriptor;IJ)J
 */
JNIEXPORT jlong JNICALL Java_com_connectedway_io_FileSystem_seek
  (JNIEnv *env, jobject objFs, jobject objFd, jint jiMode, jlong jlPos)
{
  OFC_HANDLE hFile ;
  OFC_LONG lPos ;
  OFC_DWORD dwLastError ;
  OFC_LONG lLow ;
  OFC_LONG lHigh ;
  OFC_DWORD dwMode ;

#if 0
  ofc_printf ("%s:%s:%d\n", __FILE__, __func__, __LINE__) ;
#endif
  hFile = file_descriptor_get_handle (env, objFd) ;

  lLow = (OFC_LONG) jlPos & 0xFFFFFFFF ;
  lHigh = (OFC_LONG) (jlPos >> 32) & 0xFFFFFFFF ;

  if (jiMode == com_connectedway_io_FileSystem_SEEK_SET)
    dwMode = OFC_FILE_BEGIN ;
  else if (jiMode == com_connectedway_io_FileSystem_SEEK_CUR)
    dwMode = OFC_FILE_CURRENT ;
  else if (jiMode == com_connectedway_io_FileSystem_SEEK_END)
    dwMode = OFC_FILE_END ;
  else
    dwMode = OFC_FILE_BEGIN ;

  lPos = OfcSetFilePointer (hFile, lLow, &lHigh, dwMode) ;
  dwLastError = OfcGetLastError () ;

  if (lPos == OFC_INVALID_SET_FILE_POINTER &&
      dwLastError != OFC_ERROR_SUCCESS)
    {
      jlPos = 0 ;
      throwio(env) ;
    }
  else
    {
      jlPos = ((jlong) lHigh) << 32 | lPos  ;
    }

  return (jlPos) ;
}

JNIEXPORT jlong JNICALL Java_com_connectedway_io_FileSystem_getLastError
(JNIEnv *env, jobject objFs) 
{
  OFC_DWORD lerror ;

#if 0
  ofc_printf ("%s:%s:%d\n", __FILE__, __func__, __LINE__) ;
#endif
  lerror = OfcGetLastError () ;
  return ((jlong) lerror) ;
}

JNIEXPORT jstring JNICALL Java_com_connectedway_io_FileSystem_getLastErrorString
(JNIEnv *env, jobject objFs) 
{
  jstring jstr = (jstring) 0 ;
  const char *errstr ;
#if 0
  ofc_printf ("%s:%s:%d\n", __FILE__, __func__, __LINE__) ;
#endif

  if (OfcGetLastError() != OFC_ERROR_SUCCESS)
    {
      errstr = ofc_get_error_string(OfcGetLastError()) ;
      jstr = (*env)->NewStringUTF (env, errstr) ;
    }

  return (jstr) ;
}

//
// this (and the subsequent findClose method) are alot like the listFiles method.  The difference is this one
// does it one file at a time.
//
JNIEXPORT jobject Java_com_connectedway_io_FileSystem_findFile(JNIEnv *env, jobject objFS, jobject objDir)
{
  OFC_BOOL status ;
  OFC_LPTSTR tstrPath ;
  OFC_HANDLE list_handle ;

  OFC_WIN32_FIND_DATAW *find_data ;
  OFC_BOOL more ;
  OFC_ULONG tv_sec ;
  OFC_ULONG tv_nsec ;

  jstring jstrFile ;
  jint booleanAttributes ;
  jobject objFile ;
  jobject objParent ;

  // 
  // File methods
  //
  jmethodID midSetAttributes ;
  jmethodID midSetLength ;
  jmethodID midSetDate ;
  //
  // Directory methods
  //
  jmethodID midGetHandle ;
  jmethodID midSetHandle ;
  jmethodID midGetParent ;

  jclass clsDir ;
  jclass clsOfcFile ;

  //
  // Get File class and methods
  //
  clsOfcFile = (*env)->FindClass (env, "com/connectedway/io/File") ;
  midSetAttributes = (*env)->GetMethodID(env, clsOfcFile, "setAttributes", "(I)V") ;
  midSetLength = (*env)->GetMethodID(env, clsOfcFile, "setLength", "(J)V") ;
  midSetDate = (*env)->GetMethodID(env, clsOfcFile, "setDate", "(J)V") ;
  //
  // Get directory class and methods
  //
  clsDir = (*env)->FindClass (env, "com/connectedway/nio/directory/Directory") ;
  midGetHandle = (*env)->GetMethodID(env, clsDir, "getHandle", "()J") ;
  midSetHandle = (*env)->GetMethodID(env, clsDir, "setHandle", "(J)V") ;
  midGetParent = (*env)->GetMethodID(env, clsDir, "getParent", "()Lcom/connectedway/io/File;") ;

  objFile = OFC_NULL ;
  objParent = (*env)->CallObjectMethod (env, objDir, midGetParent) ;

  /*
   * We'll use find_data as null to tell us whether we have a file to return or not
   */
  find_data = OFC_NULL ;
  /*
   * List handle will tell us if we are open or not
   */
  list_handle = (OFC_HANDLE) (*env)->CallLongMethod (env, objDir, midGetHandle) ;
  /*
   * If we are not open, then open it with a find first
   */
  if (list_handle == OFC_INVALID_HANDLE_VALUE)
    {
      tstrPath = file_get_path (env, objParent) ;

      booleanAttributes = get_boolean_attributes (tstrPath) ;
      if (booleanAttributes & com_connectedway_io_FileSystem_BA_DIRECTORY)
	{
	  OFC_SIZET len = ofc_tstrlen (tstrPath) ;
	  
	  if (len > 0 && tstrPath[len-1] != TCHAR_SLASH &&
	      tstrPath[len-1] != TCHAR_BACKSLASH)
	    {
	      tstrPath = ofc_realloc (tstrPath, 
					  (len + 3) * sizeof (OFC_TCHAR)) ;
	      ofc_tstrcpy (tstrPath + ofc_tstrlen(tstrPath), TSTR("/*")) ;
	    }
	  else
	    {
	      /*
	       * Reallocate bigger so we can add "*" and include EOS
	       */
	      tstrPath = ofc_realloc (tstrPath, 
					  (len + 2) * sizeof (OFC_TCHAR)) ;
	      ofc_tstrcpy (tstrPath + ofc_tstrlen(tstrPath), TSTR("*")) ;
	    }
	}
      find_data = ofc_malloc (sizeof (OFC_WIN32_FIND_DATAW)) ;
      /*
       * Now do the open and get the first file
       */
      list_handle = OfcFindFirstFileW (tstrPath, find_data, &more) ;
      ofc_free (tstrPath) ;
        
      if (list_handle == OFC_INVALID_HANDLE_VALUE)
	{
	  jclass newExcCls ;
	  OFC_DWORD dwLastError ;
	  /*
	   * Find first failed, error
	   */
	  ofc_free (find_data) ;
	  find_data = OFC_NULL ;

	  dwLastError = OfcGetLastError() ;
	  if (dwLastError == OFC_ERROR_ACCESS_DENIED ||
	      dwLastError == OFC_ERROR_INVALID_PASSWORD)
	    newExcCls = (*env)->FindClass(env, "java/lang/SecurityException");
	  else
	    {
	      newExcCls = (*env)->FindClass(env, "java/io/FileNotFoundException");
	    }
	  if (newExcCls != NULL)
	    {
	      (*env)->ThrowNew(env, newExcCls, "Cannot open directory") ;
	      (*env)->DeleteLocalRef (env, newExcCls) ;
	    }
	}
      else
	{
	  /* store away the handle */
	  (*env)->CallVoidMethod (env, objDir, midSetHandle, (jlong) list_handle) ;

	  if (ofc_tstrcmp (find_data->cFileName, TSTR("..")) == 0 ||
	      ofc_tstrcmp (find_data->cFileName, TSTR(".")) == 0 ||
              (find_data->dwFileAttributes & OFC_FILE_ATTRIBUTE_HIDDEN))
	    {
	      ofc_free (find_data) ;
	      find_data = OFC_NULL ;
	    }
	}
    }
  /*
   * at this point, the directory is opened.  We may or may not have already
   * gotten file data.
   * the following loop is to get a next file.  Essentially we want to do that
   * if we have a list handle but we don't have any find data.  That may
   * be because it had been opened previously, or it may be because we
   * just opened it but it was a hidden file that we skipped
   */
  status = OFC_TRUE ;
  while (status && list_handle != OFC_INVALID_HANDLE_VALUE && find_data == OFC_NULL)
    {
      find_data = ofc_malloc (sizeof (OFC_WIN32_FIND_DATAW)) ;
      status = OfcFindNextFileW (list_handle, find_data, &more) ;
      /*
       * if it failed for no more files or any other error, we're going to 
       * return.  Both will ultimately be treated the same at this api
       */
      if (status == OFC_FALSE)
	{
	  ofc_free (find_data) ;
	  find_data = OFC_NULL ;
	}
      else
	{
	  if (ofc_tstrcmp (find_data->cFileName, TSTR("..")) == 0 ||
	      ofc_tstrcmp (find_data->cFileName, TSTR(".")) == 0 ||
              (find_data->dwFileAttributes & OFC_FILE_ATTRIBUTE_HIDDEN))
	    {
	      ofc_free (find_data) ;
	      find_data = OFC_NULL ;
	    }
	}
    }
  /*
   * we have an open directory and a find_data either from a find first
   * or a find next and it's not a hidden file.  If there was some
   * problem, we will have set find_data to null
   */
  if (find_data != OFC_NULL)
    {
      /*
       * Let's create a file object for the file we just found
       */
      jstrFile = tchar2jstr (env, find_data->cFileName) ;
      objFile = new_child_file (env, objParent, jstrFile) ;
      (*env)->DeleteLocalRef (env, jstrFile) ;

      /*
       * Set attributes that we already have
       */
      jint attributes = 0 ;

      attributes |= com_connectedway_io_FileSystem_BA_EXISTS ;
      if (find_data->dwFileAttributes & OFC_FILE_ATTRIBUTE_BOOKMARK)
	attributes |= com_connectedway_io_FileSystem_BA_BOOKMARK ;
      if (find_data->dwFileAttributes & OFC_FILE_ATTRIBUTE_DIRECTORY)
	attributes |= com_connectedway_io_FileSystem_BA_DIRECTORY ;
      if (find_data->dwFileAttributes & OFC_FILE_ATTRIBUTE_NORMAL ||
	  find_data->dwFileAttributes & OFC_FILE_ATTRIBUTE_ARCHIVE)
	attributes |= com_connectedway_io_FileSystem_BA_REGULAR ;
      if (find_data->dwFileAttributes & OFC_FILE_ATTRIBUTE_HIDDEN)
	attributes |= com_connectedway_io_FileSystem_BA_HIDDEN ;
      if (find_data->dwFileAttributes & OFC_FILE_FLAG_SHARE)
	attributes |= com_connectedway_io_FileSystem_BA_SHARE ;
      if (find_data->dwFileAttributes & OFC_FILE_FLAG_SERVER)
	attributes |= com_connectedway_io_FileSystem_BA_SERVER ;
      if (find_data->dwFileAttributes & OFC_FILE_FLAG_WORKGROUP)
	attributes |= com_connectedway_io_FileSystem_BA_WORKGROUP ;

      (*env)->CallVoidMethod (env, objFile, midSetAttributes, 
			      attributes) ;

      jlong size = ((jlong) find_data->nFileSizeHigh << 32) | (jlong) find_data->nFileSizeLow ;
      (*env)->CallVoidMethod (env, objFile, midSetLength, size) ;

      file_time_to_epoch_time (&find_data->ftLastWriteTime,
			   &tv_sec, &tv_nsec) ;
      jlong date = ((jlong) tv_sec * 1000) + ((jlong) tv_nsec / (1000 * 1000)) ;
      (*env)->CallVoidMethod (env, objFile, midSetDate, date) ;

      ofc_free (find_data) ;
      find_data = OFC_NULL ;
    }
      
  (*env)->DeleteLocalRef (env, objParent) ;
  (*env)->DeleteLocalRef (env, clsOfcFile) ;
  (*env)->DeleteLocalRef (env, clsDir) ;

  return (objFile) ;
}

JNIEXPORT void Java_com_connectedway_io_FileSystem_findClose(JNIEnv *env, jobject objFS, jobject objDir)
{
  OFC_HANDLE list_handle ;

  jclass clsDir ;
  jmethodID midGetHandle ;
  jmethodID midSetHandle ;

  clsDir = (*env)->FindClass (env, "com/connectedway/nio/directory/Directory") ;

  midGetHandle = (*env)->GetMethodID(env, clsDir, "getHandle", "()J") ;
  midSetHandle = (*env)->GetMethodID(env, clsDir, "setHandle", "(J)V") ;

  list_handle = (OFC_HANDLE) (*env)->CallLongMethod (env, objDir, midGetHandle) ;
  if (list_handle != OFC_INVALID_HANDLE_VALUE)
    {
      OfcFindClose (list_handle) ;
      list_handle = OFC_INVALID_HANDLE_VALUE ;
      (*env)->CallVoidMethod (env, objDir, midSetHandle, (jlong) list_handle) ;
    }      
  (*env)->DeleteLocalRef (env, clsDir) ;
}
