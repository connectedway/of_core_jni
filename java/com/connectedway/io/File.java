package com.connectedway.io;

import androidx.annotation.NonNull;

import java.io.IOException;
import java.io.FilenameFilter;
import java.io.FileFilter;
import java.util.ArrayList;

import java.lang.NullPointerException;

import java.net.URI;
import java.net.URISyntaxException;

/**
 * A network aware representation of a file and directory pathnames. These files
 * are used by the Connected Way SMB Client. It is an extension of the basic
 * java.io.File class.
 * 
 * <p>
 * In most all cases, the documentation of the related method in the
 * java.io.File class is sufficient for the documentation of the File
 * method. Only comments reflecting a different behavior are documented.
 * 
 * <p>
 * Android SMB implements the notion of path maps. These allow applications to
 * create short hands for remote paths. These maps can be considered as similar
 * to the DOS drive name scheme. These maps are implemented as pathname
 * prefixes. So, for instance, if a path has been mapped to <em>home:</em> then
 * use of <em>home:</em> as the prefix in a path will map to a remote file.
 * 
 * <p>
 * Android SMB leverages the notion of a pathname. 
 * Paths on a CIFS file share can
 * be specified using a cifs URI, a UNC pathname of the form
 * <code> "\\\\"</code>&nbsp; or a mapped prefix in a pathname. Pathnames that
 * are neither a CIFS URI, a UNC path, or specified by a mapped prefix will be
 * passed to the java.io.File class. In this way, an application can generically
 * implement methods that utilize a File and in turn transparently access
 * either local or remote files.
 * 
 * <p>
 * Android SMB implements a flat network view. 
 * In general, this implies that the
 * first field of a pathname (UNC or CIFS URI) is the server name, next is the
 * share, followed by share relative path, followed by file name. Non-UNC
 * pathnames are always passed to the platform File class. For this reason, 
 * Android SMB paths do not support the notion of a prefix.
 * 
 * When browsing a network for workgroup and servers, there is ambiguity in the
 * path names. When the first field is not specified or is filtered, the
 * assumption is that the first field represents the workgroup or domain. When
 * the second field is not specified or is filtered, the pathname has dual
 * meanings. The first meaning is that the first field represents a workgroup
 * and the second field represents a server. The alternative meaning is that the
 * first field represents a server and the second field represents a share.
 * Whether the first field represents a workgroup or server will be determined
 * by whether a workgroup by that name exists. If it does, the path is assumed
 * to be workgroup/server. If a workgroup doesn't exist, the path is assumed to
 * be server/share.
 * 
 * <p>
 * Android SMB supports either forward slash or reverse slash separator
 * characters. Abstract pathnames though will be converted to pathname strings
 * using the backslash character for consistency. This should occur
 * transparently to the application.
 * 
 * <p>
 * In Android SMB, all pathnames are absolute. Relative pathnames can be
 * manipulated by the application but are not supported by the Android SMB APIs.
 * 
 * <p>
 * As discussed above, the prefix concept is used in pathnames to specify mapped
 * paths. Prefixes can also be used to specify share roots.
 * 
 * <p>
 * The Android SMB network file system has one root. This root is an empty UNC
 * pathname <code>"\\\\"</code>. When calling Android SMB to list roots, the
 * routine will return the network root as well as all local roots.
 * 
 * @author Richard Schmitt
 * 
 * @see java.io.File
 */

public class File extends java.io.File {
	/**
	 * 
	 */
	private static final long serialVersionUID = 1L;

	private final String pathname;
        private boolean attributesset ;
        private int attributes ;
        private boolean sizeset ;
        private long size ;
        private boolean dateset ;
	private long date ;

	/**
	 * The FileSystem object representing the platform's local file system.
	 */
	static private final FileSystem fs = FileSystem.getFileSystem();

	/**
	 * Creates a new <code>File</code> instance by converting the given
	 * pathname string into an abstract pathname. If the given string is the
	 * empty string, then the result is the empty abstract pathname.
	 * 
	 * @see java.io.File#File(String) ;
	 */
	public File(String pathname) throws NullPointerException {
		super("");

		this.attributesset = false ;
		this.sizeset = false ;
		this.dateset = false ;
		if (pathname == null)
			throw new NullPointerException();

		this.pathname = fs.normalize(pathname);
	}

