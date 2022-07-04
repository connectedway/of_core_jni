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
#include "ofc/handle.h"
#include "ofc/libc.h"
#include "ofc/framework.h"
#include "ofc/persist.h"
#include "ofc/net.h"
#include "ofc/net_internal.h"
#include "ofc/heap.h"
#include "ofc/libc.h"
#include "ofc/path.h"
#include "ofc/thread.h"
#include "ofc/file.h"
#include "ofc/fs.h"
#include "ofc/path.h"

#include "of_resolver_fs/resolver_api.h"

#include "ofc_jni/com_connectedway_io_Utils.h"
#include "ofc_jni/com_connectedway_io_Resolver.h"

#define OPEN_FUNC "Open"
#define OPEN_SIG "(Ljava/lang/String;Ljava/lang/String;)Lcom/connectedway/io/Resolver$ResolverFile;"
#define MKDIR_FUNC "MkDir"
#define MKDIR_SIG "(Ljava/lang/String;)I"
#define WRITE_FUNC "Write"
#define WRITE_SIG "(Lcom/connectedway/io/Resolver$ResolverFile;[BI)I"
#define PWRITE_FUNC "PWrite"
#define PWRITE_SIG "(Lcom/connectedway/io/Resolver$ResolverFile;Ljava/nio/ByteBuffer;II)I"
#define READ_FUNC "Read"
#define READ_SIG "(Lcom/connectedway/io/Resolver$ResolverFile;[BI)I"
#define PREAD_FUNC "PRead"
#define PREAD_SIG "(Lcom/connectedway/io/Resolver$ResolverFile;Ljava/nio/ByteBuffer;II)I"
#define CLOSE_FUNC "Close"
#define CLOSE_SIG "(Lcom/connectedway/io/Resolver$ResolverFile;)I"
#define UNLINK_FUNC "Unlink"
#define UNLINK_SIG "(Ljava/lang/String;)I"
#define RMDIR_FUNC "RmDir"
#define RMDIR_SIG "(Ljava/lang/String;)I"
#define STAT_FUNC "Stat"
#define STAT_SIG "(Ljava/lang/String;)Lcom/connectedway/io/Resolver$ResolverStat;"
#define STATFS_FUNC "StatFS"
#define STATFS_SIG "(Ljava/lang/String;)Lcom/connectedway/io/Resolver$ResolverStatFS;"
#define OPENDIR_FUNC "OpenDir"
#define OPENDIR_SIG "(Ljava/lang/String;)Lcom/connectedway/io/Resolver$ResolverDir;"
#define READDIR_FUNC "ReadDir"
#define READDIR_SIG "(Lcom/connectedway/io/Resolver$ResolverDir;)Lcom/connectedway/io/Resolver$ResolverDirent;"
#define CLOSEDIR_FUNC "CloseDir"
#define CLOSEDIR_SIG "(Lcom/connectedway/io/Resolver$ResolverDir;)I"
#define RENAME_FUNC "Rename"
#define RENAME_SIG "(Ljava/lang/String;Ljava/lang/String;)I"
#define FLUSH_FUNC "Flush"
#define FLUSH_SIG "(Lcom/connectedway/io/Resolver$ResolverFile;)I"
#define SEEK_FUNC "Seek"
#define SEEK_SIG "(Lcom/connectedway/io/Resolver$ResolverFile;JI)J"
#define TRUNCATE_FUNC "Truncate"
#define TRUNCATE_SIG "(Lcom/connectedway/io/Resolver$ResolverFile;J)I"
#define UNLOCK_FUNC "Unlock"
#define UNLOCK_SIG "(Lcom/connectedway/io/Resolver$ResolverFile;JJ)I"
#define LOCK_FUNC "Lock"
#define LOCK_SIG "(Lcom/connectedway/io/Resolver$ResolverFile;JJZ)I"
#define TRYLOCK_FUNC "TryLock"
#define TRYLOCK_SIG "(Lcom/connectedway/io/Resolver$ResolverFile;JJZ)I"

