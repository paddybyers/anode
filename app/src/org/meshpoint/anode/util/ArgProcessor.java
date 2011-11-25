package org.meshpoint.anode.util;

import java.io.IOException;
import java.net.URI;
import java.net.URISyntaxException;
import java.util.HashMap;
import java.util.Map;
import java.util.Set;

import org.meshpoint.anode.Constants;

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

	private Bundle extras;
	private String text;
	private Map<String, URI> uriMap;
	private Map<String, String> filenameMap;

	/**
	 * Constructs an instance of ArgProcessor
	 * @param extras an optional bundle containing the mapping parameters
	 * @param text the string to process
	 */
	public ArgProcessor(Bundle extras, String text) {
		this.extras = extras;
		this.text = text;
		uriMap = new HashMap<String, URI>();
		filenameMap = new HashMap<String, String>();
	}
	
	/**
	 * Process the text string
	 * @return the processed string, with downloaded resource names substituted
	 */
	public String processString() {
		if(extras != null) {
			/* extract list of args to get */
			try {
				Set<String> keys = extras.keySet();
				for(String key : keys) {
					if(key.startsWith(GET_PREFIX)) {
						String rawUri = extras.getString(key);
						String rawKey = key.substring(GET_PREFIX.length());
						URI uri = new URI(rawUri);
						String filename = ModuleUtils.getResourceUriHash(rawUri);
						uriMap.put(rawKey, uri);
						filenameMap.put(rawKey, filename);
					}
				}
			} catch(URISyntaxException e) {
				Log.v(TAG, "process exception: aborting; exception: " + e);
				return null;
			}
	
			/* download each asset */
			for(String key : uriMap.keySet()) {
				try {
					ModuleUtils.getResource(uriMap.get(key), filenameMap.get(key));
				} catch(IOException e) {
					Log.v(TAG, "process exception: aborting; exception: " + e + "; resource = " + uriMap.get(key).toString());
					return null;
				}
			}
			
			/* process the commandline */
			for(String key : filenameMap.keySet()) {
				text = text.replace("%" + key, Constants.RESOURCE_DIR + '/' + filenameMap.get(key));
			}
		}
		return text;
	}

	public String[] processArray() {
		/* split the commandline at whitespace */
		processString();
		return text.split("\\s");		
	}
	
}
