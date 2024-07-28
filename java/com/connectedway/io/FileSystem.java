package com.connectedway.io;

import java.io.IOException ;

import com.connectedway.nio.directory.Directory ;
import java.io.FileNotFoundException ;
import java.lang.SecurityException ;

/**
 * Package-private abstract class for the Blue Share CIFS
 * filesystem abstraction.
 */
public class FileSystem {
    /**
     * The default platform specific file system
     */
    static private FileSystem ref ;

    private FileSystem()
    {
	/*
	 * Insure that the JNI library is loaded
	 */
	Framework.getFramework() ;
    }

    /**
     * Return the FileSystem object 
     */
    public static FileSystem getFileSystem() {
	if (ref == null)
	    ref = new FileSystem() ;
	return (ref) ;
    }

    public static native boolean isRemoteFile (String pathname) ;

    /* -- Normalization and construction -- */

    /**
     * Return the local filesystem's name-separator character.  If the
     * platform name-separator is not a forward or backward slash, then
     * default to a backward slash.
     */
    public char getSeparator() {
	char sep = java.io.File.separatorChar ;

	if (sep != '/' && sep != '\\')
	    sep = '\\' ;

	return sep ;
    }

    /**
     * Return the local filesystem's path-separator character.
     */
    public char getPathSeparator() {
	return java.io.File.pathSeparatorChar ;
    }

    /**
     * Convert the given pathname string to normal form.  If the string is
     * already in normal form then it is simply returned.
     */
    public native String normalize(String path) ;

    /**
     * Compute the length of this pathname string's prefix.  The pathname
     * string must be in normal form.
     */
    public native int prefixLength(String path) ;

    /**
     * Resolve the child pathname string against the parent.
     * Both strings must be in normal form, and the result
     * will be in normal form.
     */
    public native String resolve(String parent, String child) ;

    public native void authenticate (String path, String username,
    		String workgroup, String password) ;

    /**
     * Return the parent pathname string to be used when the parent-directory
     * argument in one of the two-argument File constructors is the empty
     * pathname.  In a network file system, there is no notion of default
     * parent.
     */
    public String getDefaultParent() {
	return "" ;
    }

    /**
     * Blue Share supports URI directly so we do not need to convert.
     */
    public String fromURIPath(String path) {
	return (path) ;
    }

    /* -- Path operations -- */

    /**
     * Tell whether or not the given abstract pathname is absolute.
     */
    public native boolean isAbsolute(File f) ;

    /**
     * Resolve the given abstract pathname into absolute form.  Invoked by the
     * getAbsolutePath and getCanonicalPath methods in the File class.
     */
    public native String resolve(File f) ;

    /**
     * Canonicalize a path
     */
    public native String canonicalize(String path) throws IOException ;

    /* Constants for simple boolean attributes */
    public static final int BA_EXISTS    = 0x01;
    public static final int BA_REGULAR   = 0x02;
    public static final int BA_DIRECTORY = 0x04;
    public static final int BA_HIDDEN    = 0x08;
    public static final int BA_SHARE     = 0x10;
    public static final int BA_SERVER    = 0x20;
    public static final int BA_WORKGROUP = 0x40;
    //public static final int BA_BOOKMARK  = 0x80;

    /**
     * Return the simple boolean attributes for the file or directory denoted
     * by the given abstract pathname, or zero if it does not exist or some
     * other I/O error occurs.
     */
    public native int getBooleanAttributes(File f) ;

    public static final int ACCESS_READ    = 0x04;
    public static final int ACCESS_WRITE   = 0x02;
    public static final int ACCESS_EXECUTE = 0x01;

    /**
     * Check whether the file or directory denoted by the given abstract
     * pathname may be accessed by this process.  The second argument specifies
     * which access, ACCESS_READ, ACCESS_WRITE or ACCESS_EXECUTE, to check.
     * Return false if access is denied or an I/O error occurs
     */
    public native boolean checkAccess(File f, int access) ;

    /**
     * Set on or off the access permission (to owner only or to all) to the file
     * or directory denoted by the given abstract pathname, based on the 
     * parameters enable, access and owneronly.
     */
    public native boolean setPermission(File f, int access, 
					boolean enable, boolean owneronly) ;

    /**
     * Return the time at which the file or directory denoted by the given
     * abstract pathname was last modified, or zero if it does not exist or
     * some other I/O error occurs.
     */
    public native long getLastModifiedTime(File f) ;

    /**
     * Return the length in bytes of the file denoted by the given abstract
     * pathname, or zero if it does not exist, is a directory, or some other
     * I/O error occurs.
     */
    public native long getLength(File f) ;

    /* -- File operations -- */

