package org.meshpoint.anode;

import android.content.Context;
import android.util.Log;

/**
 * Wraps the native jni node.js library
 * This is a static class only
 * @author paddy
 */
final class RuntimeNative {
	
	private static String TAG = "anode::RuntimeNative";
	private static String LIBRARY_NAME = "jninode";
	
	static final int SIGINT  = 2;
	static final int SIGKILL = 9;
	static final int SIGTERM = 15;
	
	/**
	 * Initialise the native interface
	 * @param ctx the Context associated with the invoking activity,
	 * used to locate the assets
	 * @throws IOException if there was a problem accessing the native library file
	 * @throws UnsatisifiedLinkError if there was a problem initialising the native library
	 */
	static void init(Context ctx) {
		try {
			System.loadLibrary(LIBRARY_NAME);
		} catch(UnsatisfiedLinkError e) {
			Log.v(TAG, "init: unable to load library: " + e);
			throw e;
		}
	}
	
	/**
	 * Launch the node.js runtime. The thread that enters this method
	 * will block until the runtime exits.
	 * It is critical that a thread blocked in this method is not forcibly
	 * terminated by the calling application, except in the case that the
	 * entire application is about to exit, or native library resources
	 * may be leaked
	 * @param argv the options and arguments to pass to the node.js invocation
	 * @return 0 if successful, else an error code
	 */
	static native int start(String[] argv);
	
	/**
	 * Stop a running runtime. An event will be delivered to the runtime that
	 * corresponds to the action associated with the specified signal.
	 * The supported events/signals are:
	 * SIGINT:  interrupt a current blocked operation; may be caught by a handler
	 *          in the runtime 
	 * SIGTERM: request termination of the runtime; may be caught by a handler
	 *          in the runtime 
	 * SIGKILL: forcibly terminate the runtime instance 
	 * @param signum the signal number
	 * @return 0 if successful, error code otherwise
	 */
	static native int stop(int signum);
}