	/**
	 * Creates a new <code>File</code> instance from a parent pathname
	 * string and a child pathname string.
	 * 
	 * <p>
	 * If <code>parent</code> is <code>null</code> or empty then the new
	 * <code>File</code> instance is created as if it were invoked by the
	 * single-argument <code>File</code> constructor on the given
	 * <code>child</code> pathname string.
	 * 
	 * @see java.io.File#File(String, String)
	 */
	public File(String parent, String child) throws NullPointerException {
		super("");

		this.attributesset = false ;
		this.sizeset = false ;
		this.dateset = false ;
		if (child == null)
			throw new NullPointerException();

		if (parent != null) {
			this.pathname = fs.resolve(fs.normalize(parent),
					fs.normalize(child));
		} else {
			this.pathname = fs.normalize(child);
		}
	}

	/**
	 * Creates a new <code>File</code> instance from a parent abstract
	 * pathname and a child pathname string.
	 * 
	 * @see java.io.File#File(java.io.File, String)
	 */
	public File(File parent, String child) throws NullPointerException {
		super("");

		this.attributesset = false ;
		this.sizeset = false ;
		this.dateset = false ;
		if (parent != null) {
			this.pathname = fs.resolve(parent.getPath(), fs.normalize(child));
		} else {
			this.pathname = fs.normalize(child);
		}
	}

	public void authenticate(String username, String workgroup, String password)
			throws NullPointerException {
		fs.authenticate(pathname, username, workgroup, password);
	}

	/**
	 * Creates a new <tt>File</tt> instance by converting the given
	 * <tt>file:</tt> or <tt>cifs:</tt> URI into an abstract pathname.
	 * 
	 * @param uri
	 *            An absolute, hierarchical URI with a scheme equal to
	 *            <tt>"file"</tt> or <tt> "cifs"</tt>, a non-empty path
	 *            component, and undefined authority, query, and fragment
	 *            components
	 * 
	 * @see java.io.File#File(URI)
	 */
	public File(URI uri) {
		super("");

		this.attributesset = false ;
		this.sizeset = false ;
		this.dateset = false ;
		this.pathname = fs.normalize(uri.getPath());
	}

	public File(File file) {
		super("") ;
		this.attributesset = file.attributesset ;
		this.attributes = file.attributes ;
		this.sizeset = file.sizeset ;
		this.size = file.size ;
		this.dateset = file.dateset ;
		this.date = file.date ;
		this.pathname = file.pathname ;
	}

	/**
	 * Creates a new <tt>File</tt> instance by converting a java.io.File
	 * 
	 * @param file
	 *            A java.io.file
	 */
	public File(java.io.File file) {
		super("");

		this.attributesset = false ;
		this.sizeset = false ;
		this.dateset = false ;
		this.pathname = fs.normalize(file.getPath());
	}

	public static boolean isRemoteFile(String pathname) {
		return FileSystem.isRemoteFile(pathname);
	}

	public static boolean isRemoteFile(String parent, String child)
			throws NullPointerException {
		String pathName;
		if (child == null)
			throw new NullPointerException();

		if (parent != null) {
			if (parent.equals("")) {
				pathName = fs.resolve(fs.getDefaultParent(),
						fs.normalize(child));
			} else {
				pathName = fs
						.resolve(fs.normalize(parent), fs.normalize(child));
			}
		} else {
			pathName = fs.normalize(child);
		}

		return FileSystem.isRemoteFile(pathName);
	}

	public static boolean isRemoteFile(File parent, String child)
			throws NullPointerException {

		String pathName;

		if (parent != null) {
			if (parent.getPath().equals("")) {
				pathName = fs.resolve(fs.getDefaultParent(),
						fs.normalize(child));
			} else {
				pathName = fs.resolve(parent.getPath(), fs.normalize(child));
			}
		} else {
			pathName = fs.normalize(child);
		}

		return FileSystem.isRemoteFile(pathName);
	}

	public static boolean isRemoteFile(URI uri) {
		String pathName = uri.getPath();
		return FileSystem.isRemoteFile(pathName);
	}

	/**
	 * Return the name of the file or directory.
	 * 
	 * @see java.io.File#getName()
	 */
	@NonNull
	public String getName() {
		return pathname.substring(pathname.lastIndexOf(java.io.File.separatorChar) + 1);
	}

