package org.meshpoint.anode;

import java.io.File;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStream;

import android.content.Context;
import android.util.Log;

/**
 * Wraps the native jni node.js library
 * This is a static class only
 * @author paddy
 */
final class RuntimeNative {
	
	private static String PACKAGE_NAME = "org.meshpoint.anode";
	private static String TAG = "anode::RuntimeNative";
	private static String LIBRARY_NAME = "jninode";
	private static String LIBRARY_FILE = "libjninode.so";
	private static String LIBRARY_PATH = "/data/data/" + PACKAGE_NAME + "/app";
	
	static final int SIGINT  = 2;
	static final int SIGABRT = 6;
	static final int SIGKILL = 9;
	static final int SIGTERM = 15;
	
	/**
	 * Initialise the native interface
	 * @param ctx the Context associated with the invoking activity,
	 * used to locate the assets
	 * @throws IOException if there was a problem accessing the native library file
	 * @throws UnsatisifiedLinkError if there was a problem initialising the native library
	 */
	static void init(Context ctx, String[] argv) throws IOException {
		try {
			extractLib(ctx);
			System.load(LIBRARY_PATH + '/' + LIBRARY_FILE);
			nodeInit(argv);
		} catch(UnsatisfiedLinkError e) {
			Log.v(TAG, "init: unable to load library: " + e);
			throw e;
		} catch (IOException e) {
			Log.v(TAG, "init: unable to write library to file: " + e);
			throw e;
		}
	}
	
	/**
	 * Initialise the native node runtime
	 */
	static native void nodeInit(String[] argv);
	
	/**
	 * Dispose the native node runtime
	 */
	static native void nodeDispose();
	
	/**
	 * Create a node.js isolate
	 * @return isolate handle, or 0 if error
	 */
	static native long create();
	
	/**
	 * Launch the node.js runtime. The thread that enters this method
	 * will block until the runtime exits.
	 * It is critical that a thread blocked in this method is not forcibly
	 * terminated by the calling application, except in the case that the
	 * entire application is about to exit, or native library resources
	 * may be leaked
	 * @param isolate the isolate handle
	 * @param argv the options and arguments to pass to the node.js invocation
	 * @return 0 if successful, else an error code
	 */
	static native int start(long isolate, String[] argv);
	
	/**
	 * Stop a running runtime. An event will be delivered to the runtime that
	 * simulates the delivery of the specified signal.
	 * The supported events/signals are:
	 * SIGINT:  interrupt a current blocked operation; may be caught by a handler
	 *          in the runtime 
	 * SIGTERM: request termination of the runtime; may be caught by a handler
	 *          in the runtime 
	 * SIGKILL: request termination of the runtime; may be caught by a handler
	 *          in the runtime 
	 * SIGABRT: forcibly terminate the runtime instance 
	 * @param isolate the isolate handle
	 * @param signum the signal number
	 * @return 0 if successful, error code otherwise
	 */
	static native int stop(long isolate, int signum);
	
	/**
	 * Dispose a native isolate instance
	 */
	static native void isolateDispose(long isolate);
	
	/**
	 * Extract the library from assets to the default library location.
	 * @throws IOException 
	 */
	private static void extractLib(Context ctx) throws IOException {
		File dir, so, pkg;
		if(!(dir = new File(LIBRARY_PATH)).exists())
			dir.mkdirs();
		
		if((so = new File(dir, LIBRARY_FILE)).exists()) {
			/* check to see if this timestamp pre-dates
			 * the current package */
			if((pkg = new File(ctx.getPackageResourcePath())).exists()) {
				if(pkg.lastModified() < so.lastModified()) {
					Log.v(TAG, "extractLib: library up to date");
					return;
				}
			}
			Log.v(TAG, "extractLib: library present but out of date");
			so.delete();
		}
		Log.v(TAG, "extractLib: copying library");
		InputStream in = ctx.getAssets().open(LIBRARY_FILE);
		FileOutputStream out = new FileOutputStream(so);
		int read;
		byte[] buf = new byte[8192];
		while((read = in.read(buf)) != -1)
				out.write(buf, 0, read);
		in.close();
		out.flush();
		out.close();
		so.setExecutable(true);
	}
}
