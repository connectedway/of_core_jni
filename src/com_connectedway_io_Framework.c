/* Copyright (c) 2021 Connected Way, LLC. All rights reserved.
 * Use of this source code is governed by a Creative Commons 
 * Attribution-NoDerivatives 4.0 International license that can be
 * found in the LICENSE file.
 */
#define __OFC_CORE_DLL__
#include <jni.h>
#include <stdlib.h>

#include "ofc/config.h"
#include "ofc/types.h"
#include "ofc/framework.h"
#include "ofc/persist.h"
#include "ofc/net.h"
#include "ofc/net_internal.h"
#include "ofc/heap.h"
#include "ofc/libc.h"
#include "ofc/path.h"

#include "ofc_jni/com_connectedway_io_Utils.h"
#include "ofc_jni/com_connectedway_io_Framework.h"

#if defined(__ANDROID__) || defined(ANDROID)
static OFC_UINT get_library_address() 
{ 
  FILE* file ;
  OFC_UINT addr = 0; 
  OFC_CCHAR *libraryName = "libandroidsmb.so"; 
  OFC_INT len_libname ;
  OFC_CHAR buff[256]; 

  file = fopen ("/proc/net/arp", "rt"); 

  if (file != OFC_NULL) 
    {
      while (fgets(buff, sizeof(buff), file) != NULL)
	{ 
	  // dump entire file (all lines)
	  // but also search for libandroidsmb
	  ofc_printf ("%s", buff) ;
	} 
    }
  fclose (file); 

  len_libname = ofc_strlen(libraryName); 

  file = fopen ("/proc/self/maps", "rt"); 

  if (file != OFC_NULL) 
    {
      while (fgets(buff, sizeof buff, file) != NULL)
	{ 
	  // dump entire file (all lines)
	  // but also search for libandroidsmb
	  ofc_printf ("%s", buff) ;
	  OFC_INT len = ofc_strlen(buff); 

	  if (len > 0 && buff[len-1] == '\n') 
	    { 
	      buff[--len] = '\0'; 
	    } 

	  if (len > len_libname &&
	      ofc_memcmp(buff + len - len_libname,
			  libraryName, len_libname) == 0)
	    {
	      size_t start ;
	      size_t end ;
	      size_t offset; 
	      OFC_CHAR flags[4]; 

	      if (sscanf (buff, "%zx-%zx %c%c%c%c %zx", &start, &end, 
			  &flags[0], &flags[1], &flags[2], &flags[3], 
			  &offset ) == 7) 
		{
		  if (flags[0]=='r' && flags[1]=='-' && flags[2]=='x') 
		    { 
		      addr = (OFC_UINT) (start - offset); 
		    } 
		}
	    }
	} 
    }

  fclose (file); 
  return addr; 
} 
#endif

/*
 * Class:     com_connectedway_io_Framework
 * Method:    init
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_com_connectedway_io_Framework_init
  (JNIEnv *env, jobject objFramework)
{
  ofc_framework_init() ;

#if defined(__ANDROID__) || defined(ANDROID)
  /* TBD: Fix get_library_addresses on latest android */
#if 0
  ofc_printf ("libanddroidsmb loaded at 0x%08x\n", get_library_address()) ;
#endif
#endif
}

/*
 * Class:     com_connectedway_io_Framework
 * Method:    startup
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_com_connectedway_io_Framework_startup
  (JNIEnv *env, jobject objFramework)
{
  ofc_framework_startup() ;
}

/*
 * Class:     com_connectedway_io_Framework
 * Method:    load
 * Signature: (Lcom/connectedway/io/File;)V
 */
JNIEXPORT void JNICALL Java_com_connectedway_io_Framework_load
(JNIEnv *env, jobject objFramework, jobject objFile)
{
  OFC_LPTSTR str ;

  const char *cstrFile ;
  jmethodID midToString ;
  jclass clsBlueFile ;

  jstring jstringFile ;

  clsBlueFile = (*env)->FindClass
    (env, "com/connectedway/io/File") ;
  midToString = (*env)->GetMethodID 
    (env, clsBlueFile, "toString", "()Ljava/lang/String;") ;
  (*env)->DeleteLocalRef (env, clsBlueFile) ;

  jstringFile = (*env)->CallObjectMethod (env, objFile, midToString) ;
  cstrFile = (*env)->GetStringUTFChars (env, jstringFile, NULL) ;

  (*env)->ReleaseStringUTFChars (env, jstringFile, cstrFile) ;
  (*env)->DeleteLocalRef (env, jstringFile) ;

  str = file_get_path (env, objFile) ;

  ofc_framework_load(str) ;
  file_free_path (str) ;
}

/*
 * Class:     com_connectedway_io_Framework
 * Method:    save
 * Signature: (Lcom/connectedway/io/File;)V
 */
JNIEXPORT void JNICALL Java_com_connectedway_io_Framework_save
(JNIEnv *env, jobject objFramework, jobject objFile)
{
  OFC_LPTSTR str ;

  str = file_get_path (env, objFile) ;
  ofc_framework_save(str) ;
  file_free_path (str) ;
}

/*
 * Class:     com_connectedway_io_Framework
 * Method:    setHostname
 * Signature: (Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;)V
 */
JNIEXPORT void JNICALL Java_com_connectedway_io_Framework_setHostname
  (JNIEnv *env, jobject objFramework, 
   jstring jstrHostname, jstring jstrWorkgroup, jstring jstrDesc)
{
  OFC_LPTSTR tstrHostname ;
  OFC_LPTSTR tstrWorkgroup ;
  OFC_LPTSTR tstrDesc ;

  tstrHostname = jstr2tchar (env, jstrHostname) ;
  tstrWorkgroup = jstr2tchar (env, jstrWorkgroup) ;
  tstrDesc = jstr2tchar (env, jstrDesc) ;

  ofc_framework_set_host_name (tstrHostname, tstrWorkgroup, tstrDesc) ;

  ofc_free (tstrHostname) ;
  ofc_free (tstrWorkgroup) ;
  ofc_free (tstrDesc) ;
}