	/**
	 * Returns the pathname string of the parent.
	 * 
	 * @see java.io.File#getParent()
	 */
	public String getParent() {
		return pathname.substring(0, pathname.lastIndexOf(java.io.File.separatorChar));
	}

	/**
	 * Returns the abstract pathname of this pathnames parent
	 * 
	 * @see java.io.File#getParentFile()
	 */
	public File getParentFile() {
		String parent;
		File ret;
		int index;

		if (pathname.length() <= fs.prefixLength(this.getPath())) {
			ret = null;
		} else {
			index = pathname.lastIndexOf(java.io.File.separatorChar);

			if (index < fs.prefixLength(pathname)) {
				if (FileSystem.isRemoteFile(this.getPath()))
					ret = new File("//");
				else
					ret = new File("/");
			} else {
				parent = pathname.substring(0, index);
				ret = new File(parent);
			}
		}

		return ret;
	}

	/**
	 * Converts this abstract pathname into a pathname string
	 * 
	 * @see java.io.File#getPath()
	 */
	@NonNull
	public String getPath() {
		return pathname;
	}

	/**
	 * Tests whether this pathname is absolute
	 * 
	 * @see java.io.File#isAbsolute()
	 */
	public boolean isAbsolute() {
		return fs.isAbsolute(this);
	}

	/**
	 * Returns the absolute pathname string of this abstract pathname.
	 * 
	 * @see java.io.File#getAbsolutePath()
	 */
	@NonNull
	public String getAbsolutePath() {
		return fs.resolve(this);
	}

	/**
	 * Returns the absolute form of this abstract pathname
	 * 
	 * @see java.io.File#getAbsoluteFile()
	 */
	@NonNull
	public File getAbsoluteFile() {
		return new File(fs.resolve(this));
	}

	/**
	 * Returns the canonical pathname string of this abstract pathname.
	 * 
	 * @see java.io.File#getCanonicalPath()
	 */
	@NonNull
	public String getCanonicalPath() throws IOException {
		return fs.canonicalize(fs.resolve(this));
	}

	/**
	 * Returns the canonical form of this abstract pathname.
	 * 
	 * @see java.io.File#getCanonicalFile()
	 */
	@NonNull
	public File getCanonicalFile() throws IOException {
		return new File(fs.canonicalize(fs.resolve(this)));
	}

	/**
	 * Constructs a <tt>file:</tt> or <tt>cifs:</tt> URI that represents this
	 * abstract pathname.
	 * 
	 * @see java.io.File#toURI()
	 */
	@NonNull
	public URI toURI() {
		URI uri;
		String prefix ;

		if (FileSystem.isRemoteFile(this.getPath()))
		    prefix = "cifs:" ;
		else
		    prefix = "file:" ;

		try {
		    uri = new URI(prefix + getAbsolutePath ()) ;
		} catch (URISyntaxException except) {
			/*
			 * paths that have been normalized through the FileSystem should
			 * not have any URI syntax in them.
			 */
			throw new Error(except);
		}
		return uri;
	}

	/**
	 * Tests whether the application can read the file denoted by this abstract
	 * pathname.
	 * 
	 * @see java.io.File#canRead()
	 */
	public boolean canRead() {
		return fs.checkAccess(this, FileSystem.ACCESS_READ);
	}

	/**
	 * Tests whether the application can modify the file denoted by this
	 * abstract pathname.
	 * 
	 * @see java.io.File#canWrite()
	 */
	public boolean canWrite() {
		return fs.checkAccess(this, FileSystem.ACCESS_WRITE);
	}

	/**
	 * Tests whether the file or directory denoted by this abstract pathname
	 * exists.
	 * 
	 * @see java.io.File#exists()
	 */
	public boolean exists() {
	    boolean ret;

	    if (!this.attributesset) {
		this.attributes = fs.getBooleanAttributes(this) ;
		this.attributesset = true ;
	    }
		ret = (this.attributes & FileSystem.BA_EXISTS) != 0;
	    return ret;
	}

	/**
	 * Tests whether the file denoted by this abstract pathname is a directory.
	 * 
	 * @see java.io.File#isDirectory()
	 */
	public boolean isDirectory() {
	    boolean ret;

	    if (!this.attributesset) {
		this.attributes = fs.getBooleanAttributes(this) ;
		this.attributesset = true ;
	    }
		ret = (this.attributes & FileSystem.BA_DIRECTORY) != 0;
	    return ret;
	}

