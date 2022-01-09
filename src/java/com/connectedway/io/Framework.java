package com.connectedway.io ;

import java.lang.System ;

import java.util.UUID ;

import java.io.Serializable ;
import java.net.InetAddress ;

import com.connectedway.io.File ;

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
	BMODE, PMODE, MMODE, HMODE ;
    }
    /**
     * Server Authentication Modes
     */
    public enum authenticationMode {
	DISABLED, SERVER, USER ;
    }
    /**
     * Share Types
     */
    public enum shareType {
	DISK, PRINTER, PIPE, COMM, DEVICE ;
    }

    public enum mapType {
	WIN32, DARWIN, LINUX, FILEX, NUFILE, ANDROID,
	    OTHER, SMB, FILE, PIPE, MAILSLOT, ROOT, WORKGROUP,
	    SERVER, BOOKMARK, UNKNOWN ;
    }

    /**
     * A proxy for the CIFS Server
     */
    public class ProxyGateway {
	public ProxyGateway (File _path) {
	    this._path = _path ;
	}

	public File getPath() {
	    return _path ;
	}
	/**
	 * The destination to export
	 */
	private File _path ;
    }

    /**
     * A export for the CIFS Server
     */
    public class Export implements Serializable {
	public Export (shareType _shareType, String _share, File _path,
		       String _comment, String _fsName) {
	    this._shareType = _shareType ;
	    this._share = _share ;
	    this._path = _path ;
	    this._comment = _comment ;
	    this._fsName = _fsName ;
	}

	public native void test() ;

	public shareType getShareType() {
	    return _shareType ;
	}

	public String getShare() {
	    return _share ;
	}

	public File getPath() {
	    return _path ;
	}

	public String getComment() {
	    return _comment ;
	}

	public String getFsName() {
	    return _fsName ;
	}
	/**
	 * The type of share to export
	 */
	private shareType _shareType ;
	/**
	 * The name of the share
	 */
	private String _share ;
	/**
	 * The destination to export
	 */
	private File _path ;
	/**
	 * Comment to associate with the export
	 */
	private String _comment ;
	/**
	 * Type of Filesystem (HPFS, FAT, etc.)
	 */
	private String _fsName ;
    
    }

    /**
     * Server Settings
     */
    public static native boolean getServerEnabled() ;
    public static native String getServerUsername() ;
    public static native String getServerPassword() ;
    public static native void setServerEnabled(boolean enabled);
    public static native void setServerUsername(String username);
    public static native void setServerPassword(String password);

    /**
     * Interface Configuration
     */
    public class Interface {
	/**
	 * The NetBIOS Mode
	 */
	private netBIOSMode _netBIOSMode ;
	/**
	 * The Interface's IP address
	 */
	private InetAddress _ip ;
	/**
	 * The broadcast address for the interface
	 */
	private InetAddress _bcast ;
	/**
	 * The network mask for the interface
	 */
	private InetAddress _mask ;
	/**
	 * The name of the local master browser for the interface 
	 * if no advertising lmbs are found
	 */
	private String _defaultLmb ;
	/**
	 * The list of WINS servers for the interface
	 */
	private InetAddress _wins[] ;
	/**
	 * Constructor 
	 */
	public Interface (netBIOSMode _netBIOSMode, InetAddress _ip,
			  InetAddress _bcast, InetAddress _mask,
			  String _defaultLmb, InetAddress _wins[]) {
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
    public class Map implements Serializable {

	/**
	 * The name of the prefix
	 */
	private String _name ;
	/**
	 * The description of the map
	 */
	private String _desc ;
	/**
	 * Destination to map the prefix to
	 */
	private File _path ;
	/**
	 * File System Type
	 */
	private mapType _mapType ;

	/**
	 * Constructor
	 */
	public Map (String _name, String _desc, File _path, mapType _type) {
	    this._name = _name ;
	    this._desc = _desc ;
	    this._path = _path ;
	    this._mapType = _type ;
	}

	/**
	 * Copy Constructor
	 */
	public Map (Map amap) {
	    this(amap._name, amap._desc, amap._path, amap._mapType) ;
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

    }

    /**
     * Remote Node
     */
    public class Remote implements Serializable {

	/**
	 * The name of the Remote
	 */
	private String _name ;
	/**
	 * The IP Address of the remote
	 */
	private String _ip ;
	/**
	 * The port
	 */
	int _port ;
	/**
	 * Constructor
	 */
	public Remote (String _name, String _ip, int _port) {
	    this._name = _name ;
	    this._ip = _ip ;
	    this._port = _port ;
	}

	/**
	 * Copy Constructor
	 */
	public Remote (Remote aremote) {
	    this(aremote._name, aremote._ip, aremote._port) ;
	}

	public String getName() {
	    return _name ;
	}
	
	public String getIP() {
	    return _ip ;
	}

	public int getPort() {
	    return _port ;
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
     * configuration is done during iniitialization.  Updates to the
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
				   String _defaultLmb, InetAddress _wins[]) {
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
    public native InetAddress[] getMyAddresses() ;
    /**
     * Set the server's authentication mode
     */
    public native void setAuthenticationMode(authenticationMode mode) ;
    /**
     * Get the server's authentication mode
     */
    public native authenticationMode getAuthenticationMode() ;

    public Export newExport (shareType _shareType, String _share, 
			     File _path,
			     String _comment, String _fsName) {
	return new Export (_shareType, _share, _path, _comment, _fsName) ;
    }

    public ProxyGateway newProxyGateway (File _path) {
	return new ProxyGateway (_path) ;
    }
    /**
     * Add an export to the Blue Share stack
     */
    public native void addExport (Export export) ;
    /**
     * Add an export to the Blue Share stack
     */
    public native void addProxyGateway (ProxyGateway proxy) ;
    /**
     * Get a list of all exports
     */
    public native Export[] getExports() ;
    /**
     * Remove a particular export
     */
    public native void removeExport (String share) ;

    public Map newMap (String _name, String _desc, File _path, mapType _type) {
	return new Map (_name, _desc, _path, _type) ;
    }
    /**
     * Add a prefix mapping
     */
    public native void addMap (Map map) ;
    /**
     * Get a list of prefix mappings
     */
    public native Map[] getMaps () ;
    /**
     * Remove a prefix map
     */
    public native void removeMap (String name) ;

    public Remote newRemote (String _name, String _ip, int _port) {
	return new Remote (_name, _ip, _port) ;
    }
    /**
     * Add a Remote
     */
    public native void addRemote (Remote remote) ;
    /**
     * Get a list of remotes
     */
    public native Remote[] getRemotes () ;
    /**
     * Remove a prefix map
     */
    public native void removeRemote (String name) ;

    /**
     * Notify the Blue Share components that a configuration change has been
     * made.  If multiple configuration changes are being made at one time,
     * the notify should be delayed until all configuration changes have
     * been made.
     */
    public native void update () ;

    static {
	System.loadLibrary("of_core_jni") ;

	getFramework().init() ;
    }

}
