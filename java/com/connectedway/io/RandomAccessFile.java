package com.connectedway.io ;

import java.io.FileNotFoundException ;
import java.io.IOException ;
import java.io.EOFException ;
import java.lang.SecurityException ;
import java.io.DataOutput ;
import java.io.DataInput ;
import java.io.Closeable ;
import java.io.DataInputStream ;

import com.connectedway.io.FileSystem ;
import com.connectedway.io.FileDescriptor ;

/**
 * Instances of this class support both reading and writing to a
 * random access file. 
 * 
 * @see java.io.RandomAccessFile
 */
public class RandomAccessFile implements DataOutput, DataInput, Closeable {

    private FileDescriptor fd ;
    private FileSystem fs = FileSystem.getFileSystem() ;
    
    /**
     * Creates a random access file stream to read from, and optionally
     * to write to, a file with the specified name. 
     *
     * @see java.io.RandomAccessFile#RandomAccessFile(String, String)
     */
    public RandomAccessFile (String name, String mode) 
	throws FileNotFoundException, SecurityException {

	int imode ;

	if (mode.indexOf('r') != -1) {
	    if (mode.indexOf('w') != -1) {
		imode = FileSystem.OPEN_RW;
	    } else {
		imode = FileSystem.OPEN_READ;
	    }
	} else {
	    imode = FileSystem.OPEN_WRITE;
	}

	fd = fs.open (name, imode) ;
    }

    /**
     * Creates a random access file stream to read from, and optionally to
     * write to, the file specified by the {@link BlueFile} argument. 
     *
     * @see java.io.RandomAccessFile#RandomAccessFile(File, String)
     */
    public RandomAccessFile (File file, String mode) 
	throws FileNotFoundException {

	this(file.getPath(), mode) ;
    }
    
    /**
     * Returns the opaque file descriptor object associated with this
     * stream. 
     *
     * @see java.io.RandomAccessFile#getFD()
     */
    public FileDescriptor getFD() throws IOException {
        return fd ;
    }
    /**
     * Returns the <code>FileChannel</code> object for this input stream
     *
     * @see java.io.FileOutputStream#getChannel()
     */
    public java.nio.channels.FileChannel getChannel() {
	return null ;
    }
    /**
     * Reads a byte of data from this file. 
     *
     * @see java.io.RandomAccessFile#read()
     */
    public int read() throws IOException {
        return fs.read(fd) ;
    }
    
    /**
     * Reads up to <code>len</code> bytes of data from this file into an
     * array of bytes. 
     *
     * @see java.io.RandomAccessFile#read(byte[], int, int)
     */
    public int read(byte b[], int off, int len) throws IOException {
        return fs.read(fd, b, off, len) ;
    }
    
    /**
     * Reads up to <code>b.length</code> bytes of data from this file
     * into an array of bytes. 
     *
     * @see java.io.RandomAccessFile#read(byte[])
     */
    public int read(byte b[]) throws IOException {
        return fs.read(fd, b, 0, b.length) ;
    }
    
    /**
     * Reads <code>b.length</code> bytes from this file into the byte
     * array, starting at the current file pointer. 
     *
     * @see java.io.RandomAccessFile#readFully(byte[])
     */
    public void readFully(byte b[]) throws IOException {
        readFully(b, 0, b.length) ;
    }
    
    /**
     * Reads exactly <code>len</code> bytes from this file into the byte
     * array, starting at the current file pointer. 
     *
     * @see java.io.RandomAccessFile#readFully(byte[], int, int)
     */
    public void readFully (byte b[], int off, int len) throws IOException {
	int i = 0 ;
	do {
	    int count = this.read (b, off + i, len - i) ;
	    if (count < 0)
		throw new EOFException() ;
	    i += count ;
	}
	while (i < len) ;
    }

    /**
     * Attempts to skip over <code>n</code> bytes of input discarding the
     * skipped bytes.
     *
     * @see java.io.RandomAccessFile#skipBytes(int)
     */
    public int skipBytes (int n) throws IOException {
	int ret ;

	if (n <= 0)
	    ret = 0 ;
	else {
	    long oldpos = fs.seek (fd, FileSystem.SEEK_CUR, 0) ;
	    long newpos = fs.seek (fd, FileSystem.SEEK_CUR, n) ;
	    ret = (int)(newpos - oldpos) ;
	}
	return (ret) ;
    }
    
    /**
     * Writes the specified byte to this file. The write starts at
     * the current file pointer.
     *
     * @see java.io.RandomAccessFile#write(int)
     */
    public void write (int b) throws IOException {
	fs.write(fd, b) ;
    }
    
    /**
     * Writes <code>b.length</code> bytes from the specified byte array
     * to this file, starting at the current file pointer.
     *
     * @see java.io.RandomAccessFile#write(byte[])
     */
    public void write (byte b[]) throws IOException {
	fs.write (fd, b, 0, b.length) ;
    }
    
