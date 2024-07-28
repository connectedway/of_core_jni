package com.connectedway.nio.directory;

import java.io.FileNotFoundException;
import java.util.concurrent.ExecutionException;
import java.util.concurrent.FutureTask;
import java.util.concurrent.LinkedBlockingQueue;
import java.lang.InterruptedException;

import java.lang.System;

import com.connectedway.io.File;

public class FileDirectoryStream {

    private LinkedBlockingQueue<File> fileLinkedBlockingQueue =
			new LinkedBlockingQueue<>();
    private FutureTask<Void> fileTask;
    private final DirectoryListener listener ;
    private State state ;
	protected Exception ex = null ;
    protected Directory dir;
    private String lastError = "Success" ;
    private long expiration ;

    public enum State { LOADING, FRESH }

	public interface DirectoryListener {
	void onNotifyEvent() ;
    }

    public FileDirectoryStream(File startDirectory,
			       DirectoryListener listener)
    {
		this.dir = new Directory (startDirectory) ;

	this.listener = listener ;
	this.expiration = System.currentTimeMillis() + 60000 ;
	state = State.LOADING ;
	//
	// startFileSearch spawns a thread that will build up the 
	// fileLinkedBlockingQueue.  Asynchronous to this thread
	// we'll have an iterator to pull things off the queue
	//
        startFileSearch(startDirectory);	
    }

    public boolean expired() {

	return System.currentTimeMillis() > expiration ;
    }

    public void setExpired() {
	expiration = System.currentTimeMillis() ;
    }

    private void startFileSearch(final File startDirectory) {
	fileTask = new FutureTask<>(() -> {
		findFiles(startDirectory);
		return null;
	});
	start(fileTask);
    }

    private void findFiles(final File startDirectory) throws SecurityException, FileNotFoundException {
	int count ;
	File file ;

	try {
	    for (count = 0, file = dir.find() ;
		 file != null ; file = dir.find() ) {
		if (!fileTask.isCancelled()) {
		    fileLinkedBlockingQueue.offer(file);
		}
		if (count++ >= 10) {
		    listener.onNotifyEvent() ;
		    count = 0 ;
		}
	    }
	} catch (SecurityException | FileNotFoundException ex) {
	    lastError = dir.getLastErrorString() ;
	    state = State.FRESH ;
	    dir.close() ;
	    listener.onNotifyEvent() ;
	    throw ex ;
	}
	state = State.FRESH ;
	dir.close() ;
	listener.onNotifyEvent() ;
    }

    public void close() {
        if (fileTask != null) {
            fileTask.cancel(true);
        }
        fileLinkedBlockingQueue.clear();
        fileLinkedBlockingQueue = null;
        fileTask = null;
    }

    private void start(FutureTask<Void> futureTask) {
	new Thread(futureTask).start() ;
    }

    public File[] getListing() throws SecurityException, FileNotFoundException {
	try {
	    if (fileTask.isDone()) {
		fileTask.get() ;
	    }
	} catch (ExecutionException ex) {
	    if (ex.getCause() instanceof SecurityException)
		throw new SecurityException(ex.getMessage()) ;
	    else if (ex.getCause() instanceof FileNotFoundException)
		throw new FileNotFoundException(ex.getMessage()) ;
	} catch (InterruptedException ex) {
	    // ignore
	}
	return fileLinkedBlockingQueue.toArray(new File[0]) ;
    }

    public State getState() {
	return (state) ;
    }

    public String getLastErrorString() {
	return (lastError) ;
    }

}