/*
 * Class:     com_connectedway_io_Framework
 * Method:    getHostname
 * Signature: ()Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL Java_com_connectedway_io_Framework_getHostname
  (JNIEnv *env, jobject objFramework)
{
  jstring jstrHostname ;
  OFC_LPTSTR tstrHostname ;

  tstrHostname = ofc_framework_get_host_name() ;

  jstrHostname = tchar2jstr (env, tstrHostname) ;

  ofc_framework_free_host_name(tstrHostname) ;
  
  return (jstrHostname) ;
}

/*
 * Class:     com_connectedway_io_Framework
 * Method:    getWorkgroup
 * Signature: ()Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL Java_com_connectedway_io_Framework_getWorkgroup
  (JNIEnv *env, jobject objFramework)
{
  jstring jstrWorkgroup ;
  OFC_LPTSTR tstrWorkgroup ;

  tstrWorkgroup = ofc_framework_get_workgroup () ;

  jstrWorkgroup = tchar2jstr (env, tstrWorkgroup) ;

  ofc_framework_free_workgroup(tstrWorkgroup) ;
  
  return (jstrWorkgroup) ;
}

/*
 * Class:     com_connectedway_io_Framework
 * Method:    getDescription
 * Signature: ()Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL Java_com_connectedway_io_Framework_getDescription
  (JNIEnv *env, jobject objFramework)
{
  jstring jstrDesc ;
  OFC_LPTSTR tstrDesc ;

  tstrDesc = ofc_framework_get_description () ;

  jstrDesc = tchar2jstr (env, tstrDesc) ;

  ofc_framework_free_description(tstrDesc) ;
  
  return (jstrDesc) ;
}

/*
 * Class:     com_connectedway_io_Framework
 * Method:    setUUID
 * Signature: (Ljava/util/UUID;)V
 */
JNIEXPORT void JNICALL Java_com_connectedway_io_Framework_setUUID
  (JNIEnv *env, jobject objFramework, jobject objUUID)
{
  const char *jUUID ;
  jstring jstrUUID ;
  jmethodID midToString ;
  jclass clsUUID ;
  
  clsUUID = (*env)->FindClass 
    (env, "java/util/UUID") ;
  midToString = 
    (*env)->GetMethodID(env, clsUUID, "toString", "()Ljava/lang/String;") ;
  (*env)->DeleteLocalRef (env, clsUUID) ;

  jstrUUID = (*env)->CallObjectMethod (env, objUUID, midToString) ;
  jUUID = (*env)->GetStringUTFChars (env, jstrUUID, NULL) ;

  ofc_framework_set_uuid (jUUID) ;

  (*env)->ReleaseStringUTFChars(env, jstrUUID, jUUID) ;
  (*env)->DeleteLocalRef (env, jstrUUID) ;
}

/*
 * Class:     com_connectedway_io_Framework
 * Method:    getUUID
 * Signature: ()Ljava/util/UUID;
 */
JNIEXPORT jobject JNICALL Java_com_connectedway_io_Framework_getUUID
  (JNIEnv *env, jobject objFramework)
{
  OFC_CHAR *jUUID ;
  jstring jstrUUID ;
  jobject objUUID ;
  jmethodID midFromString ;
  jclass clsUUID ;

  jUUID = ofc_framework_get_uuid() ;

  jstrUUID = (*env)->NewStringUTF (env, jUUID) ;

  clsUUID = (*env)->FindClass 
    (env, "java/util/UUID") ;
  midFromString = (*env)->GetStaticMethodID(env, clsUUID, "fromString", 
				  "(Ljava/lang/String;)Ljava/util/UUID;") ;

  objUUID = (*env)->CallStaticObjectMethod(env, clsUUID, 
					   midFromString, jstrUUID) ;
  (*env)->DeleteLocalRef (env, clsUUID) ;
  (*env)->DeleteLocalRef (env, jstrUUID) ;

  ofc_framework_free_uuid(jUUID) ;
  
  return (objUUID) ;
}

/*
 * Class:     com_connectedway_io_Framework
 * Method:    getRootDir
 * Signature: ()Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL Java_com_connectedway_io_Framework_getRootDir
  (JNIEnv *env, jobject objFramework)
{
  jstring jstrRootDir ;
  OFC_LPTSTR tstrRootDir ;

  tstrRootDir = ofc_framework_get_root_dir() ;

  jstrRootDir = tchar2jstr (env, tstrRootDir) ;

  ofc_framework_free_root_dir(tstrRootDir) ;
  
  return (jstrRootDir) ;
}

/*
 * Class:     com_connectedway_io_Framework
 * Method:    setInterfaceDiscovery
 * Signature: (Z)V
 */
JNIEXPORT void JNICALL Java_com_connectedway_io_Framework_setInterfaceDiscovery
  (JNIEnv *env, jobject objFramework, jboolean jon)
{
  OFC_BOOL on ;

  on = OFC_FALSE ;
  if (jon)
    on = OFC_TRUE ;
  ofc_framework_set_interface_discovery(on) ;
}

/*
 * Class:     com_connectedway_io_Framework
 * Method:    getInterfaceDiscovery
 * Signature: ()Z
 */
JNIEXPORT jboolean JNICALL Java_com_connectedway_io_Framework_getInterfaceDiscovery
  (JNIEnv *env, jobject objFramework)
{
  OFC_BOOL on ;
  jboolean jon ;

  on = ofc_framework_get_interface_discovery() ;
  jon = JNI_FALSE ;
  if (on == OFC_TRUE)
    jon = JNI_TRUE ;
  return (jon) ;
}

