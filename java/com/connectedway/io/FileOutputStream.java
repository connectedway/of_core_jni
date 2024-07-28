package com.connectedway.io ;

import java.io.* ;
import java.lang.SecurityException ;
import java.nio.channels.FileChannel ;

/**
 * A file output stream is an output stream for writing data to a
 * <code>BlueFile</code> or to a <code>BlueFileDescriptor</code>. 
 *
 * @see java.io.FileOutputStream
 */
public class FileOutputStream extends OutputStream {

    private FileDescriptor fd;
    private final FileSystem fs = FileSystem.getFileSystem() ;
    /**
     * Creates an output file stream to write to the file with the
     * specified name.
     *
     * @see java.io.FileOutputStream#FileOutputStream(String)
     */
    public FileOutputStream (String name) 
	throws FileNotFoundException, SecurityException {
	super() ;
	fd = fs.open (name, FileSystem.OPEN_WRITE) ;
    }
    
    /**
     * Creates an output file stream to write to the file with the specified
     * <code>name</code>.
     *
     * @see java.io.FileOutputStream#FileOutputStream(String, boolean)
     */
    public FileOutputStream (String name, boolean append) 
	throws FileNotFoundException, SecurityException {
	super() ;
	fd = fs.open (name, append ? FileSystem.OPEN_APPEND : 
		      FileSystem.OPEN_WRITE) ;
    }

    /**
     * Creates a file output stream to write to the file represented by
     * the specified <code>BlueFile</code> object.
     *
     * @see java.io.FileOutputStream#FileOutputStream (File)
     */
    public FileOutputStream (java.io.File file) 
	throws FileNotFoundException, SecurityException {
	super() ;
	fd = fs.open (file.getPath(), FileSystem.OPEN_WRITE) ;
    }
    
    /**
     * Creates a file output stream to write to the file represented by
     * the specified <code>BlueFile</code> object. 
     *
     * @see java.io.FileOutputStream#FileOutputStream(java.io.File, boolean)
     */
    public FileOutputStream (java.io.File file, boolean append) 
	throws FileNotFoundException, SecurityException {
	super() ;
	fd = fs.open (file.getPath(), 
		      append ? FileSystem.OPEN_APPEND :
		      FileSystem.OPEN_WRITE) ;
    }
    
    /**
     * Creates an output file stream to write to the specified file
     * descriptor.
     *
     * @see java.io.FileOutputStream#FileOutputStream(java.io.FileDescriptor)
     */
    public FileOutputStream (FileDescriptor fd) 
	throws FileNotFoundException {
	super() ;
	this.fd = fd ;
    }

    /**
     * Writes the specified byte to this file output stream. 
     *
     * @see java.io.FileOutputStream#write(int) ;
     */
    public void write (int b) throws IOException {
	fs.write (fd, b) ;
    }
    
    /**
     * Writes <code>b.length</code> bytes from the specified byte array
     * to this file output stream.
     *
     * @see java.io.FileOutputStream#write(byte[])
     */
    public void write (byte[] b) throws IOException {
        fs.write(fd, b);
    }
    
    /**
     * Writes <code>len</code> bytes from the specified byte array
     * starting at offset <code>off</code> to this file output stream.
     *
     * @see java.io.FileOutputStream#write(byte[], int, int)
     */
    public void write (byte[] b, int off, int len) throws IOException {
	fs.write (fd, b, off, len) ;
    }
    
    /**
     * Closes this file output stream and releases any system resources
     * associated with this stream. 
     *
     * @see java.io.FileOutputStream#close()
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
     * Flushes this output stream and forces any buffered output bytes
     * to be written out. 
     *
     * @see java.io.OutputStream#flush()
     */
    public void flush() throws IOException {
	fs.flush(fd) ;
    }
    /**
     * Returns the file descriptor associated with this stream.
     *
     * @see java.io.FileOutputStream#getFD()
     */
    public FileDescriptor getFD() throws IOException {
	return fd ;
    }

    /**
     * Returns the <code>FileChannel</code> object for this input stream
     *
     * @see java.io.FileOutputStream#getChannel()
     */
    public FileChannel getChannel() {
	return null ;
    }

    protected void finalize() throws IOException {
 	if (fd != null) {
	    close();
	    fd = null ;
 	}
    }
}

