package org.meshpoint.anode.util;

import java.io.File;
import java.io.FileInputStream;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.net.URI;
import java.security.MessageDigest;

import org.apache.http.HttpEntity;
import org.apache.http.client.HttpClient;
import org.apache.http.client.methods.HttpGet;
import org.apache.http.impl.client.DefaultHttpClient;
import org.meshpoint.anode.Constants;

import android.util.Log;

public class ModuleUtils {
	private static String TAG = "anode::ModuleUtils";

	/* module types */
	public static final int TYPE_UNKNOWN = -1;
	public static final int TYPE_JS      = 0;
	public static final int TYPE_NODE    = 1;
	public static final int TYPE_UNPACK  = 2;
	public static final int TYPE_ZIP     = TYPE_UNPACK;
	public static final int TYPE_TARBALL = 3;
	private static final int TYPE_MAX    = 4;
	
	private static final String[] extensions = new String[]{".js", ".node", ".zip", ".tar.gz"};
	
	/* for hashing names for tmp files */
	private static final int HASH_LEN = 20;
	
	/* cache dir for tmp and downloaded resources */
	private static File resourceDir = new File(Constants.RESOURCE_DIR);
	private static File moduleDir = new File(Constants.MODULE_DIR);
	
	public static int guessModuleType(String filename) {
		for(int i = 0; i < TYPE_MAX; i++) {
			if(filename.endsWith(extensions[i]))
				return i;
		}
		return TYPE_UNKNOWN;
	}

	public static String getExtensionForType(int type) {
		return extensions[type];
	}
	
	public static File getModuleFile(String module, int type) {
		if(type < TYPE_UNPACK)
			module += extensions[type];
		return new File(moduleDir, module);
	}
	
	public static boolean deleteModule(String module, int type) {
		File installLocation = null;
		if(type == TYPE_UNKNOWN) {
			if(!(installLocation = new File(moduleDir, module)).exists()) {
				for(int i = 0; i < TYPE_UNPACK; i++) {
					if((installLocation = new File(moduleDir, module)).exists())
						break;
				}
			}
		} else {
			installLocation = getModuleFile(module, type);
		}
		return (installLocation != null && installLocation.exists()) ? deleteFile(installLocation) : false;
	}

	public static boolean deleteFile(File location) {
		boolean result = true;
		if(location.exists()) {
			if(location.isDirectory()) {
				result = deleteDir(location);
			} else {
				result = location.delete();
			}
		}
		return result;
	}

	private static boolean deleteDir(File location) {
        for (String child : location.list()) {
            boolean result = deleteFile(new File(location, child));
            if (!result) {
                return false;
            }
        }
        return location.delete();
	}

	public static boolean copyFile(File src, File dest) {
		boolean result = true;
		if(src.isDirectory()) {
			result = copyDir(src, dest);
		} else {
			try {
				int count;
				byte[] buf = new byte[1024];
				FileInputStream fis = new FileInputStream(src);             
				FileOutputStream fos = new FileOutputStream(dest);             
				while ((count = fis.read(buf, 0, 1024)) != -1)
					fos.write(buf, 0, count);
			} catch(IOException e) {
				Log.v(TAG, "moveFile exception: aborting; exception: " + e + "; src = " + src.toString() + "; dest = " + dest.toString());
				return false;
			}
		}
		return result;
	}

	private static boolean copyDir(File src, File dest) {
		dest.mkdir();
        for (String child : src.list()) {
            boolean result = copyFile(new File(src, child), new File(dest, child));
            if (!result) {
                return false;
            }
        }
        return true;
	}

	public static File unpack(File moduleResource, String moduleName, int type) throws IOException {
		/* create temp dir to unpack; assume no hash collision */
		String tmpDirName = moduleName + extensions[type] + "-tmp";
		File result = new File(resourceDir, tmpDirName);
		if(!result.isDirectory() && !result.mkdir())
			throw new IOException("Unable to create tmp directory to unpack: " + result.toString());
		
		if(type == TYPE_ZIP) {
			(new ZipExtractor(moduleResource, result)).extract();
		} else if(type == TYPE_TARBALL) {
			(new TarExtractor(moduleResource, result)).extract();
		} else {
			Log.v(TAG, "ModuleUtils.unpack(): aborting (internal error)");
			result = null;
		}
		return result;
	}

	public static File getResource(URI httpUri, String filename) throws IOException {
		/* download */
		HttpClient http = new DefaultHttpClient();
		HttpGet request = new HttpGet();
		request.setURI(httpUri);
		HttpEntity entity = http.execute(request).getEntity();
		InputStream in = entity.getContent();
		File destination = new File(resourceDir, filename);
		FileOutputStream out = new FileOutputStream(destination);
		byte[] buf = new byte[1024];
		int read;
		while((read = in.read(buf)) != -1) {
			out.write(buf, 0, read);
		}
		in.close();
		out.flush();
		out.close();
		return destination;
	}
	
	public static String getResourceUriHash(String id) {
		try {
			MessageDigest sha = MessageDigest.getInstance("SHA-1");
			sha.update(id.getBytes("iso-8859-1"));
			return digest2Hex(sha.digest());
		}
		catch(Exception e) {
			return null;
		}
	}

	private static String digest2Hex(byte[] digest) {
		StringBuilder hash = new StringBuilder(HASH_LEN * 2);
		final String hexChars = "0123456789abcdef";
		for(int i = 0; i < HASH_LEN; i++) {
		      hash.append(hexChars.charAt((digest[i] & 0xF0) >> 4))
		         .append(hexChars.charAt((digest[i] & 0x0F)));
		}
		return hash.toString();
	}

}