OFC_CONFIG_MODE interface_get_netbios_mode (JNIEnv *env, jobject objInterface)
{
  jmethodID midGetNetBIOSMode ;
  jmethodID midToString ;
  jstring jstrValue ;
  const char *szValue ;
  jobject objNetBiosMode ;
  jclass clsInterface ;
  jclass clsNetBIOSMode ;

  OFC_CONFIG_MODE mode ;
  
  clsInterface = (*env)->FindClass
    (env, "com/connectedway/io/Framework$Interface") ;
  midGetNetBIOSMode = 
    (*env)->GetMethodID(env, clsInterface, "getNetBIOSMode", 
			"()Lcom/connectedway/io/Framework$netBIOSMode;") ;
  (*env)->DeleteLocalRef (env, clsInterface) ;
  
  objNetBiosMode = 
    (*env)->CallObjectMethod(env, objInterface, midGetNetBIOSMode) ;

  mode = OFC_CONFIG_BMODE ;

  if (objNetBiosMode != NULL)
    {
      /*
       * Translate enum value from java to C string
       */
      clsNetBIOSMode = (*env)->FindClass
	(env, "com/connectedway/io/Framework$netBIOSMode") ;
      midToString = (*env)->GetMethodID(env, clsNetBIOSMode, "toString", 
					"()Ljava/lang/String;") ;
      (*env)->DeleteLocalRef (env, clsNetBIOSMode) ;

      jstrValue = (*env)->CallObjectMethod(env, objNetBiosMode, midToString) ;
      szValue = (*env)->GetStringUTFChars(env, jstrValue, NULL) ;
      /*
       * Now convert the C string to a C enum
       */
      if (ofc_strcmp (szValue, "BMODE") == 0)
	mode = OFC_CONFIG_BMODE ;
      else if (ofc_strcmp (szValue, "PMODE") == 0)
	mode = OFC_CONFIG_PMODE ;
      else if (ofc_strcmp (szValue, "MMODE") == 0)
	mode = OFC_CONFIG_MMODE ;
      else if (ofc_strcmp (szValue, "HMODE") == 0)
	mode = OFC_CONFIG_HMODE ;
  
      (*env)->ReleaseStringUTFChars(env, jstrValue, szValue) ;
      (*env)->DeleteLocalRef (env, objNetBiosMode) ;
      (*env)->DeleteLocalRef (env, jstrValue) ;
    }
  return (mode) ;
}

static OFC_VOID inet_address_to_ipaddr (JNIEnv *env, jobject objInetAddress,
				      OFC_IPADDR *ip) 
{
  jmethodID midToString ;
  jstring jstrInetAddress ;
  const char *szInetAddress ;
  jclass clsInetAddress ;

  clsInetAddress = (*env)->FindClass (env, "java/net/InetAddress") ;
  midToString = (*env)->GetMethodID(env, clsInetAddress, "getHostAddress", 
				    "()Ljava/lang/String;") ;
  (*env)->DeleteLocalRef (env, clsInetAddress) ;

  jstrInetAddress = (*env)->CallObjectMethod(env, objInetAddress, midToString) ;
  szInetAddress = (*env)->GetStringUTFChars (env, jstrInetAddress, NULL) ;

  ofc_pton (szInetAddress, ip) ;

  (*env)->ReleaseStringUTFChars (env, jstrInetAddress, szInetAddress) ;
  (*env)->DeleteLocalRef (env, jstrInetAddress) ;
}

OFC_VOID interface_get_bcast_address (JNIEnv *env, jobject objInterface,
				    OFC_IPADDR *bcast)
{
  jmethodID midGetBcastAddress ;
  jobject objInetAddress ;
  jclass clsInterface ;

  clsInterface = (*env)->FindClass
    (env, "com/connectedway/io/Framework$Interface") ;
  midGetBcastAddress = (*env)->GetMethodID
    (env, clsInterface, "getBcastAddress", 
     "()Ljava/net/InetAddress;") ;
  (*env)->DeleteLocalRef (env, clsInterface) ;
  objInetAddress = (*env)->CallObjectMethod(env, objInterface, midGetBcastAddress) ;

  if (objInetAddress != NULL)
    {
      
      inet_address_to_ipaddr (env, objInetAddress, bcast) ;
      (*env)->DeleteLocalRef (env, objInetAddress) ;
    }
}

static OFC_VOID interface_get_ip_address (JNIEnv *env, jobject objInterface,
					OFC_IPADDR *ip)
{
  jmethodID midGetIPAddress ;
  jobject objInetAddress ;
  jclass clsInterface ;

  clsInterface = (*env)->FindClass
    (env, "com/connectedway/io/Framework$Interface") ;
  midGetIPAddress = (*env)->GetMethodID(env, clsInterface, "getIpAddress", 
					"()Ljava/net/InetAddress;") ;
  (*env)->DeleteLocalRef (env, clsInterface) ;
  objInetAddress = (*env)->CallObjectMethod(env, objInterface, midGetIPAddress) ;

  if (objInetAddress != NULL)
    {
      inet_address_to_ipaddr (env, objInetAddress, ip) ;
      (*env)->DeleteLocalRef (env, objInetAddress) ;
    }
}

OFC_VOID interface_get_mask_address (JNIEnv *env, jobject objInterface,
				   OFC_IPADDR *mask)
{
  jmethodID midGetMaskAddress ;
  jobject objInetAddress ;
  jclass clsInterface ;

  clsInterface = (*env)->FindClass
    (env, "com/connectedway/io/Framework$Interface") ;
  midGetMaskAddress = (*env)->GetMethodID(env, clsInterface, "getMask", 
					  "()Ljava/net/InetAddress;") ;
  (*env)->DeleteLocalRef (env, clsInterface) ;
  objInetAddress = (*env)->CallObjectMethod(env, objInterface, midGetMaskAddress) ;

  if (objInetAddress != OFC_NULL)
    {
      inet_address_to_ipaddr (env, objInetAddress, mask) ;
      (*env)->DeleteLocalRef (env, objInetAddress) ;
    }

}