static jobject g_resolver = OFC_NULL ;
static JavaVM* g_jvm;
static jclass clsResolverStat;
static jclass clsResolverStatFS;
static jclass clsResolverDirent;
static jfieldID fieldResolverStatId;
static jfieldID fieldResolverStatSize;
static jfieldID fieldResolverStatMTime;
static jfieldID fieldResolverStatFlags;
static jfieldID fieldResolverDirent;
static jfieldID fieldResolverStatFSAvail;
static jfieldID fieldResolverStatFSBlocks;
static jmethodID g_method_stat;
static jmethodID g_method_statfs;
static jmethodID g_method_open;
static jmethodID g_method_mkdir;
static jmethodID g_method_write;
static jmethodID g_method_pwrite;
static jmethodID g_method_read;
static jmethodID g_method_pread;
static jmethodID g_method_close;
static jmethodID g_method_unlink;
static jmethodID g_method_rmdir;
static jmethodID g_method_opendir;
static jmethodID g_method_readdir;
static jmethodID g_method_closedir;
static jmethodID g_method_rename;
static jmethodID g_method_flush;
static jmethodID g_method_seek;
static jmethodID g_method_truncate;
static jmethodID g_method_lock;
static jmethodID g_method_trylock;
static jmethodID g_method_unlock;
static jclass clsResolverListener;

static OFC_VOID exceptCheck(JNIEnv *env)
{
  if ((*env)->ExceptionCheck(env)) {
    (*env)->ExceptionDescribe(env);
    (*env)->ExceptionClear(env);
  }
}

static OFC_VOID relEnv(JNIEnv *env)
{
  exceptCheck(env);
#if 1
  (*g_jvm)->DetachCurrentThread(g_jvm);
#endif
}

static JNIEnv *getEnv() 
{
#if 0
  static OFC_DWORD local_env = 0;
  
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

  /*
   * Has anyone gotten a local_env yet?
   */
  if (local_env == 0)
    {
      local_env = ofc_thread_create_variable();
    }

  env = (JNIEnv *) ofc_thread_get_variable(local_env);
  if (env == OFC_NULL)
    {
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
	  ofc_thread_set_variable(local_env, (OFC_DWORD_PTR) env);
	}
      else
	env = OFC_NULL ;
    }
#else
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
#endif
  return (env) ;
}

/*
 * Class:     com_connectedway_io_Resolver
 * Method:    setResolverListener
 * Signature: (Lcom/connectedway/io/Resolver$ResolverListener;)V
 */
