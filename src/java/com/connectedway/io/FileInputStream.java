package com.connectedway.io ;

import java.io.* ;
import java.lang.SecurityException ;

import java.nio.channels.FileChannel ;

import com.connectedway.io.FileSystem ;
import com.connectedway.io.FileDescriptor ;

/**
 * A <code>BlueFileInputStream</code> obtains input bytes
 * from a file in a network aware file system. 
 *
 * @see java.io.FileInputStream
 */
public class FileInputStream extends InputStream {

    private FileDescriptor fd = null ;
    private FileSystem fs = FileSystem.getFileSystem() ;

    /**
     * Creates a <code>FileInputStream</code> by opening a connection 
     * to an actual file, the file named by the path name <code>name</code>
     * in the file system. 
     *
     * @see java.io.FileInputStream#FileInputStream(String)
     */
    public FileInputStream (String name) 
	throws FileNotFoundException, SecurityException {
	super() ;
	fd = fs.open (name, FileSystem.OPEN_READ) ;
    }
	    
    /**
     * Creates a <code>FileInputStream</code> by opening a connection to an 
     * actual file, the file named by the <code>File</code>
     * object <code>file</code> in the file system.
     *
     * @see java.io.FileInputStream#FileInputStream(File)
     */
    public FileInputStream (java.io.File file) 
	throws FileNotFoundException, SecurityException {
	super() ;
	fd = fs.open (file.getPath(), FileSystem.OPEN_READ) ;
    }

    /**
     * Creates a <code>FileInputStream</code> by using the file descriptor
     * <code>fdObj</code>, which represents an existing connection to an
     * actual file in the file system.
     *
     * @see java.io.FileInputStream#FileInputStream(FileDescriptor)
     */
    public FileInputStream (FileDescriptor fd) 
	throws FileNotFoundException {
	super() ;
	this.fd = fd ;
    }
    
    /**
     * Reads a byte of data from this input stream. This method blocks
     * if no input is yet available.
     *
     * @see java.io.FileInputStream#read()
     */
    public int read() throws IOException {
	return fs.read(fd) ;
    }

    /**
     * Reads up to <code>b.length</code> bytes of data from this input
     * stream into an array of bytes.
     *
     * @see java.io.FileInputStream#read(byte[])
     */
    public int read(byte b[]) throws IOException {

	return read (b, 0, b.length) ;
    }
    
    /**
     * Reads a subarray as a sequence of bytes.
     *
     * @see java.io.FileInputStream (byte[], int, int)
     */
    public int read(byte b[], int off, int len) throws IOException {
	int ret ;
	ret = fs.read (fd, b, off, len) ;
	return ret ;
    }
    
    /**
     * Skips over and discards <code>n</code> bytes of data from the
     * input stream.
     *
     * @see java.io.FileInputStream#skip(long)
     */
    public long skip(long n) throws IOException {
	long ret ;

	ret = fs.skip(fd, n) ;

	return ret ;
    }
    
    /**
     * Returns an estimate of the number of remaining bytes that can be read (or
     * skipped over) from this input stream without blocking by the next
     * invocation of a method for this input stream.
     *
     * @see java.io.FileInputStream#available()
     */
    public int available() throws IOException {
	return fs.available(fd) ;
    }
    
    /**
     * Closes this file input stream and releases any system resources
     * associated with the stream.
     *
     * @see java.io.FileInputStream#close()
     */
    public void close() throws IOException {
	if (fd == null)
	    throw new IOException ("File already closed");
	else {
	    fs.close(fd) ;
	    fd = null ;
	}
    }
    
    /**
     * Returns the <code>BlueFileChannel</code> object for this input stream
     *
     * @see java.io.FileInputStream#getChannel()
     */
    public FileChannel getChannel() {
	return null ;
    }

    public void mark (int readLimit) {
    }

    public boolean markSupported() {
	return false ;
    }

    public void reset() throws IOException {
    }

    /**
     * Returns the <code>BlueFileDescriptor</code> object  that represents 
     * the connection to the actual file in the file system being
     * used by this <code>BlueFileInputStream</code>.
     *
     * @see java.io.FileInputStream#getFD()
     */
    public FileDescriptor getFD() throws IOException {
	return fd ;
    }

    protected void finalize() throws IOException {
	if (fd != null) {
	    close();
	    fd = null ;
	}
    }
}