    /**
     * Create a new empty file with the given pathname.  Return
     * <code>true</code> if the file was created and <code>false</code> if a
     * file or directory with the given pathname already exists.  Throw an
     * IOException if an I/O error occurs.
     */
    public native boolean createFileExclusively(String pathname)
        throws IOException ;

    /**
     * Delete the file or directory denoted by the given abstract pathname,
     * returning <code>true</code> if and only if the operation succeeds.
     */
    public native boolean delete(File f) ;

    /**
     * List the elements of the directory denoted by the given abstract
     * pathname.  Return an array of strings naming the elements of the
     * directory if successful; otherwise, return <code>null</code>.
     */
    public native String[] list(File f) ;
    public native File[] listFiles(File f) ;

    /**
     * Create a new directory denoted by the given abstract pathname,
     * returning <code>true</code> if and only if the operation succeeds.
     */
    public native boolean createDirectory(File f) ;

    /**
     * Rename the file or directory denoted by the first abstract pathname to
     * the second abstract pathname, returning <code>true</code> if and only if
     * the operation succeeds.  Both files need to be in the same share.
     */
    public native boolean rename(File f1, File f2) ;

    /**
     * Set the last-modified time of the file or directory denoted by the
     * given abstract pathname, returning <code>true</code> if and only if the
     * operation succeeds.
     */
    public native boolean setLastModifiedTime(File f, long time) ;

    /**
     * Mark the file or directory denoted by the given abstract pathname as
     * read-only, returning <code>true</code> if and only if the operation
     * succeeds.
     */
    public native boolean setReadOnly(File f) ;

    /* -- Filesystem interface -- */

    /**
     * List the available filesystem roots.
     */
    public native File[] listRoots() ;

    /* -- Disk usage -- */
    public static final int SPACE_TOTAL  = 0;
    public static final int SPACE_FREE   = 1;
    public static final int SPACE_USABLE = 2;

    /**
     * get the space available on a file system
     */
    public native long getSpace(File f, int t) ;

    public boolean isRoot (File f) {
	return f.getPath().compareTo("//") == 0 ;
    }
	
    public boolean isWorkgroup (File f) {
	boolean ret ;
        ret = (getBooleanAttributes(f) & FileSystem.BA_WORKGROUP) != 0;
	return ret ;
    }

    public String getWorkgroup (File f) {
	return f.getName() ;
    }

    public boolean isServer (File f) {
	boolean ret ;
        ret = (getBooleanAttributes(f) & FileSystem.BA_SERVER) != 0;
	return ret ;
    }

    public String getServer (File f) {
	return f.getName() ;
    }

    public boolean isShare (File f) {
	boolean ret ;
        ret = (getBooleanAttributes(f) & FileSystem.BA_SHARE) != 0;
	return ret ;
    }

    public String getShare (File f) {
	return f.getName() ;
    }

    /* -- Basic infrastructure -- */

    /**
     * Compare two abstract pathnames lexicographically.
     */
    public int compare(File f1, File f2) {
	return f1.getPath().compareTo(f2.getPath());
    }

    /**
     * Compute the hash code of an abstract pathname.
     */
    public int hashCode(File f) {
	return f.getPath().hashCode() ^ 1234321;
    }
    /**
     * Routines to open read close
     */
    public static final int OPEN_READ = 0;
    public static final int OPEN_WRITE = 1;
    public static final int OPEN_APPEND = 2;
    public static final int OPEN_RW = 3;

    public native FileDescriptor open (String path, int mode) 
	throws FileNotFoundException, SecurityException ;

    public native int read (FileDescriptor fd) throws IOException ;
    public native int read (FileDescriptor fd, byte[] b, int off, int len)
	throws IOException ;
    public native int read (FileDescriptor fd, byte[] b)
	throws IOException ;
    public native void write (FileDescriptor fd, int b) throws IOException ;
    public native void write (FileDescriptor fd, byte[] b, int off,
                              int len) throws IOException ;
    public native void write (FileDescriptor fd, byte[] b)
        throws IOException ;
    public native void seteof (FileDescriptor fd, long pos) 
	throws IOException ;
    public native long skip (FileDescriptor fd, long n) 
	throws IOException ;
    public native void flush (FileDescriptor fd) 
	throws IOException ;
    public native int available(FileDescriptor fd) throws IOException ;
    public native void close (FileDescriptor fd) throws IOException ;

    public static final int SEEK_SET = 0;
    public static final int SEEK_CUR = 1;
    public static final int SEEK_END = 2;
    
    public native long seek (FileDescriptor fd, int mode, long pos) 
	throws IOException ;
    public native long getLastError () ;
    public native String getLastErrorString () ;
    public native File findFile(Directory dir) throws SecurityException, FileNotFoundException ;
    public native void findClose(Directory dir);
}
