package com.connectedway.io ;

import java.io.FileNotFoundException;
import java.io.IOException ;
import java.io.OutputStreamWriter ;

import com.connectedway.io.FileOutputStream ;
import com.connectedway.io.FileDescriptor ;

/**
 * Convenience class for writing character files.
 * 
 * @see java.io.FileWriter
 */
public class FileWriter extends OutputStreamWriter {

    /**
     * Constructs a FileWriter object given a file name.
     *
     * @see java.io.FileWriter#FileWriter(String)
     */
    public FileWriter (String fileName) throws IOException{
	super (new FileOutputStream (fileName)) ;
    }
    
    /**
     * Constructs a FileWriter object given a file name with a boolean
     * indicating whether or not to append the data written.
     *
     * @see java.io.FileWriter#FileWriter(String, boolean)
     */
    public FileWriter (String fileName, boolean append) throws IOException {
	super (new FileOutputStream (fileName, append)) ;
    }
    
    /**
     * Constructs a FileWriter object given a BlueFile object.
     *
     * @see java.io.FileWriter#FileWriter(File)
     */
    public FileWriter (java.io.File file) throws IOException {
	super (new FileOutputStream (file)) ;
    }
    
    /**
     * Constructs a FileWriter object given a BlueFile object. 
     *
     * @see java.io.FileWriter#FileWriter(File)
     */
    public FileWriter (java.io.File file, boolean append) throws IOException {
	super (new FileOutputStream (file, append)) ;
    }
    
    /**
     * Constructs a FileWriter object associated with a file descriptor.
     * @throws FileNotFoundException 
     *
     * @see java.io.FileWriter#FileWriter(FileDescriptor)
     */
    public FileWriter (FileDescriptor fd) throws FileNotFoundException {
	super (new FileOutputStream (fd)) ;
    }
    
}