OFC_LPSTR interface_get_lmb (JNIEnv *env, jobject objInterface)
{
  jmethodID midGetDefaultLmb ;

  jstring jstrLmb ;
  const char *szLmb ;
  OFC_LPSTR cstrLmb ;
  jclass clsInterface ;

  /*
   * Now get local master browser
   */
  clsInterface = (*env)->FindClass
    (env, "com/connectedway/io/Framework$Interface") ;
  midGetDefaultLmb = (*env)->GetMethodID(env, clsInterface, "getDefaultLmb", 
					 "()Ljava/lang/String;") ;
  (*env)->DeleteLocalRef (env, clsInterface) ;
  jstrLmb = (*env)->CallObjectMethod(env, objInterface, midGetDefaultLmb) ;
  cstrLmb = OFC_NULL ;
  if (jstrLmb != NULL)
    {
      szLmb = (*env)->GetStringUTFChars (env, jstrLmb, NULL) ;
      cstrLmb = ofc_strdup (szLmb) ;
      (*env)->ReleaseStringUTFChars (env, jstrLmb, szLmb) ;
      (*env)->DeleteLocalRef (env, jstrLmb) ;
    }
  return (cstrLmb) ;
}

OFC_VOID interface_get_wins (JNIEnv *env, jobject objInterface, 
			    OFC_FRAMEWORK_WINSLIST *wins)
{
  jmethodID midGetWins ;
  jarray arrayWins ;
  int i ;
  jobject objInetAddress ;
  jclass clsInterface ;

  if (objInterface == NULL)
    ofc_printf ("Wins is null\n") ;
  else
    {
      clsInterface = (*env)->FindClass
	(env, "com/connectedway/io/Framework$Interface") ;
      midGetWins = (*env)->GetMethodID(env, clsInterface, "getWins", 
				       "()[Ljava/net/InetAddress;") ;
      (*env)->DeleteLocalRef (env, clsInterface) ;
      arrayWins = (*env)->CallObjectMethod(env, objInterface, midGetWins) ;
      if (arrayWins == NULL)
	{
	  wins->num_wins = 0 ;
	  wins->winsaddr = OFC_NULL ;
	}
      else
	{
	  wins->num_wins = (*env)->GetArrayLength (env, arrayWins) ;
	  wins->winsaddr = 
	    ofc_malloc (sizeof (OFC_INADDR) * wins->num_wins) ;

	  for (i = 0 ; i < wins->num_wins ; i++)
	    {
	      objInetAddress = 
		(*env)->GetObjectArrayElement (env, arrayWins, i) ;
	      inet_address_to_ipaddr (env, objInetAddress, &wins->winsaddr[i]) ;
	      (*env)->DeleteLocalRef (env, objInetAddress) ;
	    }
	}
    }
}

OFC_VOID interface_free_lmb (OFC_LPSTR lmb) 
{
  ofc_free (lmb) ;
}

OFC_VOID interface_free_wins (OFC_FRAMEWORK_WINSLIST *wins)
{
  ofc_free (wins->winsaddr) ;
}

OFC_LPTSTR map_get_name (JNIEnv *env, jobject objMap)
{
  jmethodID midGetName ;

  jstring jstrName ;
  OFC_LPTSTR tstrName ;
  jclass clsMap ;

  /*
   * Now get local master browser
   */
  clsMap = (*env)->FindClass
    (env, "com/connectedway/io/Framework$Map") ;
  midGetName = (*env)->GetMethodID(env, clsMap, "getName", 
				   "()Ljava/lang/String;") ;
  (*env)->DeleteLocalRef (env, clsMap) ;
  jstrName = (*env)->CallObjectMethod(env, objMap, midGetName) ;

  tstrName = OFC_NULL ;
  if (jstrName != NULL)
    {
      tstrName = jstr2tchar (env, jstrName) ;
      (*env)->DeleteLocalRef (env, jstrName) ;
    }

  return (tstrName) ;
}

OFC_LPTSTR map_get_desc (JNIEnv *env, jobject objMap)
{
  jmethodID midGetDesc ;

  jstring jstrDesc ;
  OFC_LPTSTR tstrDesc ;
  jclass clsMap ;

  clsMap = (*env)->FindClass
    (env, "com/connectedway/io/Framework$Map") ;
  midGetDesc = (*env)->GetMethodID(env, clsMap, "getDescription", 
				   "()Ljava/lang/String;") ;
  (*env)->DeleteLocalRef (env, clsMap) ;
  jstrDesc = (*env)->CallObjectMethod(env, objMap, midGetDesc) ;

  tstrDesc = OFC_NULL ;
  if (jstrDesc != NULL)
    {
      tstrDesc = jstr2tchar (env, jstrDesc) ;
      (*env)->DeleteLocalRef (env, jstrDesc) ;
    }

  return (tstrDesc) ;
}

OFC_BOOL map_get_thumbnail (JNIEnv *env, jobject objMap)
{
  jmethodID midGetThumbnail ;

  jboolean zThumbnail ;
  jclass clsMap ;

  clsMap = (*env)->FindClass
    (env, "com/connectedway/io/Framework$Map") ;
  midGetThumbnail = (*env)->GetMethodID(env, clsMap, "getThumbnailMode", 
					"()Z") ;
  (*env)->DeleteLocalRef (env, clsMap) ;
  zThumbnail = (*env)->CallBooleanMethod(env, objMap, midGetThumbnail) ;

  return (zThumbnail == JNI_TRUE) ;
}

OFC_LPTSTR map_get_path (JNIEnv *env, jobject objMap)
{
  jmethodID midGetPath ;
  jmethodID midToString ;

  jobject objFile ;

  jstring jstrPath ;
  OFC_LPTSTR tstrPath ;
  jclass clsFile ;
  jclass clsMap ;

  clsMap = (*env)->FindClass
    (env, "com/connectedway/io/Framework$Map") ;
  midGetPath = (*env)->GetMethodID(env, clsMap, "getPath", 
				   "()Lcom/connectedway/io/File;") ;
  (*env)->DeleteLocalRef (env, clsMap) ;
  objFile = (*env)->CallObjectMethod(env, objMap, midGetPath) ;

  tstrPath = OFC_NULL ;
  if (objFile != NULL)
    {
      clsFile = (*env)->FindClass (env, "com/connectedway/io/File") ;
      midToString = (*env)->GetMethodID(env, clsFile, "getPath",
				 "()Ljava/lang/String;") ;
      (*env)->DeleteLocalRef (env, clsFile) ;
      jstrPath = (*env)->CallObjectMethod (env, objFile, midToString) ;

      if (jstrPath != NULL)
	{
	  tstrPath = jstr2tchar (env, jstrPath) ;
	  (*env)->DeleteLocalRef (env, jstrPath) ;
	}
      (*env)->DeleteLocalRef (env, objFile) ;
    }
  return (tstrPath) ;
}

