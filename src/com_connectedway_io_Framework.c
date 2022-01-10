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
  ofc_printf ("libanddroidsmb loaded at 0x%08x\n", get_library_address()) ;
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


#if 0
OFC_CONFIG_AUTH_MODE 
authentication_get_mode (JNIEnv *env, jobject objAuthenticationMode)
{
  jmethodID midToString ;
  jstring jstrValue ;
  const char *szValue ;

  OFC_CONFIG_AUTH_MODE mode ;
  jclass clsAuthenticationMode ;
  
  /*
   * Translate enum value from java to C string
   */
  clsAuthenticationMode = (*env)->FindClass 
    (env, "com/connectedway/io/Framework$authenticationMode") ;
  midToString = (*env)->GetMethodID(env, clsAuthenticationMode, "toString", 
				"()Ljava/lang/String;") ;
  (*env)->DeleteLocalRef (env, clsAuthenticationMode) ;

  jstrValue = (*env)->CallObjectMethod(env, objAuthenticationMode, 
				       midToString) ;
  /*
   * Make default user so if something is wrong, we have the highest level
   * of authentication by default.
   */
  mode = OFC_CONFIG_AUTH_MODE_USER ;

  if (jstrValue != NULL)
    {
      szValue = (*env)->GetStringUTFChars(env, jstrValue, NULL) ;
      /*
       * Now convert the C string to a C enum
       */
      if (ofc_strcmp (szValue, "DISABLED") == 0)
	mode = OFC_CONFIG_AUTH_MODE_DISABLED ;
      else if (ofc_strcmp (szValue, "SERVER") == 0)
	mode = OFC_CONFIG_AUTH_MODE_SERVER ;
      else if (ofc_strcmp (szValue, "USER") == 0)
	mode = OFC_CONFIG_AUTH_MODE_USER ;
      
      (*env)->ReleaseStringUTFChars(env, jstrValue, szValue) ;
      (*env)->DeleteLocalRef (env, jstrValue) ;
    }

  return (mode) ;
}

jobject new_authentication_mode (JNIEnv *env, OFC_CONFIG_AUTH_MODE mode)
{
  jobject objAuthenticationMode ;
  jstring jstrAuthenticationMode ;
  const char *szAuthenticationMode ;

  jmethodID midValueOf ;
  jclass clsAuthenticationMode ;

  switch (mode)
    {
    case OFC_CONFIG_AUTH_MODE_DISABLED:
      szAuthenticationMode = "DISABLED" ;
      break ;

    case OFC_CONFIG_AUTH_MODE_SERVER:
      szAuthenticationMode = "SERVER" ;
      break ;

    default:
    case OFC_CONFIG_AUTH_MODE_USER:
      szAuthenticationMode = "USER" ;
      break ;

    }

  jstrAuthenticationMode = (*env)->NewStringUTF (env, szAuthenticationMode) ;

  clsAuthenticationMode = (*env)->FindClass 
    (env, "com/connectedway/io/Framework$authenticationMode") ;
  midValueOf = (*env)->GetStaticMethodID
    (env, clsAuthenticationMode, "valueOf",
     "(Ljava/lang/String;)Lcom/connectedway/io/Framework$authenticationMode;") ;
  objAuthenticationMode = 
    (*env)->CallStaticObjectMethod(env, clsAuthenticationMode,
				   midValueOf, jstrAuthenticationMode) ;
  (*env)->DeleteLocalRef (env, clsAuthenticationMode) ;
  (*env)->DeleteLocalRef (env, jstrAuthenticationMode) ;

  return (objAuthenticationMode) ;
}
	  
