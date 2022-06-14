package com.connectedway.io ;

import java.util.Hashtable;
import java.io.FileInputStream;
import java.io.FileOutputStream;
import java.io.IOException ;
import java.io.FileNotFoundException;
import java.nio.channels.FileChannel;
import java.nio.channels.FileLock;
import java.nio.ByteBuffer;

import android.os.ParcelFileDescriptor;

import android.database.Cursor;

import android.content.ContentResolver;
import android.provider.DocumentsContract.Document;
import android.provider.DocumentsContract.Root;
import android.net.Uri;

/**
 * This class manages the configuration and initialization of the Blue 
 * Share components.
 *
 * The APIs provided allow a target system or application to
 * set configuration parameters, and for the BlueShare internal components
 * to retrieve these parameters.
 *
 * An optional capability provides the ability to store the configuration as 
 * a persistent XML file and to reload this upon startup.  
 *
 * @author Richard Schmitt
 * @since 3.1
 */
public class Resolver
{
    /**
     * Server Settings
     */
    public static class ResolverFile {
	private ParcelFileDescriptor pfdos;
	private ParcelFileDescriptor pfdis;
	private FileOutputStream fos;
	private FileInputStream fis;
	private FileChannel fcoc;
	private FileChannel fcic;
	private ContentResolver cr;
	private Uri uri;
	private String mode;

	public ResolverFile (ParcelFileDescriptor pfd)
	{
	    try {
		this.pfdos = pfd.dup();
		this.fos =
		    new ParcelFileDescriptor.AutoCloseOutputStream(pfdos);
		this.fcoc = this.fos.getChannel();

		this.pfdis = pfd.dup();
		this.fis =
		    new ParcelFileDescriptor.AutoCloseInputStream(pfdis);
		this.fcic = this.fis.getChannel();
	    }
	    catch (IOException except) {
	    }
	}

	public ResolverFile (ContentResolver cr, Uri uri, String Mode)
	{
	    this.cr = cr;
	    this.uri = uri;
	    this.mode = Mode;
	    this.pfdos = null;
	}

	private int check_open()
	{
	    int ret = 0;

	    if (this.pfdos == null) {
		try {
		    this.pfdos = this.cr.openFile(this.uri, this.mode, null);
		} catch (FileNotFoundException e) {
		    this.pfdos = null;
		}
		if (this.pfdos == null)
		    ret = -1;
		else {
		    try {
			this.fos =
			    new ParcelFileDescriptor.AutoCloseOutputStream(pfdos);
			this.fcoc = this.fos.getChannel();
	    
			this.pfdis = this.pfdos.dup();
			this.fis =
			    new ParcelFileDescriptor.AutoCloseInputStream(pfdis);
			this.fcic = this.fis.getChannel();
		    }
		    catch (IOException except) {
		    }
		}
	    }
	    return (ret);
	}

	public int write(byte b[], int len)
	{
	    int ret;
	    
	    ret = check_open();
	    
	    if (ret == 0) {
		ByteBuffer bb = ByteBuffer.wrap(b, 0, len);
		try {
		    ret = this.fcoc.write(bb);
		} catch (IOException e) {
		    ret = -1;
		}
	    }
	    return(ret);
	}

	public int pwrite(ByteBuffer bb, int len, int offset)
	{
	    int ret;
	    
	    ret = check_open();
	    
	    if (ret == 0) {
		try {
		    ret = this.fcoc.write(bb, offset);
		} catch (IOException e) {
		    ret = -1;
		}
	    }
	    return(ret);
	}

	public int read(byte b[], int len)
	{
	    int ret;

	    ret = check_open();
	    if (ret == 0) {
		ByteBuffer bb = ByteBuffer.wrap(b, 0, len);

		try {
		    ret = this.fcic.read(bb);
		    if (ret == -1)
			ret = 0;
		} catch (Exception e) {
		    ret = -1;
		}
	    }
	    return (ret);
	}

        public int pread(ByteBuffer bb, int len, int offset)
	{
	    int ret;

	    ret = check_open();
	    if (ret == 0) {
		try {
		    ret = this.fcic.read(bb, offset);
		    if (ret == -1)
			ret = 0;
		} catch (Exception e) {
		    ret = -1;
		}
	    }
	    return (ret);
	}

	public int close ()
	{
	    int ret;

	    if (this.pfdos == null) {
		ret = 0;
	    } else {
		try {
		    this.fis.close();
		    this.fos.close();
		    this.pfdis.close();
		    this.pfdos.close();
		    ret = 0;
		} catch (IOException e) {
		    ret = -1;
		}
	    }
	    return (ret);
	}

	public void flush()
	{
	    if (this.pfdos != null) {
		try {
		    this.fcoc.force(true);
		} catch (IOException e) {
		}
	    }
	}

	public long seek(long offset, int whence)
	{
	    long newpos;
	    if (check_open() != 0) {
		newpos = -1;
	    } else {
		/*
		 * 0 is from the beginning
		 * 1 is from where we are
		 * 2 is from the end
		 */
		try {
		    if (whence == 0) {
			newpos = offset;
			this.fcoc.position(newpos);
			this.fcic.position(newpos);
		    } else if (whence == 1) {
			long oldpos = this.fcoc.position();
			newpos = oldpos + offset;
			this.fcoc.position(newpos);
			this.fcic.position(newpos);
		    } else {
			long size = this.fcoc.size();
			newpos = size - offset;
			this.fcoc.position(newpos);
			this.fcic.position(newpos);
		    }
		} catch (IOException e) {
		    newpos = -1;
		}
	    }
	    return (newpos);
	}

