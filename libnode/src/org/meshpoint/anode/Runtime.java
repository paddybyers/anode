package org.meshpoint.anode;

import java.io.IOException;
import java.util.HashSet;
import java.util.Set;

import android.content.Context;

/**
 * Public API to obtain an manage an instance of a node.js runtime
 * There is a singleton runtime instance, with a strict lifecycle management.
 * @author paddy
 */
public class Runtime {
	
	/**************************
	 * private state
	 **************************/
	
	private int state;
	private int exitval;
	private RuntimeThread runner;
	private Set<StateListener> listeners = new HashSet<StateListener>();
	private static Runtime theRuntime;

	/**************************
	 * public API
	 **************************/
	
	/**
	 * Public factory method to get the runtime instance.
	 * There is only a single Runtime instance per application context.
	 * @param ctx the Context associated with the invoking activity,
	 * used to locate the assets
	 * @return the Runtime
	 * @throws InitialisationException if there was a problem initialising the Runtime
	 */
	public static synchronized Runtime getRuntime(Context ctx) throws InitialisationException {
		if(theRuntime == null) {
			try {
				theRuntime = new Runtime(ctx);
			} catch (IOException e) {
				throw new InitialisationException(e);			}
		}
		return theRuntime;
	}

	/**
	 * Start the Runtime instance.
	 * This is a non-blocking call.
	 * @param argv the command-line arguments to pass to the runtime
	 * @throws IllegalStateException if the runtime is already started,
	 * or is otherwise in a state that prevents it from starting
	 * @throws NodeException if the runtime exited with an error
	 */
	public void start(String[] argv) throws IllegalStateException, NodeException {
		synchronized(runner) {
			if(state != STATE_CREATED) {
				throw new IllegalStateException(
					"Attempting to start Runtime when not in CREATED state"
				);
			}
			runner.start(argv);
			try{runner.wait();}catch(InterruptedException e){}
			if(state == STATE_STOPPED)
				handleExitval();
		}
	}
	
	/**
	 * Stop a running Runtime instance.
	 * This is a non-blocking call.
	 * @throws IllegalStateException if the runtime was not previously started,
	 * or is otherwise in a state that prevents it from being stopped
	 * @throws NodeException if the runtime exited with an error
	 */
	public void stop() throws IllegalStateException, NodeException {
		synchronized(runner) {
			switch(state) {
			case STATE_STARTED:
				/* expected case, the instance is running normally */
				setState(STATE_STOPPING);
				RuntimeNative.stop(RuntimeNative.SIGABRT);
				try{runner.wait();}catch(InterruptedException e){}
			case STATE_STOPPED:
				/* already stopped, throw error if
				 * there was one, otherwise silently succeed */
				handleExitval();
				return;
			default:
				throw new IllegalStateException(
					"Attempting to stop Runtime when not in STARTED state"
				);
			}
		}
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
	 * Get the current runtime state
	 * @return
	 */
	public int getState() {
		synchronized(runner) {
			return state;
		}
	}

	/**
	 * Listener interface for being notified of state changes
	 */
	public interface StateListener {
		public void stateChanged(int state);
	}
	
	public void addStateListener(StateListener listener) {
		synchronized(listeners) {
			listeners.add(listener);
		}
	}
	
	public void removeStateListener(StateListener listener) {
		synchronized(listeners) {
			listeners.remove(listener);
		}
	}
	
	/**
	 * Exception class signifying an initialisation problem with the
	 * framework, wrapping the underlying exception cause.
	 */
	public static class InitialisationException extends Exception {
		private static final long serialVersionUID = 2496406014266651847L;
		private InitialisationException(Throwable cause) {super(cause);}
		private InitialisationException(String msg) {super(msg);}
	}

	/**
	 * Exception class signifying a lifecycle operation failed
	 * as a result of the runtime being in an inappropriate state.
	 */
	public static class IllegalStateException extends Exception {
		private static final long serialVersionUID = -8553913470826899835L;
		private IllegalStateException(String msg) {super(msg);}
	}

	/**
	 * Exception class signifying an error in the node runtime,
	 * wrapping the indicated exit code.
	 */
	public static class NodeException extends Exception {
		private static final long serialVersionUID = 5950820713527212317L;
		private NodeException(int exitval) {super("node exited with error code: " + exitval);}
	}
	
	/**************************
	 * private
	 **************************/
	
	private Runtime(Context ctx) throws IOException {
		RuntimeNative.init(ctx);
		runner = new RuntimeThread();
		state = STATE_CREATED;
	}
	
	private void setState(int state) {
		/* note that this is only ever called with the runner already locked */
		this.state = state;
		runner.notify();
		synchronized(listeners) {
			for(StateListener listener : listeners) {
				listener.stateChanged(state);
			}
		}
	}
	
	private void handleExitval() throws NodeException {
		if(exitval != 0) {
			 /* clear exitval because we have consumed that error here */
			exitval = 0;
			throw new NodeException(exitval);
		}
	}

	private class RuntimeThread extends Thread {
		private String[] argv;
		
		public void start(String[] argv) {
			this.argv = argv;
			super.start();
		}

		public void run() {
			try {
				synchronized(this) {
					Runtime.this.setState(STATE_STARTED);
				}
				int exitval = RuntimeNative.start(argv);
				synchronized(this) {
					Runtime.this.setState(STATE_STOPPED);
					Runtime.this.exitval = exitval;
				}
			} catch(Throwable t) {
				t.printStackTrace();
			}
		}
	}

}