	/**
	 * Tests whether the file denoted by this abstract pathname is a 
	 * workgroup
	 */
	public boolean isWorkgroup() {
	    boolean ret;

	    if (!this.attributesset) {
		this.attributes = fs.getBooleanAttributes(this) ;
		this.attributesset = true ;
	    }
		ret = (this.attributes & FileSystem.BA_WORKGROUP) != 0;
	    return ret;
	}

	/**
	 * Tests whether the file denoted by this abstract pathname is a 
	 * server
	 */
	public boolean isServer() {
	    boolean ret;

	    if (!this.attributesset) {
		this.attributes = fs.getBooleanAttributes(this) ;
		this.attributesset = true ;
	    }
		ret = (this.attributes & FileSystem.BA_SERVER) != 0;
	    return ret;
	}

	/**
	 * Tests whether the file denoted by this abstract pathname is a 
	 * Share
	 */
	public boolean isShare() {
	    boolean ret;

	    if (!this.attributesset) {
		this.attributes = fs.getBooleanAttributes(this) ;
		this.attributesset = true ;
	    }
		ret = (this.attributes & FileSystem.BA_SHARE) != 0;
	    return ret;
	}

	/**
	 * Tests whether the file denoted by this abstract pathname is a normal
	 * file.
	 * 
	 * @see java.io.File#isFile()
	 */
	public boolean isFile() {
	    boolean ret;
		
	    if (!this.attributesset) {
		this.attributes = fs.getBooleanAttributes(this) ;
		this.attributesset = true ;
	    }
		ret = (this.attributes & FileSystem.BA_REGULAR) != 0;
	    return ret;
	}

	/**
	 * Tests whether the file named by this abstract pathname is a hidden file.
	 * 
	 * @see java.io.File#isHidden()
	 */
	public boolean isHidden() {
	    boolean ret;

	    if (!this.attributesset) {
		this.attributes = fs.getBooleanAttributes(this) ;
		this.attributesset = true ;
	    }
		ret = (this.attributes & FileSystem.BA_HIDDEN) != 0;
	    return ret;
	}

	public void setAttributes(int attributes) {
	    
	    this.attributesset = true ;
	    this.attributes = attributes ;
	}

	public void setLength(long size) {

	    this.sizeset = true ;
	    this.size = size ;
	}

	public void setDate(long date) {

	    this.dateset = true ;
	    this.date = date ;
	}

	/**
	 * Returns the time that the file denoted by this abstract pathname was last
	 * modified.
	 * 
	 * @see java.io.File#lastModified()
	 */
	public long lastModified() {
	    long date ;

	    if (!this.dateset) {
		this.date = fs.getLastModifiedTime(this);
		this.dateset = true ;
	    }

	    date = this.date ;
	    return date;
	}

	/**
	 * Returns the length of the file denoted by this abstract pathname.
	 * 
	 * @see java.io.File#length()
	 */
	public long length() {
	    long size ;

	    if (!this.sizeset) {
		this.size = fs.getLength(this);
		this.sizeset = true ;
	    }

	    size = this.size ;

	    return size;
	}

	/**
	 * Atomically creates a new, empty file named by this abstract pathname
	 * 
	 * @see java.io.File#createNewFile()
	 */
	public boolean createNewFile() throws IOException {
		return fs.createFileExclusively(fs.resolve(this));
	}

	/**
	 * Deletes the file or directory denoted by this abstract pathname.
	 * 
	 * @see java.io.File#delete()
	 */
	public boolean delete() {
		return fs.delete(this);
	}

	/**
	 * Returns an array of strings naming the files and directories in the
	 * directory denoted by this abstract pathname.
	 * 
	 * @see java.io.File#list()
	 */
	public String[] list() {
		return fs.list(this);
	}

	/**
	 * Returns an array of strings naming the files and directories in the
	 * directory denoted by this abstract pathname that satisfy the specified
	 * filter.
	 * 
	 * @see File#list(FilenameFilter)
	 */
	public String[] list(FilenameFilter filter) {
		String[] all;
		ArrayList<String> filtered = new ArrayList<>();

		all = fs.list(this);
		for (String s : all) {
			if (filter.accept(this, s))
				filtered.add(s);
		}
		return (String[]) filtered.toArray(new String[0]);
	}