    /**
     * Writes <code>len</code> bytes from the specified byte array
     * starting at offset <code>off</code> to this file.
     *
     * @see java.io.RandomAccessFile#write(byte[], int, int)
     */
    public void write (byte b[], int off, int len) throws IOException {
	fs.write (fd, b, off, len) ;
    }
    
    /**
     * Returns the current offset in this file.
     *
     * @see java.io.RandomAccessFile#getFilePointer()
     */
    public long getFilePointer() throws IOException {
	return fs.seek(fd, FileSystem.SEEK_CUR, 0) ;
    }
    
    /**
     * Sets the file-pointer offset, measured from the beginning of this
     * file, at which the next read or write occurs.
     *
     * @see java.io.RandomAccessFile#seek(long)
     */
    public void seek (long pos) throws IOException {
	fs.seek (fd, FileSystem.SEEK_SET, pos) ;
    }
    
    /**
     * Returns the length of this file.
     *
     * @see java.io.RandomAccessFile#length()
     */
    public long length() throws IOException {
	return fs.seek (fd, FileSystem.SEEK_END, 0) ;
    }
    
    /**
     * Sets the length of this file.
     *
     * @see java.io.RandomAccessFile#setLength(long)
     */
    public void setLength(long newLength) throws IOException {
	fs.seteof (fd, newLength) ;
    }
    
    /**
     * Closes this random access file stream and releases any system
     * resources associated with the stream.
     *
     * @see java.io.RandomAccessFile#close()
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
     * Reads a <code>boolean</code> from this file. 
     *
     * @see java.io.RandomAccessFile#readBoolean()
     */
    public boolean readBoolean() throws IOException {
	int ch = this.read();
	if (ch < 0)
	    throw new EOFException();
	return (ch != 0);
    }
    
    /**
     * Reads a signed eight-bit value from this file.
     *
     * @see java.io.RandomAccessFile#readByte()
     */
    public byte readByte() throws IOException {
	int ch = this.read();
	if (ch < 0)
	    throw new EOFException();
	return (byte)(ch);
    }
    
    /**
     * Reads an unsigned eight-bit number from this file.
     *
     * @see java.io.RandomAccessFile#readUnsignedByte()
     */
    public int readUnsignedByte() throws IOException {
	int ch = this.read();
	if (ch < 0)
	    throw new EOFException();
	return ch;
    }
    
    /**
     * Reads a signed 16-bit number from this file. 
     *
     * @see java.io.RandomAccessFile#readShort()
     */
    public short readShort() throws IOException {
	int ch1 = this.read();
	int ch2 = this.read();
	if ((ch1 | ch2) < 0)
	    throw new EOFException();
	return (short)((ch1 << 8) + (ch2 << 0));
    }
    
    /**
     * Reads an unsigned 16-bit number from this file.
     *
     * @see java.io.RandomAccessFile#readUnsignedShort()
     */
    public int readUnsignedShort() throws IOException {
	int ch1 = this.read();
	int ch2 = this.read();
	if ((ch1 | ch2) < 0)
	    throw new EOFException();
	return (ch1 << 8) + (ch2 << 0);
    }
    
    /**
     * Reads a character from this file.
     *
     * @see java.io.RandomAccessFile#readChar()
     */
    public char readChar() throws IOException {
	int ch1 = this.read();
	int ch2 = this.read();
	if ((ch1 | ch2) < 0)
	    throw new EOFException();
	return (char)((ch1 << 8) + (ch2 << 0));
    }
    
    /**
     * Reads a signed 32-bit integer from this file.
     *
     * @see java.io.RandomAccessFile#readInt()
     */
    public int readInt() throws IOException {
	int ch1 = this.read();
	int ch2 = this.read();
	int ch3 = this.read();
	int ch4 = this.read();
	if ((ch1 | ch2 | ch3 | ch4) < 0)
	    throw new EOFException();
	return ((ch1 << 24) + (ch2 << 16) + (ch3 << 8) + (ch4 << 0));
    }
    
    /**
     * Reads a signed 64-bit integer from this file. 
     *
     * @see java.io.RandomAccessFile#readLong()
     */
    public long readLong() throws IOException {
	return ((long)(readInt()) << 32) + (readInt() & 0xFFFFFFFFL);
    }
    
    /**
     * Reads a <code>float</code> from this file. 
     *
     * @see java.io.RandomAccessFile#readFloat()
     */
    public float readFloat() throws IOException {
	return Float.intBitsToFloat(readInt());
    }
    
    /**
     * Reads a <code>double</code> from this file. 
     *
     * @see java.io.RandomAccessFile#readDouble()
     */
    public double readDouble() throws IOException {
	return Double.longBitsToDouble(readLong());
    }
    
