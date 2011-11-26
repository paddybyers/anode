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
	public static class ModuleType {
		public int type;
		public String extension;
		public Unpacker unpacker;
		public ModuleType(int type, String extension, Unpacker unpacker) {
			this.type = type;
			this.extension = extension;
			this.unpacker = unpacker;
		}
	}
	
	public static final int TYPE_JS   = 0;
	public static final int TYPE_NODE = 1;
	public static final int TYPE_DIR  = 2;
	public static final int TYPE_ZIP  = 3;
	public static final int TYPE_TAR  = 4;
	
	private static final ModuleType[] modTypes = new ModuleType[] {
		new ModuleType(TYPE_JS,   ".js",     null),
		new ModuleType(TYPE_NODE, ".node",   null),
		new ModuleType(TYPE_DIR,  "",        null),
		new ModuleType(TYPE_ZIP,  ".zip",    new ZipExtractor()),
		new ModuleType(TYPE_TAR,  ".tar.gz", new TarExtractor()),
		new ModuleType(TYPE_TAR,  ".tgz",    new TarExtractor())
	};
	
	public interface Unpacker {
		public void unpack(File src, File dest) throws IOException;
	}
		
	/* for hashing names and tmp files */
	private static int counter = 0;
	private static final int HASH_LEN = 20;
	
	/* cache dir for tmp and downloaded resources */
	private static File resourceDir = new File(Constants.RESOURCE_DIR);
	private static File moduleDir = new File(Constants.MODULE_DIR);
	
	public static ModuleType guessModuleType(String filename) {
		/* guess by extension first */
		for(ModuleType modType : modTypes) {
			if(!modType.extension.isEmpty() && filename.endsWith(modType.extension))
				return modType;
		}
		/* if it's a local directory, then it's type DIR */
		if((new File(filename)).isDirectory())
			return modTypes[TYPE_DIR];

		return null;
	}

	public static File getModuleFile(String module, ModuleType modType) {
		if(modType.unpacker == null)
			module += modType.extension;
		return new File(moduleDir, module);
	}
	
	public static File locateModule(String module, ModuleType modType) {
		File installLocation = null, candidateLocation;
		if(modType == null) {
			for(ModuleType type : modTypes) {
				/* a small cheat, since all unpacked types here match as TYPE_DIR */
				if((candidateLocation = new File(moduleDir, module + type.extension)).exists()) {
					installLocation = candidateLocation;
					break;
				}
			}
		} else {
			candidateLocation = getModuleFile(module, modType);
			if(candidateLocation.exists())
				installLocation = candidateLocation;
		}
		return installLocation;
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

	public static File unpack(File moduleResource, String moduleName, ModuleType modType) throws IOException {
		/* create temp dir to unpack; assume no hash collision */
		String tmpDirName = moduleName + '-' + String.valueOf(counter++) + "-tmp";
		File result = new File(resourceDir, tmpDirName);
		if(!result.isDirectory() && !result.mkdir())
			throw new IOException("Unable to create tmp directory to unpack: " + result.toString());
		
		if(modType.unpacker != null) {
			modType.unpacker.unpack(moduleResource, result);
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
