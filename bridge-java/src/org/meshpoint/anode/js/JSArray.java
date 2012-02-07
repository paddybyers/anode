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

package org.meshpoint.anode.js;

import org.meshpoint.anode.bridge.BridgeNative;
import org.meshpoint.anode.bridge.Env;
import org.meshpoint.anode.bridge.SynchronousOperation;
import org.w3c.dom.Array;

public class JSArray implements Array {

	/*********************
	 * private state
	 *********************/

	long instHandle; /* (long)Persistent<Object>* */
	Env env;

	/*********************
	 * private API
	 *********************/

	JSArray(long instHandle) {
		this.instHandle = instHandle;
		env = Env.getCurrent();
	}

	protected void dispose(int type) {
		env.finalizeQueue.put(instHandle, type);
	}

	/*********************
	 * public API
	 *********************/

	public int getLength() {
		if(env.isEventThread())
			return BridgeNative.getLength(env.getHandle(), instHandle);
		SyncOp op = deferOp(OP.GET_LENGTH, env, instHandle, 0, 0, null);
		return (op == null) ? 0 : op.idxOrLength;
	}

	public void setLength(int length) {
		if(env.isEventThread())
			BridgeNative.setLength(env.getHandle(), instHandle, length);
		deferOp(OP.SET_LENGTH, env, instHandle, 0, length, null);
	}

	protected SyncOp deferOp(OP op, Env env, long instHandle, int elementType, int idxOrLength, Object val) {
		SyncOp syncOp = threadSyncOp.get();
		if(syncOp == null) {
			syncOp = new SyncOp();
			threadSyncOp.set(syncOp);
		}
		return syncOp.scheduleWait(op, env, instHandle, elementType, idxOrLength, val);
	}

	/*********************
	 * SynchronousOperation
	 *********************/
	
	protected enum OP {GET_LENGTH, SET_LENGTH, GET_ELEMENT, SET_ELEMENT};

	protected static ThreadLocal<SyncOp> threadSyncOp = new ThreadLocal<SyncOp>();

	protected static class SyncOp implements SynchronousOperation {

		private OP op;
		private Env env;
		private long instHandle;
		Object ob;
		private int idxOrLength;
		private int elementType;
		private boolean isPending;
		private boolean isCancelled;

		@Override
		public void run() {
			switch(op) {
			case GET_LENGTH:
				idxOrLength = BridgeNative.getLength(env.getHandle(), instHandle);
				break;
			case SET_LENGTH:
				BridgeNative.setLength(env.getHandle(), instHandle, idxOrLength);
				break;
			case GET_ELEMENT:
				ob = BridgeNative.getIndexedProperty(env.getHandle(), instHandle, elementType, idxOrLength);
				break;
			case SET_ELEMENT:
				BridgeNative.setIndexedProperty(env.getHandle(), instHandle, elementType, idxOrLength, ob);
				break;
			}
			isPending = false;
		}

		@Override
		public boolean isPending() {return isPending;}
		
		private SyncOp scheduleWait(OP op, Env env, long instHandle, int elementType, int idxOrLength, Object val) {
			this.op = op;
			this.env = env;
			this.instHandle = instHandle;
			this.elementType = elementType;
			this.idxOrLength = idxOrLength;
			this.ob = val;
			this.isPending = true;
			this.isCancelled = false;
			/* FIXME: consider not waiting if void */
			env.waitForOperation(this);
			return isCancelled ? null : this;
		}

		@Override
		public void cancel() {
			isCancelled = true;
			isPending = false;
		}
	}
}