    /**
     * Reads the next line of text from this file.
     *
     * @see java.io.RandomAccessFile#readLine()
     */
    public String readLine() throws IOException {
	StringBuffer input = new StringBuffer();
	int c = -1;
	boolean eol = false;

	while (!eol) {
	    switch (c = read()) {
	    case -1:
	    case '\n':
		eol = true;
		break;
	    case '\r':
		eol = true;
		long cur = getFilePointer();
		if ((read()) != '\n') {
		    seek(cur);
		}
		break;
	    default:
		input.append((char)c);
		break;
	    }
	}

	if ((c == -1) && (input.length() == 0)) {
	    return null;
	}
	return input.toString();
    }
    
    /**
     * Reads in a string from this file. 
     *
     * @see java.io.RandomAccessFile#readUTF()
     */
    public String readUTF() throws IOException {
	return DataInputStream.readUTF(this);
    }
    
    /**
     * Writes a <code>boolean</code> to the file as a one-byte value. 
     *
     * @see java.io.RandomAccessFile#writeBoolean(boolean)
     */
    public void writeBoolean (boolean v) throws IOException {
	write(v ? 1 : 0);
    }
    
    /**
     * Writes a <code>byte</code> to the file as a one-byte value.
     *
     * @see java.io.RandomAccessFile#writeByte(int)
     */
    public void writeByte (int v) throws IOException {
	write(v);
    }
    
    /**
     * Writes a <code>short</code> to the file as two bytes, high byte first.
     *
     * @see java.io.RandomAccessFile#writeShort(int)
     */
    public void writeShort (int v) throws IOException {
	write((v >>> 8) & 0xFF);
	write((v >>> 0) & 0xFF);
    }
    
    /**
     * Writes a <code>char</code> to the file as a two-byte value, high
     * byte first. 
     *
     * @see java.io.RandomAccessFile#writeChar(int)
     */
    public void writeChar (int v) throws IOException {
	write((v >>> 8) & 0xFF);
	write((v >>> 0) & 0xFF);
    }
    
    /**
     * Writes an <code>int</code> to the file as four bytes, high byte first.
     *
     * @see java.io.RandomAccessFile#writeInt(int)
     */
    public void writeInt (int v) throws IOException {
	write((v >>> 24) & 0xFF);
	write((v >>> 16) & 0xFF);
	write((v >>>  8) & 0xFF);
	write((v >>>  0) & 0xFF);
    }
    
    /**
     * Writes a <code>long</code> to the file as eight bytes, high byte first.
     *
     * @see java.io.RandomAccessFile#writeLong(long)
     */
    public void writeLong (long v) throws IOException {
	write((int)(v >>> 56) & 0xFF);
	write((int)(v >>> 48) & 0xFF);
	write((int)(v >>> 40) & 0xFF);
	write((int)(v >>> 32) & 0xFF);
	write((int)(v >>> 24) & 0xFF);
	write((int)(v >>> 16) & 0xFF);
	write((int)(v >>>  8) & 0xFF);
	write((int)(v >>>  0) & 0xFF);
    }

    /**
     * Converts the float argument to an <code>int</code> using the
     * <code>floatToIntBits</code> method in class <code>Float</code>,
     * and then writes that <code>int</code> value to the file as a
     * four-byte quantity, high byte first. 
     *
     * @see java.io.RandomAccessFile#writeFloat(float)
     */
    public void writeFloat (float v) throws IOException {
	writeInt(Float.floatToIntBits(v));
    }
    
    /**
     * Converts the double argument to a <code>long</code> using the
     * <code>doubleToLongBits</code> method in class <code>Double</code>,
     * and then writes that <code>long</code> value to the file as an
     * eight-byte quantity, high byte first. 
     *
     * @see java.io.RandomAccessFile#writeDouble(double)
     */
    public void writeDouble (double v) throws IOException {
	writeLong(Double.doubleToLongBits(v));
    }
    
    /**
     * Writes the string to the file as a sequence of bytes. 
     *
     * @see java.io.RandomAccessFile#writeBytes(String)
     */
    public void writeBytes (String s) throws IOException {
	byte[] b = s.getBytes () ;
	fs.write(fd, b, 0, b.length);
    }
    
    /**
     * Writes a string to the file as a sequence of characters. 
     *
     * @see java.io.RandomAccessFile#writeChars(String)
     */
    public void writeChars (String s) throws IOException {
	int clen = s.length();
	int blen = 2*clen;
	byte[] b = new byte[blen];
	char[] c = new char[clen];
	s.getChars(0, clen, c, 0);
	for (int i = 0, j = 0; i < clen; i++) {
	    b[j++] = (byte)(c[i] >>> 8);
	    b[j++] = (byte)(c[i] >>> 0);
	}
	fs.write(fd, b, 0, blen);
    }
    
    /**
     * Writes a string to the file 
     *
     * @see java.io.RandomAccessFile#writeUTF(String)
     */
    public void writeUTF(String s) throws IOException {
	byte b[] ;

	b = s.getBytes("UTF-8") ;
	fs.write (fd, b, 0, b.length) ;
    }

    protected void finalize() throws IOException {
	if (fd != null) {
	    close();
	    fd = null ;
	}
    }
}