OFC_CONFIG_EXPORT_TYPE export_get_export_type (JNIEnv *env, jobject objExport)
{
  jmethodID midGetShareType ;
  jmethodID midToString ;
  jobject objShareType ;
  jstring jstrValue ;
  const char *szValue ;

  OFC_CONFIG_EXPORT_TYPE exportType ;
  jclass clsExport ;
  jclass clsShareType ;
  
  clsExport = (*env)->FindClass
    (env, "com/connectedway/io/Framework$Export") ;
  midGetShareType = 
    (*env)->GetMethodID(env, clsExport, "getShareType", 
			"()Lcom/connectedway/io/Framework$shareType;") ;
  (*env)->DeleteLocalRef (env, clsExport) ;
  
  objShareType = (*env)->CallObjectMethod(env, objExport, midGetShareType) ;
  /*
   * Translate enum value from java to C string
   */
  clsShareType = (*env)->FindClass
    (env, "com/connectedway/io/Framework$shareType") ;
  midToString = (*env)->GetMethodID(env, clsShareType, "toString", 
				    "()Ljava/lang/String;") ;
  (*env)->DeleteLocalRef (env, clsShareType) ;

  exportType = OFC_CONFIG_EXPORT_DISK ;
  if (objShareType != NULL)
    {
      jstrValue = (*env)->CallObjectMethod(env, objShareType, midToString) ;
      szValue = (*env)->GetStringUTFChars(env, jstrValue, NULL) ;
      /*
       * Now convert the C string to a C enum
       */
      if (ofc_strcmp (szValue, "DISK") == 0)
	exportType = OFC_CONFIG_EXPORT_DISK ;
      else if (ofc_strcmp (szValue, "PRINTER") == 0)
	exportType = OFC_CONFIG_EXPORT_PRINTER ;
      else if (ofc_strcmp (szValue, "PIPE") == 0)
	exportType = OFC_CONFIG_EXPORT_PIPE ;
      else if (ofc_strcmp (szValue, "COMM") == 0)
	exportType = OFC_CONFIG_EXPORT_COMM ;
      else if (ofc_strcmp (szValue, "DEVICE") == 0)
	exportType = OFC_CONFIG_EXPORT_DEVICE ;
  
      (*env)->ReleaseStringUTFChars(env, jstrValue, szValue) ;
      (*env)->DeleteLocalRef (env, objShareType) ;
      (*env)->DeleteLocalRef (env, jstrValue) ;
    }
  return (exportType) ;
}

OFC_LPTSTR export_get_share (JNIEnv *env, jobject objExport)
{
  jmethodID midGetShare ;

  jstring jstrShare ;
  OFC_LPTSTR tstrShare ;
  jclass clsExport ;

  /*
   * Now get local master browser
   */
  clsExport = (*env)->FindClass
    (env, "com/connectedway/io/Framework$Export") ;
  midGetShare = (*env)->GetMethodID(env, clsExport, "getShare", 
					 "()Ljava/lang/String;") ;
  (*env)->DeleteLocalRef (env, clsExport) ;
  jstrShare = (*env)->CallObjectMethod(env, objExport, midGetShare) ;

  tstrShare = OFC_NULL ;
  if (jstrShare != NULL)
    tstrShare = jstr2tchar (env, jstrShare) ;
  (*env)->DeleteLocalRef (env, jstrShare) ;

  return (tstrShare) ;
}

OFC_LPTSTR export_get_path (JNIEnv *env, jobject objExport)
{
  jmethodID midGetPath ;
  jmethodID midToString ;

  jobject objFile ;

  jstring jstrPath ;
  OFC_LPTSTR tstrPath ;
  jclass clsBlueFile ;
  jclass clsExport ;

  tstrPath = OFC_NULL ;

  clsExport = (*env)->FindClass
    (env, "com/connectedway/io/Framework$Export") ;
  midGetPath = (*env)->GetMethodID(env, clsExport, "getPath", 
				   "()Lcom/connectedway/io/File;") ;
  (*env)->DeleteLocalRef (env, clsExport) ;

  objFile = (*env)->CallObjectMethod(env, objExport, midGetPath) ;

  clsBlueFile = (*env)->FindClass
    (env, "com/connectedway/io/File") ;
  if (objFile != NULL)
    {
      midToString = (*env)->GetMethodID(env, clsBlueFile, "toString",
					"()Ljava/lang/String;") ;
      jstrPath = (*env)->CallObjectMethod (env, objFile, midToString) ;

      if (jstrPath != NULL)
	{
	  OFC_TCHAR *tstrPath2 ;
	  OFC_SIZET len ;
	  tstrPath = jstr2tchar (env, jstrPath) ;
	  /* append "/" to end of it */
	  len = ofc_tstrlen(tstrPath) ;
	  tstrPath2 = ofc_malloc ((len+2)*sizeof(OFC_TCHAR)) ;
	  ofc_tstrncpy (tstrPath2, tstrPath, len) ;
	  ofc_free (tstrPath) ;
	  tstrPath = tstrPath2 ;
	  tstrPath[len] = TCHAR('/');
	  tstrPath[len+1] = TCHAR_EOS ;
	}
      (*env)->DeleteLocalRef (env, objFile) ;
      (*env)->DeleteLocalRef (env, jstrPath) ;
    }
  (*env)->DeleteLocalRef (env, clsBlueFile) ;

  return (tstrPath) ;
}

