package org.meshpoint.anode;

import java.io.File;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.net.URI;
import java.net.URISyntaxException;
import java.security.MessageDigest;
import java.util.HashMap;
import java.util.Map;
import java.util.Set;

import org.apache.http.HttpEntity;
import org.apache.http.client.HttpClient;
import org.apache.http.client.methods.HttpGet;
import org.apache.http.impl.client.DefaultHttpClient;

import android.os.Bundle;
import android.util.Log;

/**
 * A class that processes a given commandline together with an optional
 * mapping of resource identifiers and URLs to fetch those resources.
 * 
 * Resources are limited to those that can be fetched with an http GET.
 * 
 * Downloaded resources are cached in an asset directory specific to the
 * application, obtained using the supplied Context.
 * Cached resources are stored in files whose names are a hash of the
 * originating URL. It is safe to reuse the cache directory across
 * multiple invocations of the same app, or across invocations of different
 * apps.
 * 
 * No processing is performed of expiry times of the specified resources
 * so they are downloaded again on each invocation.
 * 
 * Name substitution is performed in the supplied commandline, to map
 * resource specifiers to the local cache filename.
 * 
 * A resource is identified with a Bundle entry of:
 *     get:<key> = <value>
 * and is referenced in the commandline by:
 *     %<key>
 */
public class ArgProcessor {
	
	private static String TAG = "anode::ArgsProcessor";
	private static final String GET_PREFIX   = "get:";
	private static final String RESOURCE_DIR = "/data/data/org.meshpoint.anode/uriCache";
	private static final int    HASH_LEN     = 20;

	private Bundle extras;
	private String cmdline;
	private Map<String, URI> uriMap;
	private Map<String, String> filenameMap;

	/**
	 * Constructs an instance of ArgProcessor
	 * @param extras an optional bundle containing the mapping parameters
	 * @param cmdline the commandline to process
	 */
	ArgProcessor(Bundle extras, String cmdline) {
		this.extras = extras;
		this.cmdline = cmdline;
		uriMap = new HashMap<String, URI>();
		filenameMap = new HashMap<String, String>();
	}
	
	/**
	 * Process the commandline
	 * @return the processed commandline, with 
	 */
	String[] process() {
		if(extras != null) {
			/* extract list of args to get */
			try {
				Set<String> keys = extras.keySet();
				for(String key : keys) {
					if(key.startsWith(GET_PREFIX)) {
						String rawUri = extras.getString(key);
						String rawKey = key.substring(GET_PREFIX.length());
						URI uri = new URI(rawUri);
						String filename = getResourceUriHash(rawUri);
						uriMap.put(rawKey, uri);
						filenameMap.put(rawKey, filename);
					}
				}
			} catch(URISyntaxException e) {
				Log.v(TAG, "process exception: aborting; exception: " + e);
				return null;
			}
	
			/* get target directory for downloaded assets */
			File resourceDir = new File(RESOURCE_DIR);
			resourceDir.mkdirs();
			
			/* download each asset */
			for(String key : uriMap.keySet()) {
				try {
					HttpClient http = new DefaultHttpClient();
					HttpGet request = new HttpGet();
					request.setURI(uriMap.get(key));
					HttpEntity entity = http.execute(request).getEntity();
					InputStream in = entity.getContent();
					FileOutputStream out = new FileOutputStream(new File(resourceDir, filenameMap.get(key)));
					byte[] buf = new byte[1024];
					int read;
					while((read = in.read(buf)) != -1) {
						out.write(buf, 0, read);
					}
					in.close();
					out.flush();
					out.close();
				} catch(IOException e) {
					Log.v(TAG, "process exception: aborting; exception: " + e + "; resource = " + uriMap.get(key).toString());
					return null;
				}
			}
			
			/* process the commandline */
			String resourcePath = resourceDir.getAbsolutePath() + '/';
			for(String key : filenameMap.keySet()) {
				cmdline = cmdline.replace("%" + key, resourcePath + filenameMap.get(key));
			}
		}
		
		/* split the commandline at whitespace */
		return cmdline.split("\\s");		
	}
	
	private String getResourceUriHash(String id) {
		try {
			MessageDigest sha = MessageDigest.getInstance("SHA-1");
			sha.update(id.getBytes("iso-8859-1"));
			return digest2Hex(sha.digest());
		}
		catch(Exception e) {
			return null;
		}
	}

	private String digest2Hex(byte[] digest) {
		StringBuilder hash = new StringBuilder(HASH_LEN * 2);
		final String hexChars = "0123456789abcdef";
		for(int i = 0; i < HASH_LEN; i++) {
		      hash.append(hexChars.charAt((digest[i] & 0xF0) >> 4))
		         .append(hexChars.charAt((digest[i] & 0x0F)));
		}
		return hash.toString();
	}

}
