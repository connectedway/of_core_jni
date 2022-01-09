/**
 * \mainpage
 *
 * - \subpage openfiles_jni 
 * Open Files also supports Java access through the Open Files JNI.  The JNI
 * provides a configuration interface as well as support for file i/o
 * compatible with java.io
 */

/**
 * \page openfiles_jni Open Files Java Native Interface (JNI)
 *
 * If the configuration parameter \ref OFC_INCLUDE_JNI is set, Open Files
 * will produce a JNI for the desired platform.  The JNI will be in the 
 * for of a JAR file and will consist of a set of Java Classes and a 
 * native library that provides the interface between the Java classes and
 * the Open Files stack
 *
 * Open Files JNI includes the following classes all in the 
 * "com/connectedway/io" namespace (\ref com.connectedway.io).
 *
 * - \ref com.connectedway.io.FileDescriptor
 * - \ref com.connectedway.io.FileSystem
 * - \ref com.connectedway.io.FileOutputStream
 * - \ref com.connectedway.io.FileInputStream
 * - \ref com.connectedway.io.FileWriter
 * - \ref com.connectedway.io.Framework
 * - \ref com.connectedway.io.RandomAccessFile
 * - \ref com.connectedway.io.File
 *
 * Please view the following application for an example Java application
 * written for Open Files
 * 
 * - \ref OfcExplorer.java "Open Files Explorer Java Test App"
 *
 * \example OfcExplorer.java
 * This is an example of using the Open Files JNI to access local 
 * files.  
 */