OFC_LPSTR proxy_gateway_get_path (JNIEnv *env, jobject objProxy)
{
  jmethodID midGetPath ;

  jmethodID midToString ;

  jobject objFile ;

  jstring jstrPath ;
  OFC_CHAR *cstrPath ;
  const OFC_CHAR *szPath ;
  jclass clsBlueFile ;
  jclass clsProxy ;

  cstrPath = OFC_NULL ;

  clsProxy = (*env)->FindClass
    (env, "com/connectedway/io/Framework$ProxyGateway") ;
  midGetPath = (*env)->GetMethodID(env, clsProxy, "getPath", 
				   "()Lcom/connectedway/io/File;") ;
  (*env)->DeleteLocalRef (env, clsProxy) ;

  objFile = (*env)->CallObjectMethod(env, objProxy, midGetPath) ;

  clsBlueFile = (*env)->FindClass
    (env, "com/connectedway/io/File") ;

  cstrPath = OFC_NULL ;
  if (objFile != NULL)
    {
      midToString = (*env)->GetMethodID(env, clsBlueFile, "toString",
					"()Ljava/lang/String;") ;

      jstrPath = (*env)->CallObjectMethod (env, objFile, midToString) ;

      if (jstrPath != NULL)
	{
	  szPath = (*env)->GetStringUTFChars (env, jstrPath, NULL) ;
	  cstrPath = ofc_strdup (szPath) ;
	  (*env)->ReleaseStringUTFChars (env, jstrPath, szPath) ;
	}
      (*env)->DeleteLocalRef (env, objFile) ;
      (*env)->DeleteLocalRef (env, jstrPath) ;
    }
  (*env)->DeleteLocalRef (env, clsBlueFile) ;
  return (cstrPath) ;
}

OFC_LPTSTR export_get_description (JNIEnv *env, jobject objExport)
{
  jmethodID midGetComment ;

  jstring jstrComment ;
  OFC_LPTSTR tstrComment ;
  jclass clsExport ;

  clsExport = (*env)->FindClass
    (env, "com/connectedway/io/Framework$Export") ;
  midGetComment = (*env)->GetMethodID(env, clsExport, "getComment", 
				      "()Ljava/lang/String;") ;
  (*env)->DeleteLocalRef (env, clsExport) ;

  jstrComment = (*env)->CallObjectMethod(env, objExport, midGetComment) ;

  tstrComment = OFC_NULL ;
  if (jstrComment != NULL)
    {
      tstrComment = jstr2tchar (env, jstrComment) ;
      (*env)->DeleteLocalRef (env, jstrComment) ;
    }
  return (tstrComment) ;
}

OFC_LPTSTR export_get_native_fs (JNIEnv *env, jobject objExport)
{
  jmethodID midGetFsName ;

  jstring jstrFsName ;
  OFC_LPTSTR tstrFsName ;
  jclass clsExport ;

  clsExport = (*env)->FindClass
    (env, "com/connectedway/io/Framework$Export") ;
  midGetFsName = (*env)->GetMethodID(env, clsExport, "getFsName", 
				      "()Ljava/lang/String;") ;
  (*env)->DeleteLocalRef (env, clsExport) ;
  jstrFsName = (*env)->CallObjectMethod(env, objExport, midGetFsName) ;

  tstrFsName = OFC_NULL ;
  if (jstrFsName != NULL)
    {
      tstrFsName = jstr2tchar (env, jstrFsName) ;
      (*env)->DeleteLocalRef (env, jstrFsName) ;
    }

  return (tstrFsName) ;
}

OFC_VOID export_free_share (OFC_LPTSTR str)
{
  ofc_free (str) ;
}

OFC_VOID export_free_path (OFC_LPTSTR str)
{
  ofc_free (str) ;
}

OFC_VOID proxy_gateway_free_path (OFC_LPSTR str)
{
  ofc_free (str) ;
}

OFC_VOID export_free_description (OFC_LPTSTR str)
{
  ofc_free (str) ;
}
OFC_VOID export_free_native_fs (OFC_LPTSTR str)
{
  ofc_free (str) ;
}
#endif

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

#if 0
jobject new_share_type (JNIEnv *env, OFC_CONFIG_EXPORT_TYPE export_type)
{
  jobject objShareType ;
  jstring jstrShareType ;
  const char *szShareType ;

  jmethodID midValueOf ;
  jclass clsShareType ;

  switch (export_type)
    {
    default:
    case OFC_CONFIG_EXPORT_DISK:
      szShareType = "DISK" ;
      break ;

    case OFC_CONFIG_EXPORT_PRINTER:
      szShareType = "PRINTER" ;
      break ;

    case OFC_CONFIG_EXPORT_PIPE:
      szShareType = "PIPE" ;
      break ;

    case OFC_CONFIG_EXPORT_COMM:
      szShareType = "COMM" ;
      break ;

    case OFC_CONFIG_EXPORT_DEVICE:
      szShareType = "DEVICE" ;
      break ;
    }

  jstrShareType = (*env)->NewStringUTF (env, szShareType) ;

  clsShareType = (*env)->FindClass
    (env, "com/connectedway/io/Framework$shareType") ;
  midValueOf = (*env)->GetStaticMethodID
    (env, clsShareType, "valueOf",
     "(Ljava/lang/String;)Lcom/connectedway/io/Framework$shareType;") ;

  objShareType = 
    (*env)->CallStaticObjectMethod(env, clsShareType,
				   midValueOf, jstrShareType) ;
  (*env)->DeleteLocalRef (env, clsShareType) ;
  (*env)->DeleteLocalRef (env, jstrShareType) ;

  return (objShareType) ;
}
	  