JNIEXPORT void JNICALL Java_com_connectedway_io_Resolver_setResolverListener
  (JNIEnv *env, jobject objSmb, jobject resolverListener)
{
  (*env)->GetJavaVM(env, &g_jvm) ;

  clsResolverStat = (*env)->FindClass(env, "com/connectedway/io/Resolver$ResolverStat");
  clsResolverStatFS = (*env)->FindClass(env, "com/connectedway/io/Resolver$ResolverStatFS");
  clsResolverDirent = (*env)->FindClass(env, "com/connectedway/io/Resolver$ResolverDirent");
  clsResolverListener = (*env)->FindClass(env, "com/connectedway/io/Resolver$ResolverListener");

  fieldResolverStatId = (*env)->GetFieldID(env, clsResolverStat,
                                           "FileId", "J");
  fieldResolverStatSize = (*env)->GetFieldID(env, clsResolverStat,
					     "Size", "J");
  fieldResolverStatMTime = (*env)->GetFieldID(env, clsResolverStat,
					      "MTime", "J");
  fieldResolverStatFlags = (*env)->GetFieldID(env, clsResolverStat,
					     "Flags", "I");
  fieldResolverDirent = (*env)->GetFieldID(env, clsResolverDirent,
					   "Name",
					   "Ljava/lang/String;");
  fieldResolverStatFSAvail = (*env)->GetFieldID(env, clsResolverStatFS,
						"Avail", "J");
  fieldResolverStatFSBlocks = (*env)->GetFieldID(env, clsResolverStatFS,
						 "Blocks", "J");

  g_method_stat = (*env)->GetMethodID(env, clsResolverListener, 
				      STAT_FUNC, STAT_SIG);
  g_method_statfs = (*env)->GetMethodID(env, clsResolverListener, 
					STATFS_FUNC, STATFS_SIG);
  g_method_open = (*env)->GetMethodID(env, clsResolverListener, 
				      OPEN_FUNC, OPEN_SIG);
  g_method_mkdir = (*env)->GetMethodID(env, clsResolverListener, 
				       MKDIR_FUNC, MKDIR_SIG);
  g_method_write = (*env)->GetMethodID(env, clsResolverListener, 
				       WRITE_FUNC, WRITE_SIG);
  g_method_pwrite = (*env)->GetMethodID(env, clsResolverListener, 
					PWRITE_FUNC, PWRITE_SIG);
  g_method_read = (*env)->GetMethodID(env, clsResolverListener, 
				      READ_FUNC, READ_SIG);
  g_method_pread = (*env)->GetMethodID(env, clsResolverListener, 
				       PREAD_FUNC, PREAD_SIG);
  g_method_close = (*env)->GetMethodID(env, clsResolverListener, 
				       CLOSE_FUNC, CLOSE_SIG);
  g_method_unlink = (*env)->GetMethodID(env, clsResolverListener, 
					UNLINK_FUNC, UNLINK_SIG);
  g_method_rmdir = (*env)->GetMethodID(env, clsResolverListener, 
				       RMDIR_FUNC, RMDIR_SIG);
  g_method_opendir = (*env)->GetMethodID(env, clsResolverListener, 
					 OPENDIR_FUNC, OPENDIR_SIG);
  g_method_readdir = (*env)->GetMethodID(env, clsResolverListener,
					 READDIR_FUNC, READDIR_SIG);
  g_method_closedir = (*env)->GetMethodID(env, clsResolverListener, 
					  CLOSEDIR_FUNC, CLOSEDIR_SIG);
  g_method_rename = (*env)->GetMethodID(env, clsResolverListener, 
					RENAME_FUNC, RENAME_SIG);
  g_method_flush = (*env)->GetMethodID(env, clsResolverListener, 
				       FLUSH_FUNC, FLUSH_SIG);
  g_method_seek = (*env)->GetMethodID(env, clsResolverListener, 
				      SEEK_FUNC, SEEK_SIG);
  g_method_truncate = (*env)->GetMethodID(env, clsResolverListener, 
					  TRUNCATE_FUNC, TRUNCATE_SIG);
  g_method_lock = (*env)->GetMethodID(env, clsResolverListener, 
				      LOCK_FUNC, LOCK_SIG);
  g_method_trylock = (*env)->GetMethodID(env, clsResolverListener, 
					 TRYLOCK_FUNC, TRYLOCK_SIG);
  g_method_unlock = (*env)->GetMethodID(env, clsResolverListener, 
					UNLOCK_FUNC, UNLOCK_SIG);

  g_resolver = (*env)->NewGlobalRef(env, resolverListener) ;
}

RESOLVER_FILE *resolver_open(OFC_CTCHAR *lpFileName, OFC_CCHAR *mode)
{
  JNIEnv *env ;
  jstring jstrFileName;
  jstring jstrMode;
  jobject rFile;
  jobject gFile;

  gFile = NULL;
  env = getEnv();
  if (env != OFC_NULL)
    {
      jstrFileName = tchar2jstr(env, lpFileName);
      jstrMode = (*env)->NewStringUTF(env, mode);
      rFile = (*env)->CallObjectMethod(env, g_resolver,
				       g_method_open,
				       jstrFileName,
				       jstrMode);
      exceptCheck(env);
      gFile = (*env)->NewGlobalRef(env, rFile) ;
      relEnv(env);
    }
  return (gFile);
}

OFC_INT resolver_mkdir(OFC_CTCHAR *lpFileName)
{
  JNIEnv *env ;
  jstring jstrFileName;
  OFC_INT ret;

  env = getEnv();
  ret = -1;
  if (env != OFC_NULL)
    {
      jstrFileName = tchar2jstr(env, lpFileName);
      ret = (*env)->CallIntMethod(env, g_resolver,
				  g_method_mkdir,
				  jstrFileName);
      relEnv(env);
    }
  return(ret);
}
  
OFC_SIZET resolver_write(RESOLVER_FILE *rfile, OFC_LPCVOID lpBuffer,
			 OFC_SIZET count)
{
  jobject jFile;
  JNIEnv *env ;
  jint written;
  jbyteArray baBuffer;

  written = -1;
  
  env = getEnv();
  if (env != OFC_NULL)
    {
      jFile = (jobject) rfile;
      baBuffer = (*env)->NewByteArray(env, count);
      (*env)->SetByteArrayRegion(env, baBuffer, 0, count,
				 (jbyte *) lpBuffer);

      written = (*env)->CallIntMethod(env, g_resolver,
				      g_method_write,
				      jFile, baBuffer,
				      (jint) count);
      
      relEnv(env);
    }
  return ((OFC_SIZET) written);
}