OFC_FST_TYPE map_get_type (JNIEnv *env, jobject objMap)
{
  jmethodID midGetType ;
  jmethodID midToString ;
  jobject objMapType ;
  jstring jstrValue ;
  const char *szValue ;

  OFC_FST_TYPE fsType ;
  jclass clsMap ;
  jclass clsMapType ;
  
  clsMap = (*env)->FindClass
    (env, "com/connectedway/io/Framework$Map") ;
  midGetType = 
    (*env)->GetMethodID(env, clsMap, "getType", 
			"()Lcom/connectedway/io/Framework$mapType;") ;
  (*env)->DeleteLocalRef (env, clsMap) ;
  
  objMapType = (*env)->CallObjectMethod(env, objMap, midGetType) ;
  /*
   * Translate enum value from java to C string
   */
  clsMapType = (*env)->FindClass
    (env, "com/connectedway/io/Framework$mapType") ;
  midToString = (*env)->GetMethodID(env, clsMapType, "toString", 
				    "()Ljava/lang/String;") ;
  (*env)->DeleteLocalRef (env, clsMapType) ;

  fsType = OFC_FST_UNKNOWN ;
  if (objMapType != NULL)
    {
      jstrValue = (*env)->CallObjectMethod(env, objMapType, midToString) ;
      szValue = (*env)->GetStringUTFChars(env, jstrValue, NULL) ;
      /*
       * Now convert the C string to a C enum
       */
      if (ofc_strcmp (szValue, "WIN32") == 0)
	fsType = OFC_FST_WIN32 ;
      else if (ofc_strcmp (szValue, "DARWIN") == 0)
	fsType = OFC_FST_DARWIN ;
      else if (ofc_strcmp (szValue, "LINUX") == 0)
	fsType = OFC_FST_LINUX ;
      else if (ofc_strcmp (szValue, "FILEX") == 0)
	fsType = OFC_FST_FILEX ;
      else if (ofc_strcmp (szValue, "NUFILE") == 0)
	fsType = OFC_FST_NUFILE ;
      else if (ofc_strcmp (szValue, "ANDROID") == 0)
	fsType = OFC_FST_ANDROID ;
      else if (ofc_strcmp (szValue, "OTHER") == 0)
	fsType = OFC_FST_OTHER ;
      else if (ofc_strcmp (szValue, "SMB") == 0)
	fsType = OFC_FST_SMB ;
      else if (ofc_strcmp (szValue, "PIPE") == 0)
	fsType = OFC_FST_PIPE ;
      else if (ofc_strcmp (szValue, "MAILSLOT") == 0)
	fsType = OFC_FST_MAILSLOT ;
      else if (ofc_strcmp (szValue, "ROOT") == 0)
	fsType = OFC_FST_BROWSE_WORKGROUPS ;
      else if (ofc_strcmp (szValue, "WORKGROUP") == 0)
	fsType = OFC_FST_BROWSE_SERVERS ;
      else if (ofc_strcmp (szValue, "SERVER") == 0)
	fsType = OFC_FST_BROWSE_SHARES ;
      else if (ofc_strcmp (szValue, "BOOKMARK") == 0)
	fsType = OFC_FST_BOOKMARKS ;
      else if (ofc_strcmp (szValue, "UNKNOWN") == 0)
	fsType = OFC_FST_UNKNOWN ;
  
      (*env)->ReleaseStringUTFChars(env, jstrValue, szValue) ;
      (*env)->DeleteLocalRef (env, objMapType) ;
      (*env)->DeleteLocalRef (env, jstrValue) ;
    }
  return (fsType) ;
}

static OFC_VOID map_free_map (OFC_FRAMEWORK_MAP *map)
{
  ofc_free (map->prefix) ;
  ofc_free (map->desc) ;
  ofc_free (map->path) ;
}

jobject new_map_type (JNIEnv *env, OFC_FST_TYPE fsType)
{
  jobject objMapType ;
  jstring jstrMapType ;
  const char *szMapType ;

  jmethodID midValueOf ;
  jclass clsMapType ;

  switch (fsType)
    {
    case OFC_FST_WIN32:
      szMapType = "WIN32" ;
      break ;

    case OFC_FST_DARWIN:
      szMapType = "DARWIN" ;
      break ;

    case OFC_FST_LINUX:
      szMapType = "LINUX" ;
      break ;

    case OFC_FST_FILEX:
      szMapType = "FILEX" ;
      break ;

    case OFC_FST_NUFILE:
      szMapType = "NUFILE" ;
      break ;

    case OFC_FST_ANDROID:
      szMapType = "ANDROID" ;
      break ;

    case OFC_FST_OTHER:
      szMapType = "OTHER" ;
      break ;

    case OFC_FST_SMB:
      szMapType = "SMB" ;
      break ;

    case OFC_FST_FILE:
      szMapType = "FILE" ;
      break ;

    case OFC_FST_PIPE:
      szMapType = "PIPE" ;
      break ;

    case OFC_FST_MAILSLOT:
      szMapType = "MAILSLOT" ;
      break ;

    case OFC_FST_BROWSE_WORKGROUPS:
      szMapType = "ROOT" ;
      break ;

    case OFC_FST_BROWSE_SERVERS:
      szMapType = "WORKGROUP" ;
      break ;

    case OFC_FST_BROWSE_SHARES:
      szMapType = "SERVER" ;
      break ;

    case OFC_FST_BOOKMARKS:
      szMapType = "BOOKMARK" ;
      break ;

    case OFC_FST_UNKNOWN:
    default:
      szMapType = "UNKNOWN" ;
      break ;
    }

  jstrMapType = (*env)->NewStringUTF (env, szMapType) ;

  clsMapType = (*env)->FindClass
    (env, "com/connectedway/io/Framework$mapType") ;
  midValueOf = (*env)->GetStaticMethodID
    (env, clsMapType, "valueOf",
     "(Ljava/lang/String;)Lcom/connectedway/io/Framework$mapType;") ;

  objMapType = 
    (*env)->CallStaticObjectMethod(env, clsMapType,
				   midValueOf, jstrMapType) ;
  (*env)->DeleteLocalRef (env, clsMapType) ;
  (*env)->DeleteLocalRef (env, jstrMapType) ;

  return (objMapType) ;
}
	  