	public void truncate(long offset)
	{
	    if (check_open() == 0) {
		try {
		    this.fcoc.truncate(offset);
		} catch (IOException e) {
		}
	    }
	}

	private class HashKey {
	    public long position;
	    public long size;

	    public HashKey(long position, long size) {
		this.position = position;
		this.size = size;
	    }

	    public int hashCode() {
		return (int) this.position;
	    }

	    public boolean equals (HashKey other) {
		boolean ret = false;
		if (this.position == other.position &&
		    this.size == other.size)
		    ret = true;
		return ret;
	    }
	}

	static Hashtable<HashKey,FileLock> hashtable =
	    new Hashtable<HashKey,FileLock>();

	public synchronized void lock(long position, long size, boolean shared)
	{
	    if (check_open() == 0) {
		try {
		    FileLock fl = this.fcoc.lock(position, size, shared);
		    HashKey key = new HashKey(position, size);
		    hashtable.put(key, fl);
		} catch (IOException e) {
		}
	    }
	}

	public synchronized boolean trylock(long position,
					    long size, boolean shared)
	{
	    boolean ret = false;
	    if (check_open() == 0) {
		try {
		    FileLock fl = this.fcoc.tryLock(position, size, shared);
		    HashKey key = new HashKey(position, size);
		    if (key != null) {
			hashtable.put(key, fl);
			ret = true;
		    }
		} catch (IOException e) {
		}
	    }
	    return ret;
	}

	public synchronized void unlock(long position, long size) {
	    if (check_open() == 0) {
		HashKey key = new HashKey(position, size);
		FileLock fl = hashtable.remove(key);
		try {
		    if (fl != null)
			fl.release();
		} catch (IOException e) {
		}
	    }
	}
    }

    public static class ResolverStat {
	public long Size;
	public int Flags;
	public long MTime;

	public static void fromCursor(Cursor cursor, ResolverStat stat)
	{
	    int columnId;

	    columnId = cursor.getColumnIndex(Document.COLUMN_SIZE);
	    if (columnId >= 0)
		stat.Size = cursor.getLong(columnId);

	    columnId = cursor.getColumnIndex(Document.COLUMN_LAST_MODIFIED);
	    if (columnId >= 0) {
		/*
		 * Returns # ms since EPOCH 1/1/1970.
		 * Want the equivalent of time_t which is # seconds
		 * since epch.
		 */
		stat.MTime = cursor.getLong(columnId) / 1000;
	    }
	    columnId = cursor.getColumnIndex(Document.COLUMN_FLAGS);
	    if (columnId >= 0)
		stat.Flags = cursor.getInt(columnId);
	}
    };

    public static class ResolverStatFS {
	public long Avail;
	public long Blocks;

	public static void fromCursor(Cursor cursor, ResolverStatFS statfs)
	{
	    int columnId;

	    columnId = cursor.getColumnIndex(Root.COLUMN_AVAILABLE_BYTES);
	    if (columnId >= 0)
		statfs.Avail = cursor.getLong(columnId);

	    columnId = cursor.getColumnIndex(Root.COLUMN_CAPACITY_BYTES);
	    if (columnId >= 0) {
		statfs.Blocks = cursor.getLong(columnId);
	    }
	}
    };

    public static class ResolverDir {
	private Cursor cursor;

	public ResolverDir(Cursor cursor)
	{
	    this.cursor = cursor;
	}

	public boolean Next()
	{
	    return (this.cursor.moveToNext());
	}

	public int close()
	{
	    int ret;

	    this.cursor.close();
	    ret = 0;
	    return (ret);
	}

	public Cursor getCursor()
	{
	    return (this.cursor);
	}

    };

    public static class ResolverDirent {
	public String Name;

	public ResolverDirent (Cursor cursor)
	{
	    int columnId;

	    columnId = cursor.getColumnIndex(Document.COLUMN_DISPLAY_NAME);
	    if (columnId >= 0)
		this.Name = cursor.getString(columnId);
	}
    };

    public abstract interface ResolverListener {
	public abstract ResolverFile Open (String FileName, String mode);
	public abstract int MkDir(String FileName);
	public abstract int Write(ResolverFile rFile, byte b[], int len);
	public abstract int PWrite(ResolverFile rFile, ByteBuffer bb, int len,
				   int offset);
	public abstract int Read(ResolverFile rFile, byte b[], int len);
	public abstract int PRead(ResolverFile rFile, ByteBuffer bb, int len,
                                  int offset);
	public abstract int Close(ResolverFile rFile);
	public abstract int Unlink(String FileName);
	public abstract int RmDir(String FileName);
	public abstract ResolverStat Stat(String FileName);
	public abstract ResolverStatFS StatFS(String FileName);
	public abstract ResolverDirent ReadDir(ResolverDir dir);
	public abstract ResolverDir OpenDir(String FileName);
	public abstract int CloseDir(ResolverDir dir);
	public abstract int Rename(String oldFile, String newFile);
	public abstract int Flush(ResolverFile rFile);
	public abstract long Seek(ResolverFile rFile, long offset, int whence);
	public abstract int Truncate(ResolverFile rFile, long offset);
	public abstract int Lock(ResolverFile rFile, long offset, long size,
				 boolean shared);
	public abstract int TryLock(ResolverFile rFile, long offset,
				    long size, boolean shared);
	public abstract int Unlock(ResolverFile rFile, long offset,
				    long size);
	//public abstract int StatFS(String FileName, ResolverStatFS stat);
    }

    public static native void setResolverListener(ResolverListener listener);
};