OFC_SIZET resolver_pwrite(RESOLVER_FILE *rfile, OFC_LPCVOID lpBuffer,
			  OFC_SIZET count, OFC_OFFT offset)
{
  jobject jFile;
  JNIEnv *env ;
  jint written;
  jobject bbBuffer;

  written = -1;
  
  env = getEnv();
  if (env != OFC_NULL)
    {
      jFile = (jobject) rfile;
      bbBuffer = (*env)->NewDirectByteBuffer(env, (void *) lpBuffer, count);
      written = (*env)->CallIntMethod(env, g_resolver,
				      g_method_pwrite,
				      jFile, bbBuffer,
				      (jint) count,
				      (jint) offset);
      
      relEnv(env);
    }
  return ((OFC_SIZET) written);
}

OFC_SIZET resolver_read(RESOLVER_FILE *rfile, OFC_LPCVOID lpBuffer,
			OFC_SIZET count)
{
  jobject jFile;
  JNIEnv *env ;
  jint readd;
  jbyteArray baBuffer;

  readd = -1;
  
  env = getEnv();
  if (env != OFC_NULL)
    {
      jFile = (jobject) rfile;
      baBuffer = (*env)->NewByteArray(env, count);
      readd = (*env)->CallIntMethod(env, g_resolver,
				    g_method_read,
				    jFile, (jbyteArray) baBuffer,
				    (jint) count);
      (*env)->GetByteArrayRegion(env, baBuffer, 0, readd,
				 (jbyte *) lpBuffer);

      relEnv(env);
    }
  return ((OFC_SIZET) readd);
}

OFC_SIZET resolver_pread(RESOLVER_FILE *rfile, OFC_LPVOID lpBuffer,
			 OFC_SIZET count, OFC_OFFT offset)
{
  jobject jFile;
  JNIEnv *env ;
  jint readd;
  jobject bbBuffer;

  readd = -1;
  
  env = getEnv();
  if (env != OFC_NULL)
    {
      jFile = (jobject) rfile;
      bbBuffer = (*env)->NewDirectByteBuffer(env, lpBuffer, count);
      readd = (*env)->CallIntMethod(env, g_resolver,
				    g_method_pread,
				    jFile, bbBuffer,
				    (jint) count,
				    (jint) offset);
      relEnv(env);
    }
  return ((OFC_SIZET) readd);
}

OFC_INT resolver_close(RESOLVER_FILE *rfile)
{
  jobject jFile;
  JNIEnv *env ;
  int ret;
  
  env = getEnv();
  ret = -1;
  if (env != OFC_NULL)
    {
      jFile = (jobject) rfile;
      ret = (*env)->CallIntMethod(env, g_resolver,
				  g_method_close,
				  jFile);

      (*env)->DeleteGlobalRef(env, jFile) ;
      relEnv(env);
    }
  return (ret);
}

OFC_INT resolver_unlink(OFC_CTCHAR *lpFileName)
{
  JNIEnv *env ;
  jstring jstrFileName;
  int ret;

  ret = -1;
  env = getEnv();
  if (env != OFC_NULL)
    {
      jstrFileName = tchar2jstr(env, lpFileName);
      ret = (*env)->CallIntMethod(env, g_resolver,
				  g_method_unlink,
				  jstrFileName);
      relEnv(env);
    }
  return (ret);
}

OFC_INT resolver_rmdir(OFC_CTCHAR *lpPathName)
{
  JNIEnv *env ;
  jstring jstrFileName;
  int ret;

  ret = -1;
  env = getEnv();
  if (env != OFC_NULL)
    {
      jstrFileName = tchar2jstr(env, lpPathName);
      ret = (*env)->CallIntMethod(env, g_resolver,
				  g_method_rmdir,
				  jstrFileName);
      relEnv(env);
    }
  return (ret);
}