jobject new_map (JNIEnv *env, jobject objFramework,
		OFC_FRAMEWORK_MAP *map)
{
  jstring jstrName ;
  jstring jstrDesc ;
  jobject objFile ;
  jobject objMap ;
  jboolean zThumbnail ;

  jmethodID midNewMap ;

  jclass clsBlueFramework ;

  jobject objMapType ;

  jstrName = tchar2jstr (env, map->prefix) ;
  jstrDesc = tchar2jstr (env, map->desc) ;

  zThumbnail = map->thumbnail ? JNI_TRUE : JNI_FALSE ;

  objFile = new_file_from_path (env, map->path) ;

  clsBlueFramework = (*env)->FindClass
    (env, "com/connectedway/io/Framework") ;

  objMapType = new_map_type (env, map->type) ;

  midNewMap = (*env)->GetMethodID 
    (env, clsBlueFramework, "newMap",
     "(Ljava/lang/String;Ljava/lang/String;Lcom/connectedway/io/File;Lcom/connectedway/io/Framework$mapType;Z)Lcom/connectedway/io/Framework$Map;") ;

  (*env)->DeleteLocalRef (env, clsBlueFramework) ;

  objMap = (*env)->CallObjectMethod (env, objFramework, 
				     midNewMap, jstrName, jstrDesc, objFile, 
				     objMapType, zThumbnail) ;
  (*env)->DeleteLocalRef (env, objFile) ;
  (*env)->DeleteLocalRef (env, jstrName) ;
  (*env)->DeleteLocalRef (env, jstrDesc) ;

  return (objMap) ;
}

jobjectArray new_maps (JNIEnv *env, 
		      jobject objFramework,
		      OFC_FRAMEWORK_MAPS *maps)
{
  jobjectArray jmaps ;
  jobject objMap ;
  int i ;
  jclass clsMap ;

  clsMap = (*env)->FindClass
    (env, "com/connectedway/io/Framework$Map") ;

  jmaps = (*env)->NewObjectArray (env, maps->numMaps, clsMap, NULL) ;
  (*env)->DeleteLocalRef (env, clsMap) ;  

  for (i = 0 ; i < maps->numMaps ; i++)
    {
      objMap = new_map (env, objFramework, &maps->map[i]) ;
      (*env)->SetObjectArrayElement (env, jmaps, i, objMap) ;
    }

  return (jmaps) ;
}

JNIEXPORT jboolean JNICALL Java_com_connectedway_io_Framework_setLMB
(JNIEnv *env, jobject objFramework, jobject objInetAddress, jstring lmb)
{
  OFC_IPADDR ipaddr ;
  OFC_INT i ;
  OFC_IPADDR if_ip ;
  OFC_IPADDR if_mask ;
  OFC_CCHAR *strLmb ;
  jboolean ret ;
  OFC_CHAR ip_addr[IP6STR_LEN] ;
  
  ret = JNI_FALSE ;
  inet_address_to_ipaddr (env, objInetAddress, &ipaddr) ;
  strLmb = (*env)->GetStringUTFChars (env, lmb, NULL) ;

  for (i = 0 ; i < ofc_persist_interface_count() && ret == JNI_FALSE ; i++)
    {
      ofc_persist_interface_addr (i, &if_ip, OFC_NULL, &if_mask) ;
      if (ofc_net_subnet_match (&ipaddr, &if_ip, &if_mask))
	{
	  ofc_persist_set_local_master (i, strLmb) ;
	  ofc_printf ("Local Master Browser for Interface %s Set to %s\n", 
		       ofc_ntop (&if_ip, ip_addr, IP6STR_LEN), strLmb) ;
	  ret = JNI_TRUE ;
	}
    }
  
  (*env)->ReleaseStringUTFChars(env, lmb, strLmb) ;
  return (ret) ;
}
  
/*
 * Class:     com_connectedway_io_Framework
 * Method:    addInterface
 * Signature: (Lcom/connectedway/io/Framework$Interface;)V
 */
JNIEXPORT void JNICALL Java_com_connectedway_io_Framework_addInterface
(JNIEnv *env, jobject objFramework, jobject objInterface)
{
  OFC_FRAMEWORK_INTERFACE iface ;

  iface.netBiosMode = interface_get_netbios_mode (env, objInterface) ;
  interface_get_ip_address (env, objInterface, &iface.ip) ;
  interface_get_bcast_address (env, objInterface, &iface.bcast) ;
  interface_get_mask_address (env, objInterface, &iface.mask) ;
  iface.lmb = interface_get_lmb (env, objInterface) ;
  interface_get_wins (env, objInterface, &iface.wins) ;
  ofc_framework_add_interface(&iface) ;
  /*
   * Clean up what we allocated
   */
  interface_free_lmb ((OFC_LPSTR) iface.lmb) ;
  interface_free_wins (&iface.wins) ;
}

/*
 * Class:     com_connectedway_io_Framework
 * Method:    removeInterface
 * Signature: (Ljava/net/InetAddress;)V
 */
JNIEXPORT void JNICALL Java_com_connectedway_io_Framework_removeInterface
  (JNIEnv *env, jobject objFramework, jobject objInetAddress)
{
  OFC_IPADDR ip ;

  inet_address_to_ipaddr (env, objInetAddress, &ip) ;
  ofc_framework_remove_interface (&ip) ;
}