jobject new_export (JNIEnv *env, jobject objFramework,
		   OFC_FRAMEWORK_EXPORT *export)
{
  jobject objShareType ;
  jstring jstrShare ;
  jobject objFile ;
  jstring jstrComment ;
  jstring jstrFsName ;
  jobject objExport ;

  jmethodID midNewExport ;

  jclass clsBlueFramework ;

  jstrShare = tchar2jstr (env, export->share) ;
  jstrComment = tchar2jstr (env, export->description) ;
  jstrFsName = tchar2jstr (env, export->native_fs) ;

  objShareType = new_share_type (env, export->exportType) ;
  objFile = new_file_from_path (env, export->path) ;

  clsBlueFramework = (*env)->GetObjectClass (env, objFramework) ;

  midNewExport = (*env)->GetMethodID 
    (env, clsBlueFramework, "newExport",
     "(Lcom/connectedway/io/Framework$shareType;Ljava/lang/String;Lcom/connectedway/io/File;Ljava/lang/String;Ljava/lang/String;)Lcom/connectedway/io/Framework$Export;") ;
  (*env)->DeleteLocalRef (env, clsBlueFramework) ;

  objExport = (*env)->CallObjectMethod (env, objFramework, midNewExport,
					objShareType, 
					jstrShare, objFile, jstrComment,
					jstrFsName) ;
  (*env)->DeleteLocalRef (env, objFile) ;
  (*env)->DeleteLocalRef (env, objShareType) ;
  (*env)->DeleteLocalRef (env, jstrShare) ;
  (*env)->DeleteLocalRef (env, jstrComment) ;
  (*env)->DeleteLocalRef (env, jstrFsName) ;

  return (objExport) ;
}

jobjectArray new_exports (JNIEnv *env, 
			 jobject objFramework,
			 OFC_FRAMEWORK_EXPORTS *exports)
{
  jobjectArray jexports ;
  jobject objExport ;
  int i ;
  jclass clsExport ;

  clsExport = (*env)->FindClass
    (env, "com/connectedway/io/Framework$Export") ;
  jexports = 
    (*env)->NewObjectArray (env, exports->numExports, clsExport, NULL) ;
  (*env)->DeleteLocalRef (env, clsExport) ;

  for (i = 0 ; i < exports->numExports ; i++)
    {
      objExport = new_export (env, objFramework, &exports->exp[i]) ;
      (*env)->SetObjectArrayElement (env, jexports, i, objExport) ;
      (*env)->DeleteLocalRef (env, objExport) ;
    }

  return (jexports) ;
}
#endif

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

#if 0
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
#endif
	  
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
#if 0
  jobject objNetBiosMode ;
#endif
  jobject objBcastAddress ;
  jobject objMaskAddress ;
  jobject objInetAddress ;
  jobject objInterface ;

  jmethodID midNewInterface ;
  jclass clsBlueFramework ;

#if 0
  objNetBiosMode = new_netbios_mode (env, iface->netBiosMode) ;
#endif
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

#if 0
  objInterface = (*env)->CallObjectMethod (env, objFramework, midNewInterface,
					   objNetBiosMode, 
					   objInetAddress, objBcastAddress, 
					   objMaskAddress, jstrLmb, arrayWins) ;
  (*env)->DeleteLocalRef (env, objNetBiosMode) ;
#endif
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

#if 0
/*
 * Class:     com_connectedway_io_Framework
 * Method:    setAuthenticationMode
 * Signature: (Lcom/connectedway/io/Framework$authenticationMode;)V
 */
JNIEXPORT void JNICALL Java_com_connectedway_io_Framework_setAuthenticationMode
  (JNIEnv *env, jobject objFramework, jobject objAuthenticationMode)
{
  OFC_CONFIG_AUTH_MODE authMode ;

  authMode = authentication_get_mode (env, objAuthenticationMode) ;
  ofc_framework_set_authentication_mode(authMode) ;
}