	/**
	 * Returns an array of abstract pathnames denoting the files in the
	 * directory denoted by this abstract pathname.
	 * 
	 * @see java.io.File#listFiles()
	 */
	public File[] listFiles() {
	    return (fs.listFiles(this)) ;
	}

	/**
	 * Returns an array of abstract pathnames denoting the files and directories
	 * in the directory denoted by this abstract pathname that satisfy the
	 * specified filter.
	 * 
	 * @see java.io.File#listFiles(FilenameFilter)
	 */
	public File[] listFiles(FilenameFilter filter) {
		String[] all;
		ArrayList<File> filtered = new ArrayList<>();
		File f;

		all = fs.list(this);
		for (String s : all) {
			if (filter.accept(this, s)) {
				f = new File(this, s);
				filtered.add(f);
			}
		}
		return (File[]) filtered.toArray(new File[0]);
	}

	/**
	 * Returns an array of abstract pathnames denoting the files and directories
	 * in the directory denoted by this abstract pathname that satisfy the
	 * specified filter.
	 * 
	 * @see java.io.File#listFiles(FileFilter)
	 */
	public File[] listFiles(FileFilter filter) {
		String[] all;
		ArrayList<File> filtered = new ArrayList<>();
		File test;

		all = fs.list(this);
		for (String s : all) {
			test = new File(this, s);
			if (filter.accept(test))
				filtered.add(test);
		}
		return (File[]) filtered.toArray(new File[0]);
	}

	/**
	 * Creates the directory named by this abstract pathname.
	 * 
	 * @see java.io.File#mkdir()
	 */
	public boolean mkdir() {
		return fs.createDirectory(this);
	}

	/**
	 * Creates the directory named by this abstract pathname, including any
	 * necessary but nonexistent parent directories.
	 * 
	 * @see java.io.File#mkdirs()
	 */
	@SuppressWarnings("ResultOfMethodCallIgnored")
	public boolean mkdirs() {

		boolean ret = false;
		File parent = getParentFile();

		if (parent != null) {
			if (!parent.exists())
				parent.mkdirs();
			if (parent.isDirectory())
				ret = mkdir();
		}
		return ret;
	}

	/**
	 * Renames the file denoted by this abstract pathname.
	 * 
	 * @see java.io.File#renameTo(java.io.File)
	 */
	public boolean renameTo(File dest) {
		return fs.rename(this, dest);
	}

	/**
	 * Sets the last-modified time of the file or directory named by this
	 * abstract pathname.
	 * 
	 * @see java.io.File#setLastModified(long)
	 */
	public boolean setLastModified(long time) {
		return fs.setLastModifiedTime(this, time);
	}

	/**
	 * Marks the file or directory named by this abstract pathname so that only
	 * read operations are allowed.
	 * 
	 * @see java.io.File#setReadOnly()
	 */
	public boolean setReadOnly() {
		return fs.setReadOnly(this);
	}

	/**
	 * Sets the owner's or everybody's write permission for this abstract
	 * pathname.
	 * 
	 * @see java.io.File#setWritable(boolean, boolean)
	 */
	public boolean setWritable(boolean writable, boolean ownerOnly) {
		return fs.setPermission(this, FileSystem.ACCESS_WRITE, writable,
				ownerOnly);
	}

	/**
	 * A convenience method to set the owner's write permission for this
	 * abstract pathname.
	 * 
	 * @see java.io.File#setWritable(boolean)
	 */
	public boolean setWritable(boolean writable) {
		return fs.setPermission(this, FileSystem.ACCESS_WRITE, writable,
				true);
	}

	/**
	 * Sets the owner's or everybody's read permission for this abstract
	 * pathname.
	 * 
	 * @see java.io.File#setReadable(boolean, boolean)
	 */
	public boolean setReadable(boolean readable, boolean ownerOnly) {
		return fs.setPermission(this, FileSystem.ACCESS_READ, readable,
				ownerOnly);
	}

	/**
	 * A convenience method to set the owner's read permission for this abstract
	 * pathname.
	 * 
	 * @see java.io.File#setReadable(boolean)
	 */
	public boolean setReadable(boolean readable) {
		return fs.setPermission(this, FileSystem.ACCESS_READ, readable,
				true);
	}