jobject new_netbios_mode (JNIEnv *env, OFC_CONFIG_MODE netBiosMode)
{
  jobject objNetBIOSMode ;
  jstring jstrNetBIOSMode ;
  const char *szNetBIOSMode ;
  jmethodID midValueOf ;
  jclass clsNetBIOSMode ;

  switch (netBiosMode)
    {
    default:
    case OFC_CONFIG_BMODE:
      szNetBIOSMode = "BMODE" ;
      break ;

    case OFC_CONFIG_PMODE:
      szNetBIOSMode = "PMODE" ;
      break ;

    case OFC_CONFIG_MMODE:
      szNetBIOSMode = "MMODE" ;
      break ;

    case OFC_CONFIG_HMODE:
      szNetBIOSMode = "HMODE" ;
      break ;
    }

  jstrNetBIOSMode = (*env)->NewStringUTF (env, szNetBIOSMode) ;

  clsNetBIOSMode = (*env)->FindClass
    (env, "com/connectedway/io/Framework$netBIOSMode") ;
  midValueOf = (*env)->GetStaticMethodID
    (env, clsNetBIOSMode, "valueOf",
     "(Ljava/lang/String;)Lcom/connectedway/io/Framework$netBIOSMode;") ;
  objNetBIOSMode = (*env)->CallStaticObjectMethod(env, clsNetBIOSMode,
						  midValueOf, jstrNetBIOSMode) ;
  (*env)->DeleteLocalRef (env, clsNetBIOSMode) ;
  (*env)->DeleteLocalRef (env, jstrNetBIOSMode) ;

  return (objNetBIOSMode) ;
}
	  
jobject new_inet_address (JNIEnv *env, OFC_IPADDR *ip)
{
  jobject objInetAddress ;
  jbyteArray bInetArray ;
  jbyte bInetAddress[4] ;
  jmethodID midGetByAddress ;
  jclass clsInetAddress ;

  bInetAddress[0] = (ip->u.ipv4.addr >> 24) & 0xFF ;
  bInetAddress[1] = (ip->u.ipv4.addr >> 16) & 0xFF ;
  bInetAddress[2] = (ip->u.ipv4.addr >> 8) & 0xFF ;
  bInetAddress[3] = (ip->u.ipv4.addr >> 0) & 0xFF ;
  
  bInetArray = (*env)->NewByteArray(env, 4) ;
  (*env)->SetByteArrayRegion (env, bInetArray, 0, 4, bInetAddress) ;

  clsInetAddress = (*env)->FindClass 
    (env, "java/net/InetAddress") ;
  midGetByAddress = (*env)->GetStaticMethodID
    (env, clsInetAddress, "getByAddress", 
     "([B)Ljava/net/InetAddress;") ;
  objInetAddress = 
    (*env)->CallStaticObjectMethod (env, clsInetAddress, 
				    midGetByAddress, bInetArray) ;
  (*env)->DeleteLocalRef (env, clsInetAddress) ;
  return (objInetAddress) ;
}

jobjectArray new_wins (JNIEnv *env, OFC_FRAMEWORK_WINSLIST *wins)
{
  jobjectArray arrayWins ;
  jobject objInetAddress ;
  int i ;
  jclass clsInetAddress ;

  clsInetAddress = (*env)->FindClass 
    (env, "java/net/InetAddress") ;
  arrayWins = (*env)->NewObjectArray (env, wins->num_wins,
				      clsInetAddress, NULL) ;
  (*env)->DeleteLocalRef (env, clsInetAddress) ;

  for (i = 0 ; i < wins->num_wins ; i++)
    {
      objInetAddress = new_inet_address (env, &wins->winsaddr[i]) ;
      (*env)->SetObjectArrayElement (env, arrayWins, i, objInetAddress) ;
      (*env)->DeleteLocalRef (env, objInetAddress) ;
    }
  return (arrayWins) ;
}

jobject new_interface (JNIEnv *env, jobject objFramework,
		      OFC_FRAMEWORK_INTERFACE *iface)
{
  jobjectArray arrayWins ;
  jstring jstrLmb ;
  jobject objNetBiosMode ;
  jobject objBcastAddress ;
  jobject objMaskAddress ;
  jobject objInetAddress ;
  jobject objInterface ;

  jmethodID midNewInterface ;
  jclass clsBlueFramework ;

  objNetBiosMode = new_netbios_mode (env, iface->netBiosMode) ;
  objInetAddress = new_inet_address (env, &iface->ip) ;
  objBcastAddress = new_inet_address (env, &iface->bcast) ;
  objMaskAddress = new_inet_address (env, &iface->mask) ;
  jstrLmb = (*env)->NewStringUTF (env, iface->lmb) ;
  arrayWins = new_wins (env, &iface->wins) ;

  clsBlueFramework = (*env)->GetObjectClass (env, objFramework) ;

  midNewInterface = (*env)->GetMethodID
    (env, clsBlueFramework, "newInterface",
     "(Lcom/connectedway/io/Framework$netBIOSMode;Ljava/net/InetAddress;Ljava/net/InetAddress;Ljava/net/InetAddress;Ljava/lang/String;[Ljava/net/InetAddress;)Lcom/connectedway/io/Framework$Interface;") ; 
  (*env)->DeleteLocalRef (env, clsBlueFramework) ;

  objInterface = (*env)->CallObjectMethod (env, objFramework, midNewInterface,
					   objNetBiosMode, 
					   objInetAddress, objBcastAddress, 
					   objMaskAddress, jstrLmb, arrayWins) ;
  (*env)->DeleteLocalRef (env, objNetBiosMode) ;
  (*env)->DeleteLocalRef (env, objInetAddress) ;
  (*env)->DeleteLocalRef (env, objBcastAddress) ;
  (*env)->DeleteLocalRef (env, objMaskAddress) ;
  (*env)->DeleteLocalRef (env, jstrLmb) ;
  (*env)->DeleteLocalRef (env, arrayWins) ;

  return (objInterface) ;
}