/*
 * Class:     com_connectedway_io_Framework
 * Method:    getAuthenticationMode
 * Signature: ()I
 */
JNIEXPORT jobject JNICALL 
Java_com_connectedway_io_Framework_getAuthenticationMode
  (JNIEnv *env, jobject objFramework)
{
  jobject objAuthenticationMode ;

  OFC_CONFIG_AUTH_MODE mode ;

  ofc_persist_auth_mode(&mode) ;

  objAuthenticationMode = new_authentication_mode (env, mode) ;

  return (objAuthenticationMode) ;
}

/*
 * Class:     com_connectedway_io_Framework
 * Method:    addExport
 * Signature: (Lcom/connectedway/io/Framework$Export;)V
 */
JNIEXPORT void JNICALL Java_com_connectedway_io_Framework_addExport
  (JNIEnv *env, jobject objFramework, jobject objExport)
{
  OFC_FRAMEWORK_EXPORT export ;

  export.exportType = export_get_export_type (env, objExport) ;
  export.share = export_get_share (env, objExport) ;
  export.path = export_get_path (env, objExport) ;
  export.description = export_get_description (env, objExport) ;
  export.native_fs = export_get_native_fs (env, objExport) ;

  ofc_framework_add_export(&export) ;
  /*
   * Clean up what we allocated
   */
  export_free_share (export.share) ;
  export_free_path (export.path) ;
  export_free_description (export.description) ;
  export_free_native_fs (export.native_fs) ;
}

/*
 * Class:     com_connectedway_io_Framework
 * Method:    addProxyGateway
 * Signature: (Lcom/connectedway/io/Framework$Export;)V
 */
JNIEXPORT void JNICALL Java_com_connectedway_io_Framework_addProxyGateway
  (JNIEnv *env, jobject objFramework, jobject objProxy)
{
  OFC_FRAMEWORK_PROXY proxy ;

  proxy.path = proxy_gateway_get_path (env, objProxy) ;

  ofc_framework_add_proxy_gateway(&proxy) ;
  /*
   * Clean up what we allocated
   */
  proxy_gateway_free_path (proxy.path) ;
}

/*
 * Class:     com_connectedway_io_Framework
 * Method:    getExports
 * Signature: ()[Lcom/connectedway/io/Framework$Export;
 */
JNIEXPORT jobjectArray JNICALL Java_com_connectedway_io_Framework_getExports
  (JNIEnv *env, jobject objFramework)
{
  OFC_FRAMEWORK_EXPORTS *exports ;
  jobjectArray jexports ;

  jexports = OFC_NULL ;
  exports = ofc_framework_get_exports() ;
  if (exports != OFC_NULL)
    {
      jexports = new_exports (env, objFramework, exports) ;
    }

  ofc_framework_free_exports(exports) ;
  return (jexports) ;
}

/*
 * Class:     com_connectedway_io_Framework
 * Method:    removeExport
 * Signature: (Ljava/lang/String;)V
 */
JNIEXPORT void JNICALL Java_com_connectedway_io_Framework_removeExport
  (JNIEnv *env, jobject objFramework, jstring jstrExport)
{
  OFC_LPTSTR tstrShare ;

  tstrShare = jstr2tchar (env, jstrExport) ;

  ofc_framework_remove_export(tstrShare) ;
  ofc_free (tstrShare) ;
}
#endif

/*
 * Class:     com_connectedway_io_Framework
 * Method:    addMap
 * Signature: (Lcom/connectedway/io/Framework$Map;)V
  */
