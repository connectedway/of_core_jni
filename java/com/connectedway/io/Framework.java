package com.connectedway.io ;

import java.lang.System ;

import java.util.UUID ;

import java.io.Serializable ;
import java.net.InetAddress ;

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
public class Framework implements Serializable
{
    /**
     * NetBIOS Interface Modes
     */
    public enum netBIOSMode {
	BMODE, PMODE, MMODE, HMODE
    }
    /**
     * Share Types
     */
    public enum shareType {
	DISK, PRINTER, PIPE, COMM, DEVICE
    }

    public enum mapType {
	WIN32, DARWIN, LINUX, FILEX, NUFILE, ANDROID,
	    OTHER, SMB, FILE, PIPE, MAILSLOT, ROOT, WORKGROUP,
	    SERVER, BOOKMARK, UNKNOWN
    }

    /**
     * Interface Configuration
     */
    public static class Interface {
	/**
	 * The NetBIOS Mode
	 */
	private final netBIOSMode _netBIOSMode ;
	/**
	 * The Interface's IP address
	 */
	private final InetAddress _ip ;
	/**
	 * The broadcast address for the interface
	 */
	private final InetAddress _bcast ;
	/**
	 * The network mask for the interface
	 */
	private final InetAddress _mask ;
	/**
	 * The name of the local master browser for the interface 
	 * if no advertising local master browsers are found
	 */
	private final String _defaultLmb ;
	/**
	 * The list of WINS servers for the interface
	 */
	private final InetAddress[] _wins;
	/**
	 * Constructor 
	 */
	public Interface (netBIOSMode _netBIOSMode, InetAddress _ip,
					  InetAddress _bcast, InetAddress _mask,
					  String _defaultLmb, InetAddress[] _wins) {
	    this._netBIOSMode = _netBIOSMode ;
	    this._ip = _ip ;
	    this._bcast = _bcast ;
	    this._mask = _mask ;
	    this._defaultLmb = _defaultLmb ;
	    this._wins = _wins ;
	}

	public netBIOSMode getNetBIOSMode() {
	    return _netBIOSMode ;
	}

	public InetAddress getIpAddress() {
	    return _ip ;
	}

	public InetAddress getBcastAddress() {
	    return _bcast ;
	}
	
	public InetAddress getMask() {
	    return _mask ;
	}

	public String getDefaultLmb() {
	    return _defaultLmb ;
	}

	public InetAddress[] getWins() {
	    return _wins ;
	}
    }

    /**
     * Path prefix map
     */
    public static class Map implements Serializable {

	/**
	 * The name of the prefix
	 */
	private final String _name ;
	/**
	 * The description of the map
	 */
	private final String _desc ;
	/**
	 * Destination to map the prefix to
	 */
	private final File _path ;
	/**
	 * File System Type
	 */
	private final mapType _mapType ;
	/**
	 * Thumbnail Mode (whether to display thumbnails)
	 */
	private final Boolean _thumbnail_mode ;
	/**
	 * Constructor
	 */
	public Map (String _name, String _desc, File _path, mapType _type,
		    boolean _thumbnail_mode) {
	    this._name = _name ;
	    this._desc = _desc ;
	    this._path = _path ;
	    this._mapType = _type ;
	    this._thumbnail_mode = _thumbnail_mode;
	}

	/**
	 * Copy Constructor
	 */
	public Map (Map amap) {
	    this(amap._name, amap._desc, amap._path, amap._mapType,
		 amap._thumbnail_mode) ;
	}

	public String getName() {
	    return _name ;
	}
	
	public File getPath() {
	    return _path ;
	}

	public mapType getType() {
	    return _mapType ;
	}

	public String getDescription() {
	    return _desc ;
	}
	public boolean getThumbnailMode() {
	    return Boolean.TRUE.equals(_thumbnail_mode) ;
	}

    }

    /**
     * Singleton constructor
     */
    private Framework () {
    }

    static private Framework ref = null ;

    /**
     * Return the Framework object
     */
    public static Framework getFramework() {
	if (ref == null)
	    ref = new Framework() ;
	return ref ;
    }

    /**
     * Initialize the Android SMB stack
     *
     * This should be done before any framework or io APIs are invoked.
     */
    public native void init () ;
    /**
     * Startup the Blue Share stack.  This should be done after all
     * configuration is done during initialization.  Updates to the
     * configuration can still be done after startup.
     */
    public native void startup() ;
    /**
     * Load configuration from a file
     */
    public native void load(File file) ;
    /**
     * Store configuration to a file
     */
    public native void save(File file) ;
    /**
     * Set the Hostname, Workgroup, and description for the current
     * node
     */
    public native void setHostname (String name, String workgroup, 
				    String description) ;
    /**
     * Get the Hostname of the current node
     */
    public native String getHostname() ;
    /**
     * Get the workgroup of the current node
     */
    public native String getWorkgroup() ;
    /**
     * Get the description of the current node
     */
    public native String getDescription() ;
    /**
     * Set the UUID of the node
     */
    public native void setUUID (UUID uuid) ;
    /**
     * Get the UUID of the node
     */
    public native UUID getUUID () ;
    /**
     * Get the Rood Directory for the server
     */
    public native String getRootDir() ;
    /**
     * Enable or Disable auto interface discovery.  When auto discovery
     * is enabled, the Blue Share stack will query the underlying platform
     * and enable all discovered interfaces.  If auto discovery is disabled,
     * then individual interfaces must be manually configured.
     */
    public native void setInterfaceDiscovery(boolean on) ;
    /**
     * Get the auto discovery state
     */
    public native boolean getInterfaceDiscovery() ;

    public Interface newInterface (netBIOSMode _netBIOSMode, InetAddress _ip,
								   InetAddress _bcast, InetAddress _mask,
								   String _defaultLmb, InetAddress[] _wins) {
	return new Interface (_netBIOSMode, _ip, _bcast, _mask,
			      _defaultLmb, _wins) ;
    }

    public native boolean setLMB (InetAddress _ip, String _Lmb) ;
    /**
     * Add an interface to the Blue Share stack
     */
    public native void addInterface(Interface iface) ;
    /**
     * Remove an interface from the Blue Share stack
     */
    public native void removeInterface(InetAddress ip) ;
    /**
     * Get all the interfaces that are currently configured
     */
    public native Interface[] getInterfaces() ;
    public Map newMap (String _name, String _desc, File _path, mapType _type,
		       boolean thumbnail) {
	return new Map(_name, _desc, _path, _type, thumbnail);
    }
    /**
     * Add a prefix mapping
     */
    public native boolean addMap (Map map) ;
    /**
     * Get a list of prefix mappings
     */
    public native Map[] getMaps () ;
    /**
     * Remove a prefix map
     */
    public native void removeMap (String name) ;

    /**
     * Notify the Blue Share components that a configuration change has been
     * made.  If multiple configuration changes are being made at one time,
     * the notify should be delayed until all configuration changes have
     * been made.
     */
    public native void update () ;

    public native void println(String output);

    public native void dumpHeap ();

    public native void statsHeap();

    public native void setInterfaceFilter (int ip) ;

    static {
	System.loadLibrary("of_core_jni") ;

	getFramework().init() ;
    }

}
