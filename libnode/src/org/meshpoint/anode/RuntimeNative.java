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
	
	private static String TAG = "anode::RuntimeNative";
	
	private static String RUNTIME_LIBRARY_NAME = "jninode";
	
	private static String BRIDGE_LIBRARY = "bridge.node";
	private static String BRIDGE_LIBRARY_NAME = "bridge.node";
	
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
		char sep = File.separatorChar;
		String packageName = ctx.getPackageName();
		
		// Example: `/data/data/org.mypackage.android/node_modules`
		// TODO: make the node dynamic library not depend on assumed /data/data filesystem structure
		String modulePath = sep + "data" + sep + "data" + sep + packageName + sep + "node_modules";
		
		try {
			String packageName = ctx.getPackageName();
			char sep = File.separatorChar;
			
			System.loadLibrary(RUNTIME_LIBRARY_NAME);
			
			// Example: `/data/data/org.mypackage.android/node_modules`
			// TODO: make the node dynamic library not depend on assumed /data/data filesystem structure
			String modulePath = sep + "data" + sep + "data" + sep + packageName + sep + "node_modules";
			
			extractLib(ctx, modulePath, BRIDGE_LIBRARY_NAME);
			System.load(modulePath + sep + BRIDGE_LIBRARY);
			
			Log.v(TAG, "init: loaded libraries");
			nodeInit(argv, modulePath);
		} catch(UnsatisfiedLinkError e) {
			Log.v(TAG, "init: unable to load library: " + e);
			throw e;
		}
	}
	
	/**
	 * Initialise the native node runtime
	 */
	static native void nodeInit(String[] argv, String modulePath);
	
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
	private static void extractLib(Context ctx, String path, String name) throws IOException {
		File dir, so, pkg;
		if(!(dir = new File(path)).exists())
			dir.mkdirs();
		
		if((so = new File(dir, name)).exists()) {
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
		InputStream in = ctx.getAssets().open(name);
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