OFC_INT resolver_stat(OFC_CTCHAR *tName, struct resolver_stat *sb)
{
  JNIEnv *env ;
  jstring jstrFileName;
  jobject objResolverStat;
  OFC_INT ret;
  int mode;
  OFC_PATH *path;

  ret = -1;
  env = getEnv();

  if (env != OFC_NULL)
    {
      jstrFileName = tchar2jstr(env, tName);

      objResolverStat = (*env)->CallObjectMethod(env, g_resolver,
						 g_method_stat,
						 jstrFileName);

      if (objResolverStat != OFC_NULL)
	{
          sb->st_ino = (OFC_UINT64) (*env)->GetLongField(env, objResolverStat,
                                                         fieldResolverStatId);
	  sb->st_size = (OFC_OFFT) (*env)->GetLongField(env, objResolverStat,
							fieldResolverStatSize);
	  sb->st_blocks = (OFC_OFFT) (sb->st_size + 4095) / 4096;
	  sb->st_mtime = (*env)->GetLongField(env,
					      objResolverStat,
					      fieldResolverStatMTime) / 1000;
	  sb->st_atime = sb->st_mtime;
	  sb->st_ctime = sb->st_mtime;
	  sb->st_nlink = 1;
	  mode = (OFC_INT) (*env)->GetIntField(env, objResolverStat,
                                               fieldResolverStatFlags);
          if (mode & 0x01)
            sb->st_mode = RESOLVER_S_IFDIR;
          else
            {
              path = ofc_path_createW(tName);
              if (ofc_path_hidden(path))
                sb->st_mode = RESOLVER_S_IFHID;
              else
                sb->st_mode = RESOLVER_S_IFREG;
            }

	  ret = 0;
	}
      relEnv(env);
    }
  return (ret);
}

OFC_INT resolver_statfs(OFC_CTCHAR *tName, struct resolver_statfs *fsstat)
{
  JNIEnv *env ;
  jstring jstrFileName;
  jobject objResolverStatFS;
  OFC_INT ret;

  ret = -1;
  env = getEnv();

  if (env != OFC_NULL)
    {
      jstrFileName = tchar2jstr(env, tName);

      objResolverStatFS = (*env)->CallObjectMethod(env, g_resolver,
						   g_method_statfs,
						   jstrFileName);

      if (objResolverStatFS != OFC_NULL)
	{
	  fsstat->f_bsize = 512;
	  fsstat->f_bavail =
	    (OFC_LONG) ((*env)->GetLongField(env,
					    objResolverStatFS,
					    fieldResolverStatFSAvail) /
			4096);
	  fsstat->f_blocks =
	    (OFC_LONG) ((*env)->GetLongField(env,
					    objResolverStatFS,
					    fieldResolverStatFSBlocks) /
			4096);
	  ret = 0;
	}
      relEnv(env);
    }
  return (ret);
}

RESOLVER_DIR *resolver_opendir(OFC_CTCHAR *name)
{
  JNIEnv *env ;
  jstring jstrFileName;
  jobject rDir;
  jobject gDir;

  gDir = OFC_NULL;
  env = getEnv();
  if (env != OFC_NULL)
    {
      jstrFileName = tchar2jstr(env, name);
      rDir = (RESOLVER_DIR *) (*env)->CallObjectMethod(env, g_resolver,
						       g_method_opendir,
						       jstrFileName);
      gDir = (*env)->NewGlobalRef(env, rDir) ;
    }
  return (gDir);
}

struct resolver_dirent *resolver_readdir(RESOLVER_DIR *dirp)
{
  JNIEnv *env ;
  jobject objResolverDirent;
  static struct resolver_dirent sdirent;
  struct resolver_dirent *ret;

  ret = OFC_NULL;

  env = getEnv();
  if (env != OFC_NULL)
    {
      objResolverDirent = (*env)->CallObjectMethod(env, g_resolver,
						   g_method_readdir,
						   (jobject) dirp);
      if (objResolverDirent != NULL)
	{
	  jstring name =
	    (jstring) (*env)->GetObjectField(env, objResolverDirent,
					     fieldResolverDirent);
	  OFC_TCHAR *tstrName;
	  tstrName = jstr2tchar(env, name);
	  ofc_tstrncpy(sdirent.d_name, tstrName,
		       sizeof(sdirent.d_name)/sizeof(sdirent.d_name[0]));
	  ofc_free(tstrName);
	  ret = &sdirent;
	}
      relEnv(env);
    }
  return(ret);
}

OFC_INT resolver_closedir(RESOLVER_DIR *dirp)
{
  jobject jDir;
  JNIEnv *env ;
  int ret;
  
  ret = -1;
  env = getEnv();
  if (env != OFC_NULL)
    {
      jDir = (jobject) dirp;
      ret = (*env)->CallIntMethod(env, g_resolver,
				     g_method_closedir,
				     jDir);
      (*env)->DeleteGlobalRef(env, jDir) ;
      relEnv(env);
    }
  return(ret);
}

