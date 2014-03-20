/*
 * Copyright 2011-2012 Paddy Byers
 *
 *   Licensed under the Apache License, Version 2.0 (the "License");
 *   you may not use this file except in compliance with the License.
 *   You may obtain a copy of the License at
 *
 *       http://www.apache.org/licenses/LICENSE-2.0
 *
 *   Unless required by applicable law or agreed to in writing, software
 *   distributed under the License is distributed on an "AS IS" BASIS,
 *   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *   See the License for the specific language governing permissions and
 *   limitations under the License.
 */

package org.meshpoint.anode;

import java.util.HashSet;
import java.util.Set;

import org.meshpoint.anode.Runtime.IllegalStateException;
import org.meshpoint.anode.Runtime.NodeException;
import org.meshpoint.anode.Runtime.StateListener;
import org.meshpoint.anode.bridge.BridgeNative;

import android.content.Context;
import android.util.Log;

public class Isolate {

	private static String TAG = "anode::Isolate";

	/**************************
	 * private state
	 **************************/
	
	private final Context ctx;
	private final long handle;
	private int state;
	private int exitval;
	private final RuntimeThread runner;
	private final Set<StateListener> listeners = new HashSet<StateListener>();

	/**************************
	 * public API
	 **************************/
	
	/**
	 * Start the Isolate.
	 * This is a non-blocking call.
	 * @param argv the command-line arguments to pass to the runtime
	 * @throws IllegalStateException if the runtime is already started,
	 * or is otherwise in a state that prevents it from starting
	 * @throws NodeException if the runtime exited with an error
	 */
	public void start(String[] argv) throws IllegalStateException, NodeException {
		synchronized(runner) {
			if(state != Runtime.STATE_CREATED) {
				throw new IllegalStateException(
					"Attempting to start Runtime when not in CREATED state"
				);
			}
			runner.start(argv);
			try{runner.wait();}catch(InterruptedException e){}
			if(state == Runtime.STATE_STOPPED)
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
			case Runtime.STATE_STARTED:
				/* expected case, the instance is running normally */
				setState(Runtime.STATE_STOPPING);
				RuntimeNative.stop(handle, RuntimeNative.SIGKILL);
				try{runner.wait();}catch(InterruptedException e){}
			case Runtime.STATE_STOPPED:
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
	 * Get the current runtime state
	 * @return
	 */
	public int getState() {
		synchronized(runner) {
			return state;
		}
	}

	/**
	 * Get the exit code for this isolate if in the stopped state
	 * @return
	 * @throws IllegalStateException
	 */
	public int getExitval() throws IllegalStateException {
		synchronized(runner) {
			if(state == Runtime.STATE_STOPPED)
				return exitval;

			throw new IllegalStateException(
					"Attempting to access exit code when not in STOPPED state"
				);
		}
	}

	/**************************
	 * private
	 **************************/
	
	Isolate(Context ctx) {
		this.ctx = ctx;
		runner = new RuntimeThread();
		handle = RuntimeNative.create();
		state = Runtime.STATE_CREATED;
	}
	
	private void setState(int state) {
		Log.v(TAG, "stateState: state = " + state);
		synchronized(runner) {
			this.state = state;
			runner.notifyAll();
		}
		synchronized(listeners) {
			for(StateListener listener : listeners) {
				try {
					listener.stateChanged(state);
				} catch(Throwable t) {
					Log.e(TAG, "Unexpected exception calling state listener", t);
				}
			}
		}
	}
	
	/**
	 * Listener interface for being notified of state changes
	 */
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
				Log.v(TAG, "Isolate.run(): setting context");
				BridgeNative.setContext(ctx);
				Log.v(TAG, "Isolate.run(): set context");
				setState(Runtime.STATE_STARTED);
				exitval = RuntimeNative.start(handle, argv);
				setState(Runtime.STATE_STOPPED);
			} catch(Throwable t) {
				Log.e(TAG, "Isolate.run(): exception", t);
				exitval = 1;
				setState(Runtime.STATE_STOPPED);
			}
		}
	}

}
