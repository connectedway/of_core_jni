package com.connectedway.io ;

import com.connectedway.io.FileSystem ;
import java.io.SyncFailedException ;
import java.io.IOException ;

/**
 * Instances of the file descriptor class serve as an opaque handle
 * to the underlying machine-specific structure representing an open
 * file, an open socket, or another source or sink of bytes. 
 *
 * Currently, this class simply shadows the platform FileDescriptor
 *
 * #see java.io.FileDescriptor
 */
public class FileDescriptor {
    
    private long handle ;
    private FileSystem fs = FileSystem.getFileSystem() ;
    /**
     * Create a Blue File Descriptor
     *
     * @see java.io.FileDescriptor#FileDescriptor() ;
     */
    public FileDescriptor(long handle) {
	this.handle = handle ;
    }
    /**
     * Get the platform file descriptor
     *
     * @return Platform File Descriptor
     */
    public long getHandle() {
        return handle ;
    }

    public void sync() throws SyncFailedException {
	try {
	    fs.flush(this) ;
	} catch (IOException except) {
	    throw new SyncFailedException("Could Not Flush File") ;
	}
    }
}

