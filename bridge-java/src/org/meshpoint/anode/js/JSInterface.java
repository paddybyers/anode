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
import org.meshpoint.anode.idl.Types;

public class JSInterface {

	/*********************
	 * private state
	 *********************/
	private static final String TAG = JSInterface.class.getCanonicalName();
	long instHandle; /* (long)Persistent<Object>* */
	protected Env env = Env.getCurrent();

	/*********************
	 * private API
	 *********************/
	protected JSInterface(long instHandle) {
		this.instHandle = instHandle;
	}

	public void release(int classId) {
		//Log.v(TAG, "Putting for finalization; this class = " + this.getClass().getName() + "; instHandle = " + instHandle + "; classId = " + classId);
		env.finalizeQueue.put(instHandle, Types.classid2Type(classId));
	}

	/*********************
	 * bridge API
	 *********************/
	protected Object __invoke(int classId, int opIdx, Object[] args) {
		if(env.isEventThread())
			return BridgeNative.invokeJSInterface(env.getHandle(), instHandle, classId, opIdx, args);
		return deferOp(OP.INVOKE, env, instHandle, classId, opIdx, null, args);
	}

	protected Object __get(int classId, int attrIdx) {
		if(env.isEventThread())
			return BridgeNative.getJSInterface(env.getHandle(), instHandle, classId, attrIdx);
		return deferOp(OP.GET, env, instHandle, classId, attrIdx, null, null);
		
	}

	protected void __set(int classId, int attrIdx, Object val) {
		if(env.isEventThread())
			BridgeNative.setJSInterface(env.getHandle(), instHandle, classId, attrIdx, val);
		deferOp(OP.SET, env, instHandle, classId, attrIdx, val, null);
	}

	private Object deferOp(OP op, Env env, long instHandle, int classId, int idx, Object val, Object[] args) {
		SyncOp syncOp = threadSyncOp.get();
		if(syncOp == null) {
			syncOp = new SyncOp();
			threadSyncOp.set(syncOp);
		}
		return syncOp.scheduleWait(op, env, instHandle, classId, idx, val, args);
	}

	/*********************
	 * SynchronousOperation
	 *********************/
	
	private enum OP {INVOKE, GET, SET};

	private static ThreadLocal<SyncOp> threadSyncOp = new ThreadLocal<SyncOp>();

	private static class SyncOp implements SynchronousOperation {

		private Env env;
		private long instHandle;
		private int classId;
		private OP op;
		private Object[] args;
		private Object ob;
		private int idx;
		private boolean isPending;
		private boolean isCancelled;

		@Override
		public void run() {
			switch(op) {
			case INVOKE:
				ob = BridgeNative.invokeJSInterface(env.getHandle(), instHandle, classId, idx, args);
				break;
			case GET:
				ob = BridgeNative.getJSInterface(env.getHandle(), instHandle, classId, idx);
				break;
			case SET:
				BridgeNative.setJSInterface(env.getHandle(), instHandle, classId, idx, ob);
				break;
			}
			isPending = false;
		}

		@Override
		public boolean isPending() {return isPending;}
		
		private Object scheduleWait(OP op, Env env, long instHandle, int classId, int idx, Object val, Object[] args) {
			this.op = op;
			this.env = env;
			this.instHandle = instHandle;
			this.classId = classId;
			this.idx = idx;
			this.ob = val;
			this.args = args;
			this.isPending = true;
			this.isCancelled = false;
			/* FIXME: consider not waiting if void */
			env.waitForOperation(this);
			return isCancelled ? null : ob;
		}

		@Override
		public void cancel() {
			isCancelled = true;
			isPending = false;
		}
	}
}