jobjectArray new_interfaces (JNIEnv *env, 
			    jobject objFramework,
			    OFC_FRAMEWORK_INTERFACES *interfaces)
{
  jobjectArray jinterfaces ;
  jobject objInterface ;
  int i ;
  jclass clsInterface ;

  clsInterface = (*env)->FindClass
    (env, "com/connectedway/io/Framework$Interface") ;

  jinterfaces = (*env)->NewObjectArray (env, interfaces->num_interfaces,
					clsInterface, NULL) ;
  (*env)->DeleteLocalRef (env, clsInterface) ;

  for (i = 0 ; i < interfaces->num_interfaces ; i++)
    {
      objInterface = new_interface (env, objFramework, 
				   &interfaces->iface[i]) ;
      (*env)->SetObjectArrayElement (env, jinterfaces, i, objInterface) ;
      (*env)->DeleteLocalRef (env, objInterface) ;
    }
  return (jinterfaces) ;
}

/*
 * Class:     com_connectedway_io_Framework
 * Method:    getInterfaces
 * Signature: ()[Lcom/connectedway/io/Framework$Interface;
 */
JNIEXPORT jobjectArray JNICALL Java_com_connectedway_io_Framework_getInterfaces
  (JNIEnv *env, jobject objFramework)
{
  OFC_FRAMEWORK_INTERFACES *interfaces ;
  jobjectArray jinterfaces ;

  jinterfaces = OFC_NULL ;
  interfaces = ofc_framework_get_interfaces() ;
  if (interfaces != OFC_NULL)
    {
      jinterfaces = new_interfaces (env, objFramework, interfaces) ;
      ofc_framework_free_interfaces(interfaces) ;
    }

  return (jinterfaces) ;
}


/*
 * Class:     com_connectedway_io_Framework
 * Method:    addMap
 * Signature: (Lcom/connectedway/io/Framework$Map;)Z
  */
JNIEXPORT jboolean JNICALL Java_com_connectedway_io_Framework_addMap
  (JNIEnv *env, jobject objFramework, jobject objMap)
{
  OFC_FRAMEWORK_MAP map ;
  jboolean jret ;

  jret = JNI_FALSE ;
 
  map.prefix = map_get_name (env, objMap) ;
  map.path = map_get_path (env, objMap) ;
  map.type = map_get_type (env, objMap) ;
  map.desc = map_get_desc (env, objMap) ;
  map.thumbnail = map_get_thumbnail (env, objMap) ;
 
  if (ofc_framework_add_map(&map) == OFC_TRUE)
    jret = JNI_TRUE;
 
  map_free_map (&map) ;
  return (jret);
}
 
/*
 * Class:     com_connectedway_io_Framework
 * Method:    getMaps
 * Signature: ()[Lcom/connectedway/io/Framework$Map;
 */
JNIEXPORT jobjectArray JNICALL Java_com_connectedway_io_Framework_getMaps
   (JNIEnv *env, jobject objFramework)
{
  OFC_FRAMEWORK_MAPS *maps ;
  jobjectArray jmaps ;
 
  jmaps = OFC_NULL ;
  maps = ofc_framework_get_maps() ;
  if (maps != OFC_NULL)
    {
      jmaps = new_maps (env, objFramework, maps) ;
    }

  ofc_framework_free_maps(maps) ;
  return (jmaps) ;
}
 
/*
 * Class:     com_connectedway_io_Framework
 * Method:    removeMap
 * Signature: (Ljava/lang/String;)V
 */
JNIEXPORT void JNICALL Java_com_connectedway_io_Framework_removeMap
  (JNIEnv *env, jobject objFramework, jstring prefix)
{
  OFC_LPTSTR tstrPrefix ;
 
  tstrPrefix = jstr2tchar (env, prefix) ;
 
  ofc_framework_remove_map(tstrPrefix) ;
  ofc_free (tstrPrefix) ;
}


/*
 * Class:     com_connectedway_io_Framework
 * Method:    update
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_com_connectedway_io_Framework_update
  (JNIEnv *env, jobject objFramework)
{
  ofc_framework_update() ;
}


static jmethodID g_method = 0 ;
static jobject g_interface = OFC_NULL ;
static JavaVM* g_jvm = OFC_NULL ;
static jclass g_serverevent = OFC_NULL ;
static jmethodID g_midvalueof = 0 ;

static JNIEnv *get_env() 
{
  JNIEnv *env;
  int status ;
  /* ok, ugly but the Java environment on Darwin has a slightly incorrect
   * prototype.  
   */
#if defined(__ANDROID__) || defined(ANDROID)
  JNIEnv *envx;
#elif defined(__APPLE__) || defined(__linux__)
  void *envx ;
#else
  JNIEnv *envx;
#endif

  if (g_jvm != OFC_NULL)
    {
      status = (*g_jvm)->GetEnv(g_jvm, (void **) &env, JNI_VERSION_1_6);
      if (status < 0) 
	{
	  status = (*g_jvm)->AttachCurrentThread (g_jvm, &envx, NULL);
	  if (status < 0)
	    env = OFC_NULL ;
	  else
	    env = (JNIEnv *) envx ;
	}
    }
  else
    env = OFC_NULL ;
  return (env) ;
}

/*
 * Class:     com_connectedway_io_Framework
 * Method:    println
 * Signature: (Ljava/lang/String;)V
 */
JNIEXPORT void JNICALL Java_com_connectedway_io_Framework_println
  (JNIEnv *env, jobject objFramework, jstring out)
{
  OFC_LPTSTR tstrout ;

  tstrout = jstr2tchar (env, out) ;

  ofc_printf ("%s: %S\n", __func__, tstrout) ;
  ofc_free (tstrout) ;

}

JNIEXPORT void JNICALL Java_com_connectedway_io_Framework_setInterfaceFilter
(JNIEnv *env, jobject objFramework, jint ip)
{
  ofc_framework_set_wifi_ip(ip) ;
}
  
JNIEXPORT void JNICALL Java_com_connectedway_io_Framework_dumpHeap
(JNIEnv *env, jobject objFramework)
{
  ofc_framework_dump_heap() ;
}
  
JNIEXPORT void JNICALL Java_com_connectedway_io_Framework_statsHeap
(JNIEnv *env, jobject objFramework)
{
  ofc_framework_stats_heap() ;
}
  