	/**
	 * Sets the owner's or everybody's execute permission for this abstract
	 * pathname.
	 * 
	 * @see java.io.File#setExecutable(boolean, boolean)
	 */
	public boolean setExecutable(boolean executable, boolean ownerOnly) {
		return fs.setPermission(this, FileSystem.ACCESS_EXECUTE,
				executable, ownerOnly);
	}

	/**
	 * A convenience method to set the owner's execute permission for this
	 * abstract pathname.
	 * 
	 * @see java.io.File#setExecutable(boolean)
	 */
	public boolean setExecutable(boolean executable) {
		return fs.setPermission(this, FileSystem.ACCESS_EXECUTE,
				executable, true);
	}

	/**
	 * Tests whether the application can execute the file denoted by this
	 * abstract pathname.
	 * 
	 * @see java.io.File#canExecute()
	 */
	public boolean canExecute() {
		return fs.checkAccess(this, FileSystem.ACCESS_EXECUTE);
	}

	/**
	 * List the available filesystem roots.
	 * 
	 * @see java.io.File#listRoots()
	 */
	@NonNull
	public static File[] listRoots() {
		return fs.listRoots();
	}

	/**
	 * Returns the size of the partition <a href="#partName">named</a> by this
	 * abstract pathname.
	 * 
	 * @see java.io.File#getTotalSpace()
	 */
	public long getTotalSpace() {
		return fs.getSpace(this, FileSystem.SPACE_TOTAL);
	}

	/**
	 * Returns the number of unallocated bytes in the partition <a
	 * href="#partName">named</a> by this abstract path name.
	 * 
	 * @see java.io.File#getFreeSpace()
	 */
	public long getFreeSpace() {
		return fs.getSpace(this, FileSystem.SPACE_FREE);
	}

	/**
	 * Returns the number of bytes available to this virtual machine on the
	 * partition <a href="#partName">named</a> by this abstract pathname.
	 * 
	 * @see java.io.File#getUsableSpace()
	 */
	public long getUsableSpace() {
		return fs.getSpace(this, FileSystem.SPACE_USABLE);
	}

	/**
	 * Compares two abstract pathnames lexicographically.
	 * 
	 * @see java.io.File#compareTo(java.io.File)
	 */
	public int compareTo(File pathname) {
		return fs.compare(this, pathname);
	}

	/**
	 * <p>
	 * Creates a new empty file in the specified directory, using the given
	 * prefix and suffix strings to generate its name.
	 * 
	 * @see java.io.File#createTempFile(String, String, java.io.File)
	 */
	public static File createTempFile(String prefix, String suffix,
			File directory) throws IOException {
            java.io.File jdir = new java.io.File(directory.toString());
            java.io.File tfile =
                java.io.File.createTempFile(prefix, suffix, jdir);
            return new File(tfile);
	}

	/**
	 * Creates an empty file in the default temporary-file directory, using the
	 * given prefix and suffix to generate its name.
	 * 
	 * @see java.io.File#createTempFile(String, String)
	 */
	@NonNull
	public static File createTempFile(@NonNull String prefix, String suffix)
			throws IOException {
            java.io.File tfile = java.io.File.createTempFile(prefix, suffix);
            return new File(tfile);
	}

	/**
	 * Tests this abstract pathname for equality with the given object.
	 * 
	 * @see java.io.File#equals(Object)
	 */
	public boolean equals(Object obj) {
		boolean ret;

		ret = false;
		if ((obj instanceof File)) {
		    ret = (fs.compare(this, (File) obj) == 0);
		}
		return ret;
	}

	/**
	 * Computes a hash code for this abstract pathname.
	 * 
	 * @see java.io.File#hashCode()
	 */
	public int hashCode() {
		return fs.hashCode(this);
	}

	/**
	 * Returns the pathname string of this abstract pathname.
	 * 
	 * @see java.io.File#toString()
	 */
	@NonNull
	public String toString() {
		return getPath();
	}

	/**
	 * Returns the last error Android SMB encountered
	 * 
	 */
        public static long getLastError() {
	    return fs.getLastError() ;
	}

        public static String getLastErrorString() {
	    return fs.getLastErrorString() ;
	}
}
