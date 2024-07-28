package com.connectedway.io ;

import java.lang.System ;

import java.util.UUID ;

import java.io.Serializable ;
import java.net.InetAddress ;
import java.io.FileInputStream;
import java.io.FileOutputStream;
import java.io.FileNotFoundException;
import java.io.IOException;
    
import java.security.KeyStore;
import java.security.KeyStoreException;
import java.security.NoSuchAlgorithmException;
import java.security.NoSuchProviderException;
import java.security.InvalidAlgorithmParameterException;
import java.security.InvalidKeyException;
import java.security.UnrecoverableKeyException;
import java.security.cert.CertificateException;

import javax.crypto.BadPaddingException;
import javax.crypto.Cipher;
import javax.crypto.IllegalBlockSizeException;
import javax.crypto.KeyGenerator;
import javax.crypto.NoSuchPaddingException;
import javax.crypto.SecretKey;
import javax.crypto.spec.GCMParameterSpec;

import android.util.Log;
import android.security.keystore.KeyProperties;
import android.security.keystore.KeyGenParameterSpec;

import com.connectedway.io.File;
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
     * Set the Network Handle for the JNI code.  After API 31, the NDK
     * provides a call for this but until then, we need to pass it in.
     */
    public native void setNetworkHandle(long handle);
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

    private File configFile;
    private File configDir;
    private SecretKey configKey;
    static byte[] configIV ;
    private final String CONFIG_KEY_ALIAS = "_com.spiritcloud.app_config";

    public native byte[] getConfig() ;
    public native void putConfig(byte[] plainConfig);
    public native void setConfigPath(String path);

    public File getConfigDir() {
        return (this.configDir);
    }

    public void setConfigFile(File configFile) {
        setConfigPath(configFile.getPath());
        this.configFile = configFile;
        this.configDir = configFile.getParentFile();

        File ivFile = new File (this.configDir, "config.iv");
        FileInputStream inputStreamIv;
        this.configIV = new byte[(int)ivFile.length()];
        try {
            inputStreamIv = new FileInputStream(ivFile);
            inputStreamIv.read(this.configIV);
            inputStreamIv.close();
        } catch (IOException e) {
            Log.e(Framework.class.getSimpleName(),
                  "Iv File Not Found");
        }
    }

    public Boolean configExists() {
        return (this.configFile.exists());
    }

    public void setConfigKey(SecretKey configKey) {
        this.configKey = configKey;
    }
    
    public void loadConfigFile()
    throws IOException {
        GCMParameterSpec gcmParameterSpec =
            new GCMParameterSpec(128, this.configIV);
        Cipher configCipher;
        try {
            configCipher =
                Cipher.getInstance("AES/GCM/NoPadding");
        } catch (NoSuchAlgorithmException | NoSuchPaddingException e) {
            Log.e(Framework.class.getSimpleName(),
                  "Unable to get AES Cipher");
            throw new IOException();
        }

        try {
            configCipher.init(Cipher.DECRYPT_MODE, this.configKey,
                              gcmParameterSpec);
        } catch (InvalidAlgorithmParameterException | InvalidKeyException e) {
            Log.e(Framework.class.getSimpleName(),
                  "Unable to Initialize AES Cipher");
            throw new IOException();
        }
        FileInputStream inputStreamConfig;
        try {
            inputStreamConfig = new FileInputStream(configFile);
        } catch (FileNotFoundException e) {
            Log.e(Framework.class.getSimpleName(),
                  "Config File Not Found");
            throw new IOException();
        }
        byte[] secretConfig = new byte[(int)configFile.length()];
        try {
            inputStreamConfig.read(secretConfig);
            inputStreamConfig.close();
        } catch (IOException e) {
            Log.e(Framework.class.getSimpleName(),
                  "Couldn't Read from Config File");
            throw new IOException();
        }
            
        byte[] plainConfig;
        try {
            plainConfig = configCipher.doFinal(secretConfig);
            putConfig(plainConfig);
        } catch (BadPaddingException | IllegalBlockSizeException e) {
            Log.e(Framework.class.getSimpleName(),
                  "Couldn't Decrypt Config File, Trying plain text");
            putConfig(secretConfig);
        }
    }

    public void saveConfigFile()
    throws IOException {
        Cipher configCipher;
        boolean cipher_init = false;
        try {
            configCipher = Cipher.getInstance("AES/GCM/NoPadding");
        } catch (NoSuchAlgorithmException | NoSuchPaddingException e) {
            Log.e(Framework.class.getSimpleName(),
                  "Unable to get AES Cipher");
            throw new IOException();
        }
        while (!cipher_init) {
            try {
                configCipher.init(Cipher.ENCRYPT_MODE, this.configKey);
                cipher_init = true;
            } catch (InvalidKeyException e) {
                Log.e(Framework.class.getSimpleName(),
                      "Unable to Initialize AES Cipher or save.  Creating new Key");
                newConfigKey();
            }
        }

        configIV = configCipher.getIV();
        File ivFile = new File(this.configDir, "config.iv");
        FileOutputStream ivOutputStream ;
        try {
            ivOutputStream = new FileOutputStream (ivFile);
        } catch (IOException e) {
            Log.e(Framework.class.getSimpleName(),
                  "Couldn't Open IV File");
            throw new IOException();
        }

        try {
            ivOutputStream.write(this.configIV);
            ivOutputStream.close();
        } catch (IOException e) {
            Log.e(Framework.class.getSimpleName(),
                  "Couldn't Write to IV File");
            throw new IOException();
        }

        byte[] plainConfig = getConfig() ;
        byte[] secretConfig;
        try {
            secretConfig = configCipher.doFinal(plainConfig);
        } catch (BadPaddingException | IllegalBlockSizeException e) {
            Log.e(Framework.class.getSimpleName(),
                  "Couldn't Encrypt Config File");
            throw new IOException();
        }

        FileOutputStream outputStreamConfig;
        try {
            outputStreamConfig =  new FileOutputStream (configFile);
        } catch (FileNotFoundException e) {
            Log.e(Framework.class.getSimpleName(),
                  "Config File Not Found");
            throw new IOException();
        }
        try {
            outputStreamConfig.write(secretConfig);
            outputStreamConfig.close();
        } catch (IOException e) {
            Log.e(Framework.class.getSimpleName(),
                  "Couldn't Write to Config File");
            throw new IOException();
        }
    }

    public void getConfigKey() {
        KeyStore keyStore;
        try {
            keyStore = KeyStore.getInstance("AndroidKeyStore");
        } catch (KeyStoreException e) {
            Log.e(Framework.class.getSimpleName(),
                  "Could Not Android Key Store Instance");
            return ;
        }
            
        try {
            keyStore.load(null);
        } catch (CertificateException |
                 IOException |
                 NoSuchAlgorithmException e) {
            Log.e(Framework.class.getSimpleName(),
                  "Could Not Load Android Key Store");
            return ;
        }

        try {
            this.configKey =
                (SecretKey) keyStore.getKey(CONFIG_KEY_ALIAS, null);
        } catch (KeyStoreException |
                 NoSuchAlgorithmException |
                 UnrecoverableKeyException e) {
            Log.e(Framework.class.getSimpleName(),
                  "Could Not Get Config Key");
            return ;
        }
    }

    public void newConfigKey() {
        KeyGenerator keyGenerator ;
        try {
            keyGenerator =
                KeyGenerator.getInstance(KeyProperties.KEY_ALGORITHM_AES,
                                         "AndroidKeyStore");
        } catch (NoSuchAlgorithmException | NoSuchProviderException e) {
            Log.e(Framework.class.getSimpleName(),
                  "Unable to get AES Cipher");
            return ;
        }

        KeyGenParameterSpec keySpec =
            new KeyGenParameterSpec.Builder("_com.spiritcloud.app_config",
                                            KeyProperties.PURPOSE_ENCRYPT |
                                            KeyProperties.PURPOSE_DECRYPT)
            .setBlockModes(KeyProperties.BLOCK_MODE_GCM)
            .setEncryptionPaddings(KeyProperties.ENCRYPTION_PADDING_NONE)
            .build();
        try {
            keyGenerator.init(keySpec);
        } catch (InvalidAlgorithmParameterException e) {
            Log.e(Framework.class.getSimpleName(),
                  "Could Not Intialize Config Key Generator");
            return ;
        }

        this.configKey = keyGenerator.generateKey();
    }
}

