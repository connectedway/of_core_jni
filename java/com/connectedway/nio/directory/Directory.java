package com.connectedway.nio.directory;

import com.connectedway.io.File ;
import com.connectedway.io.FileSystem ;

import java.io.FileNotFoundException;
import java.lang.SecurityException;

public class Directory {
    static private FileSystem fs = FileSystem.getFileSystem() ;

    private File parent ;
    private long handle ;

    public Directory (File parent) {
	this.parent = parent ;
	this.handle = new Long(-1) ;
    }

    public File find () throws SecurityException, FileNotFoundException {
	return (fs.findFile(this)) ;
    }

    public void close() {
	fs.findClose(this) ;
    }

    private File getParent() {
	return (parent) ;
    }

    public long getHandle() {
	return (handle) ;
    }

    public void setHandle(long handle) {
	this.handle = handle ;
    }

    public String getLastErrorString() {
	return this.parent.getLastErrorString() ;
    }

}

