package org.meshpoint.anode;

import java.io.IOException;

import android.content.Context;

/**
 * Public API to obtain and manage Isolate instances of node.js
 * @author paddy
 */
public class Runtime {

	/**************************
	 * private state
	 **************************/
	
	private static Runtime theRuntime;
	private Context ctx;

	/**************************
	 * public API
	 **************************/
	
	/**
	 * Public factory method to initialise the runtime environment.
	 * @param ctx the Context associated with the invoking activity,
	 * used to locate the assets
	 * @throws InitialisationException if there was a problem initialising the Runtime
	 */
	public static synchronized void initRuntime(Context ctx, String[] argv) throws InitialisationException {
		if(theRuntime == null) {
			try {
				theRuntime = new Runtime(ctx, argv);
			} catch (IOException e) {
				throw new InitialisationException(e);			}
		}
	}
	
	public static boolean isInitialised() {
		return theRuntime != null;
	}

	public static Isolate createIsolate() throws IllegalStateException {
		if(theRuntime == null) {
			throw new IllegalStateException("Runtime has not beed initialised");
		}
		return new Isolate(theRuntime.ctx);
	}

	/**
	 * States
	 */
	public static final int STATE_UNINITIALISED = 0;
	public static final int STATE_CREATED       = 1;
	public static final int STATE_STARTED       = 2;
	public static final int STATE_STOPPING      = 3;
	public static final int STATE_STOPPED       = 4;

	/**
	 * Listener interface for being notified of state changes
	 */
	public interface StateListener {
		public void stateChanged(int state);
	}
	
	/**
	 * Exception class signifying an initialisation problem with the
	 * framework, wrapping the underlying exception cause.
	 */
	public static class InitialisationException extends Exception {
		private static final long serialVersionUID = 2496406014266651847L;
		InitialisationException(Throwable cause) {super(cause);}
		InitialisationException(String msg) {super(msg);}
	}

	/**
	 * Exception class signifying a lifecycle operation failed
	 * as a result of the runtime being in an inappropriate state.
	 */
	public static class IllegalStateException extends Exception {
		private static final long serialVersionUID = -8553913470826899835L;
		IllegalStateException(String msg) {super(msg);}
	}

	/**
	 * Exception class signifying an error in the node runtime,
	 * wrapping the indicated exit code.
	 */
	public static class NodeException extends Exception {
		private static final long serialVersionUID = 5950820713527212317L;
		NodeException(int exitval) {super("node exited with error code: " + exitval);}
	}
	
	/**************************
	 * private
	 **************************/
	
	Runtime(Context ctx, String[] argv) throws IOException {
		this.ctx = ctx;
		RuntimeNative.init(ctx, argv);
	}
	
	public void finalize() {
		RuntimeNative.nodeDispose();
	}
}