OFC_INT resolver_rename(OFC_CTCHAR *old, OFC_CTCHAR *new)
{
  int ret;
  JNIEnv *env ;
  jstring jstrOldName;
  jstring jstrNewName;

  ret = -1;
  env = getEnv();
  if (env != OFC_NULL)
    {
      jstrOldName = tchar2jstr(env, old);
      jstrNewName = tchar2jstr(env, new);

      ret = (*env)->CallIntMethod(env, g_resolver,
				  g_method_rename,
				  jstrOldName,
				  jstrNewName);
      relEnv(env);
    }
  return (ret);
}

OFC_INT resolver_flush(RESOLVER_FILE *rfile)
{
  jobject jFile;
  JNIEnv *env ;
  int ret;
  
  env = getEnv();
  ret = -1;
  if (env != OFC_NULL)
    {
      jFile = (jobject) rfile;
      ret = (*env)->CallIntMethod(env, g_resolver,
				  g_method_flush,
				  jFile);
      relEnv(env);
    }
  return (ret);
}

OFC_LONG resolver_seek(RESOLVER_FILE *rfile, OFC_INT64 offset, OFC_INT whence)
{
  jobject jFile;
  JNIEnv *env ;
  OFC_LONG ret;
  jlong joffset;
  jint jwhence;
  
  env = getEnv();
  ret = -1;
  if (env != OFC_NULL)
    {
      jFile = (jobject) rfile;
      joffset = (jlong) offset;
      jwhence = (jint) whence;
      
      ret = (OFC_LONG) (*env)->CallLongMethod(env, g_resolver,
					      g_method_seek,
					      jFile, joffset, jwhence);

      relEnv(env);
    }
  return (ret);
}

OFC_INT resolver_truncate(RESOLVER_FILE *rfile, OFC_UINT64 offset)
{
  jobject jFile;
  JNIEnv *env ;
  OFC_INT ret;
  jlong joffset;
  
  env = getEnv();
  ret = -1;
  if (env != OFC_NULL)
    {
      jFile = (jobject) rfile;
      joffset = (jlong) offset;
      ret = (*env)->CallIntMethod(env, g_resolver,
				  g_method_truncate,
				  jFile, joffset);

      relEnv(env);
    }
  return (ret);
}

OFC_VOID resolver_unlock(RESOLVER_FILE *rfile, OFC_UINT64 offset,
			 OFC_UINT64 size)
{
  jobject jFile;
  JNIEnv *env ;
  jlong joffset;
  jlong jsize;
  
  env = getEnv();
  if (env != OFC_NULL)
    {
      jFile = (jobject) rfile;
      joffset = (jlong) offset;
      jsize = (jlong) size;
      (*env)->CallIntMethod(env, g_resolver,
			    g_method_unlock,
			    jFile, joffset, jsize);
      relEnv(env);
    }
}

OFC_VOID resolver_lock(RESOLVER_FILE *rfile, OFC_UINT64 offset,
		       OFC_UINT64 size, OFC_BOOL shared)
{
  jobject jFile;
  JNIEnv *env ;
  jboolean jshared;
  jlong joffset;
  jlong jsize;
  
  env = getEnv();
  if (env != OFC_NULL)
    {
      jFile = (jobject) rfile;
      joffset = (jlong) offset;
      jsize = (jlong) size;
      jshared = (jboolean) shared;
      (*env)->CallIntMethod(env, g_resolver,
			    g_method_lock,
			    jFile, joffset, jsize, jshared);
      relEnv(env);
    }
}

OFC_INT resolver_trylock(RESOLVER_FILE *rfile, OFC_UINT64 offset,
			 OFC_UINT64 size, OFC_BOOL shared)
{
  jobject jFile;
  JNIEnv *env ;
  jboolean jshared;
  jlong joffset;
  jlong jsize;
  OFC_INT ret = -1;
  
  env = getEnv();
  if (env != OFC_NULL)
    {
      jFile = (jobject) rfile;
      joffset = (jlong) offset;
      jsize = (jlong) size;
      jshared = (jboolean) shared;
      (*env)->CallIntMethod(env, g_resolver,
			    g_method_trylock,
			    jFile, joffset, jsize, jshared);
      ret = 0;
      relEnv(env);
    }
  return (ret);
}