JNIEXPORT void JNICALL Java_com_connectedway_io_Framework_addMap
  (JNIEnv *env, jobject objFramework, jobject objMap)
{
  OFC_FRAMEWORK_MAP map ;
 
  map.prefix = map_get_name (env, objMap) ;
  map.path = map_get_path (env, objMap) ;
  map.type = map_get_type (env, objMap) ;
  map.desc = map_get_desc (env, objMap) ;
  map.thumbnail = map_get_thumbnail (env, objMap) ;
 
  ofc_framework_add_map(&map) ;
 
  map_free_map (&map) ;
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

#if 0
jobject new_remote (JNIEnv *env, jobject objFramework,
		   OFC_FRAMEWORK_REMOTE *remote)
{
  jstring jstrName ;
  jstring jstrIp ;
  jint jiPort ;
  jobject objRemote ;

  jmethodID midNewRemote ;

  jclass clsBlueFramework ;

  jstrIp = tchar2jstr (env, remote->ip) ;
  jstrName = tchar2jstr (env, remote->name) ;
  jiPort = remote->port ;

  clsBlueFramework = (*env)->FindClass
    (env, "com/connectedway/io/Framework") ;

  midNewRemote = (*env)->GetMethodID 
    (env, clsBlueFramework, "newRemote",
     "(Ljava/lang/String;Ljava/lang/String;I)Lcom/connectedway/io/Framework$Remote;") ;

  (*env)->DeleteLocalRef (env, clsBlueFramework) ;

  objRemote = (*env)->CallObjectMethod (env, objFramework, 
					midNewRemote, jstrName, 
					jstrIp, jiPort) ;
  (*env)->DeleteLocalRef (env, jstrName) ;
  (*env)->DeleteLocalRef (env, jstrIp) ;

  return (objRemote) ;
}

jobjectArray new_remotes (JNIEnv *env, 
			 jobject objFramework,
			 OFC_FRAMEWORK_REMOTES *remotes)
{
  jobjectArray jremotes ;
  jobject objRemote ;
  int i ;
  jclass clsRemote ;

  clsRemote = (*env)->FindClass
    (env, "com/connectedway/io/Framework$Remote") ;

  jremotes = 
    (*env)->NewObjectArray (env, remotes->numRemotes, clsRemote, NULL) ;
  (*env)->DeleteLocalRef (env, clsRemote) ;  

  for (i = 0 ; i < remotes->numRemotes ; i++)
    {
      objRemote = new_remote (env, objFramework, &remotes->remote[i]) ;
      (*env)->SetObjectArrayElement (env, jremotes, i, objRemote) ;
    }

  return (jremotes) ;
}

OFC_LPTSTR remote_get_name (JNIEnv *env, jobject objRemote)
{
  jmethodID midGetName ;

  jstring jstrName ;
  OFC_LPTSTR tstrName ;
  jclass clsRemote ;

  clsRemote = (*env)->FindClass
    (env, "com/connectedway/io/Framework$Remote") ;
  midGetName = (*env)->GetMethodID(env, clsRemote, "getName", 
				   "()Ljava/lang/String;") ;
  (*env)->DeleteLocalRef (env, clsRemote) ;
  jstrName = (*env)->CallObjectMethod(env, objRemote, midGetName) ;

  tstrName = OFC_NULL ;
  if (jstrName != NULL)
    {
      tstrName = jstr2tchar (env, jstrName) ;
      (*env)->DeleteLocalRef (env, jstrName) ;
    }

  return (tstrName) ;
}

OFC_LPTSTR remote_get_ip (JNIEnv *env, jobject objRemote)
{
  jmethodID midGetIP ;
  jstring jstrIp ;
  OFC_LPTSTR tstrIp ;
  jclass clsRemote ;

  clsRemote = (*env)->FindClass
    (env, "com/connectedway/io/Framework$Remote") ;
  midGetIP = (*env)->GetMethodID(env, clsRemote, "getIP", 
				 "()Ljava/lang/String;") ;
  (*env)->DeleteLocalRef (env, clsRemote) ;

  jstrIp = (*env)->CallObjectMethod(env, objRemote, midGetIP) ;

  tstrIp = OFC_NULL ;
  if (jstrIp != NULL)
    {
      tstrIp = jstr2tchar (env, jstrIp) ;
      (*env)->DeleteLocalRef (env, jstrIp) ;
    }

  return (tstrIp) ;
}

OFC_UINT16 remote_get_port (JNIEnv *env, jobject objRemote)
{
  jmethodID midGetPort ;
  jint jiPort ;
  jclass clsRemote ;

  clsRemote = (*env)->FindClass
    (env, "com/connectedway/io/Framework$Remote") ;
  midGetPort = (*env)->GetMethodID(env, clsRemote, "getPort", 
				   "()I") ;
  (*env)->DeleteLocalRef (env, clsRemote) ;

  jiPort = (*env)->CallIntMethod(env, objRemote, midGetPort) ;

  return ((OFC_UINT16) jiPort) ;
}

static OFC_VOID remote_free_remote (OFC_FRAMEWORK_REMOTE *remote)
{
  ofc_free (remote->name) ;
}

/*
 * Class:     com_connectedway_io_Framework
 * Method:    addRemote
 * Signature: (Lcom/connectedway/io/Framework$Remote;)V
 */
JNIEXPORT void JNICALL Java_com_connectedway_io_Framework_addRemote
  (JNIEnv *env, jobject objFramework, jobject objRemote)
{
  OFC_FRAMEWORK_REMOTE remote ;

  remote.name = remote_get_name (env, objRemote) ;
  remote.ip = remote_get_ip (env, objRemote) ;
  remote.port = remote_get_port (env, objRemote) ;

  ofc_framework_add_remote(&remote) ;

  remote_free_remote (&remote) ;
}

/*
 * Class:     com_connectedway_io_Framework
 * Method:    getRemotes
 * Signature: ()[Lcom/connectedway/io/Framework$Remote;
 */
JNIEXPORT jobjectArray JNICALL Java_com_connectedway_io_Framework_getRemotes
  (JNIEnv *env, jobject objFramework)
{
  OFC_FRAMEWORK_REMOTES *remotes ;
  jobjectArray jremotes ;

  jremotes = OFC_NULL ;
  remotes = ofc_framework_get_remotes() ;
  if (remotes != OFC_NULL)
    {
      jremotes = new_remotes (env, objFramework, remotes) ;
      ofc_framework_free_remotes(remotes) ;
    }
  return (jremotes) ;
}

/*
 * Class:     com_connectedway_io_Framework
 * Method:    removeRemote
 * Signature: (Ljava/lang/String;)V
 */
JNIEXPORT void JNICALL Java_com_connectedway_io_Framework_removeRemote
  (JNIEnv *env, jobject objFramework, jstring name)
{
  OFC_LPTSTR tstrName ;

  tstrName = jstr2tchar (env, name) ;

  ofc_framework_remove_remote(tstrName) ;
  ofc_free (tstrName) ;

}
#endif

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

#if 0
/*
 * Class:     com_connectedway_io_Framework
 * Method:    getServerEnabled
 * Signature: ()Z
 */
JNIEXPORT jboolean JNICALL Java_com_connectedway_io_Framework_getServerEnabled
  (JNIEnv *env, jobject objFramework)
{
  OFC_BOOL on ;
  jboolean jon ;

  on = ofc_framework_get_server_enabled() ;
  jon = JNI_FALSE ;
  if (on == OFC_TRUE)
    jon = JNI_TRUE ;
  return (jon) ;
}

/*
 * Class:     com_connectedway_io_Framework
 * Method:    getServerUsername
 * Signature: ()Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL Java_com_connectedway_io_Framework_getServerUsername
  (JNIEnv *env, jobject objFramework)
{
  jstring jstrUsername ;
  OFC_LPTSTR tstrUsername ;

  tstrUsername = ofc_framework_get_server_username () ;

  jstrUsername = tchar2jstr (env, tstrUsername) ;

  ofc_free (tstrUsername) ;
  
  return (jstrUsername) ;
}

/*
 * Class:     com_connectedway_io_Framework
 * Method:    getServerPassword
 * Signature: ()Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL Java_com_connectedway_io_Framework_getServerPassword
  (JNIEnv *env, jobject objFramework)
{
  jstring jstrPassword ;
  OFC_LPTSTR tstrPassword ;

  tstrPassword = ofc_framework_get_server_password () ;

  jstrPassword = tchar2jstr (env, tstrPassword) ;

  ofc_free (tstrPassword) ;
  
  return (jstrPassword) ;
}
#endif

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
#if defined(__APPLE__)
  void *envx ;
#else
  JNIEnv *envx ;
#endif

  if (g_jvm != OFC_NULL)
    {
      status = (*g_jvm)->GetEnv(g_jvm, (void **) &env, JNI_VERSION_1_6);
      if (status < 0) 
	{
	  //	  status = (*g_jvm)->AttachCurrentThread (g_jvm, (const struct JNINativeInterface **) &env, NULL);
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

#if 0
jobject new_server_event (JNIEnv *env, int event)
{
  jobject objServerEvent ;
  jstring jstrServerEvent ;
  const char *szServerEvent ;

  switch (event)
    {
    default:
    case SERVER_EVENT_INFO:
      szServerEvent = "INFO" ;
      break ;

    case SERVER_EVENT_ERROR:
      szServerEvent = "ERROR" ;
      break ;

    case SERVER_EVENT_SECURITY:
      szServerEvent = "SECURITY" ;
      break ;
    }

  jstrServerEvent = (*env)->NewStringUTF (env, szServerEvent) ;

  objServerEvent = (*env)->CallStaticObjectMethod(env, g_serverevent,
						  g_midvalueof, 
						  jstrServerEvent) ;
  (*env)->DeleteLocalRef (env, jstrServerEvent) ;

  return (objServerEvent) ;
}
	  
void call_on_server_event (int event, OFC_CCHAR * message)
{
  jstring jstr ;
  JNIEnv *env ;
  jobject objServerEvent ;

  env = get_env() ;
  if (env != OFC_NULL)
    {
      objServerEvent = new_server_event (env, event) ;
      jstr = (*env)->NewStringUTF (env, message) ;
      (*env)->CallVoidMethod (env, g_interface, g_method, objServerEvent, jstr) ;
      (*env)->DeleteLocalRef (env, jstr) ;
      (*env)->DeleteLocalRef (env, objServerEvent) ;
    }
}

/*
 * Class:     com_connectedway_io_Framework
 * Method:    setServerListener
 * Signature: (Lcom/connectedway/io/Framework$ServerListener;)V
 */
JNIEXPORT void JNICALL Java_com_connectedway_io_Framework_setServerListener
  (JNIEnv *env, jobject objFramework, jobject serverListener)
{
  jclass clsListener ;
  jclass clsServerEvent ;

  (*env)->GetJavaVM(env, &g_jvm) ;

  clsListener = (*env)->GetObjectClass (env, serverListener) ;
  g_interface = 
    (*env)->NewGlobalRef(env, serverListener) ;


  g_method = (*env)->GetMethodID(env, clsListener, 
				 "onServerEvent", 
				 "(Lcom/connectedway/io/Framework$serverEvent;Ljava/lang/String;)V");

  clsServerEvent = (*env)->FindClass
    (env, "com/connectedway/io/Framework$serverEvent") ;
  g_serverevent = 
    (*env)->NewGlobalRef(env, clsServerEvent) ;
  g_midvalueof = (*env)->GetStaticMethodID
    (env, g_serverevent, "valueOf",
     "(Ljava/lang/String;)Lcom/connectedway/io/Framework$serverEvent;") ;

  (*env)->DeleteLocalRef(env, clsListener) ;
  (*env)->DeleteLocalRef(env, clsServerEvent) ;
#if 0
  /*
   * Later if we ever deregister
   */
  (*env)->DeleteGlobalRef(env, g_interface) ;
  (*env)->DeleteGlobalRef(env, g_serverevent) ;
#endif
}

/*
 * Class:     com_connectedway_io_Framework
 * Method:    setServerEnabled
 * Signature: (Z)V
 */
JNIEXPORT void JNICALL Java_com_connectedway_io_Framework_setServerEnabled
  (JNIEnv *env, jobject objFramework, jboolean jon)
{
  OFC_BOOL on ;

  on = OFC_FALSE ;
  if (jon)
    on = OFC_TRUE ;
  ofc_framework_set_server_enabled(on) ;
}

/*
 * Class:     com_connectedway_io_Framework
 * Method:    setServerUsername
 * Signature: (Ljava/lang/String;)V
 */
JNIEXPORT void JNICALL Java_com_connectedway_io_Framework_setServerUsername
  (JNIEnv *env, jobject objFramework, jstring name)
{
  OFC_LPTSTR tstrName ;

  tstrName = jstr2tchar (env, name) ;

  ofc_framework_set_server_username(tstrName) ;
  ofc_free (tstrName) ;
}

/*
 * Class:     com_connectedway_io_Framework
 * Method:    setServerPassword
 * Signature: (Ljava/lang/String;)V
 */
JNIEXPORT void JNICALL Java_com_connectedway_io_Framework_setServerPassword
  (JNIEnv *env, jobject objFramework, jstring pass)
{
  OFC_LPTSTR tstrPass ;

  tstrPass = jstr2tchar (env, pass) ;

  ofc_framework_set_server_password(tstrPass) ;
  ofc_free (tstrPass) ;
}
#endif

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

#if 0
/*
 * Class:     com_connectedway_io_Framework
 * Method:    getMaxEvents
 * Signature: ()I
 */
JNIEXPORT int JNICALL Java_com_connectedway_io_Framework_getMaxEvents
  (JNIEnv *env, jobject objFramework)
{
  return (ofc_framework_get_max_events()) ;
}

/*
 * Class:     com_connectedway_io_Framework
 * Method:    getTimePeriod
 * Signature: ()J
 */
JNIEXPORT jlong JNICALL Java_com_connectedway_io_Framework_getTimePeriod
  (JNIEnv *env, jobject objFramework)
{
  return ((jlong) ofc_framework_get_time_period()) ;
}

/*
 * Class:     com_connectedway_io_Framework
 * Method:    setMaxEvents
 * Signature: (I)V
 */
JNIEXPORT void JNICALL Java_com_connectedway_io_Framework_setMaxEvents
  (JNIEnv *env, jobject objFramework, int maxEvents)
{
  ofc_framework_set_max_events(maxEvents) ;
}

/*
 * Class:     com_connectedway_io_Framework
 * Method:    setTimePeriod
 * Signature: (J)V
 */
JNIEXPORT void JNICALL Java_com_connectedway_io_Framework_setTimePeriod
  (JNIEnv *env, jobject objFramework, long timePeriod)
{
  ofc_framework_set_time_period(timePeriod) ;
}
#endif

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
  