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

package org.meshpoint.anode.bridge;

import android.util.Log;


public class FinalizeQueue implements SynchronousOperation {
	
	/********************
	 * private state
	 ********************/
	private final String TAG = FinalizeQueue.class.getCanonicalName();
	private Env env;
	private static final int QUEUE_LENGTH = 1024;
	private long[] handleBuffer = new long[QUEUE_LENGTH];
	private int[] typeBuffer = new int[QUEUE_LENGTH];
	private int count;
	
	/********************
	 * public API
	 *******************/
	
	public FinalizeQueue(Env env) {
		this.env = env;
	}
	
	/**
	 * Appends a new item to the queue; may be called
	 * from any thread
	 * @param h the handle
	 */
	public synchronized void put(long h, int type) {
		if(count == QUEUE_LENGTH)
			env.waitForOperation(this);
		if(count == QUEUE_LENGTH)
			throw new RuntimeException("Fatal error processing FinalizeQueue");
		handleBuffer[count] = h;
		typeBuffer[count++] = type;
	}

	/**
	 * Removes and releases all items currently queued
	 */
	@Override
	public synchronized void run() {
		for(int i = 0; i < count; i++) {
			//Log.v(TAG, "Finalizing; instHandle = " + handleBuffer[i] + "; type = " + typeBuffer[i]);
			BridgeNative.releaseObjectHandle(env.envHandle, handleBuffer[i], typeBuffer[i]);
		}
		count = 0;
	}

	@Override
	public synchronized boolean isPending() {
		return count > 0;
	}

	@Override
	public void cancel() {}
}
