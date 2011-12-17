package org.meshpoint.anode.js;

import org.meshpoint.anode.bridge.BridgeNative;
import org.meshpoint.anode.bridge.Env;
import org.meshpoint.anode.bridge.SynchronousOperation;

public class JSInterface {

	/*********************
	 * private state
	 *********************/
	long instHandle; /* (long)Persistent<Object>* */
	protected Env env = Env.getCurrent();

	/*********************
	 * private API
	 *********************/
	protected JSInterface(long instHandle) {
		this.instHandle = instHandle;
	}

	public void release(int classId) {
		env.finalizeQueue.put(instHandle, classId);
	}

	/*********************
	 * bridge API
	 *********************/
	protected Object __invoke(int classId, int opIdx, Object[] args) {
		if(env.isEventThread())
			return BridgeNative.invokeJSInterface(env.getHandle(), instHandle, classId, opIdx, args);
		return deferOp(OP.INVOKE, classId, opIdx, null, args);
	}

	protected Object __get(int classId, int attrIdx) {
		if(env.isEventThread())
			return BridgeNative.getJSInterface(env.getHandle(), instHandle, classId, attrIdx);
		return deferOp(OP.GET, classId, attrIdx, null, null);
		
	}

	protected void __set(int classId, int attrIdx, Object val) {
		if(env.isEventThread())
			BridgeNative.setJSInterface(env.getHandle(), instHandle, classId, attrIdx, val);
		deferOp(OP.SET, classId, attrIdx, val, null);
	}

	private Object deferOp(OP op, int classId, int idx, Object val, Object[] args) {
		SyncOp syncOp = threadSyncOp.get();
		if(syncOp == null) {
			syncOp = new SyncOp();
			threadSyncOp.set(syncOp);
		}
		return syncOp.scheduleWait(op, classId, idx, val, args);
	}

	/*********************
	 * SynchronousOperation
	 *********************/
	
	private enum OP {INVOKE, GET, SET};

	private static ThreadLocal<SyncOp> threadSyncOp = new ThreadLocal<SyncOp>();

	private class SyncOp implements SynchronousOperation {

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
		
		private Object scheduleWait(OP op, int classId, int idx, Object val, Object[] args) {
			this.op = op;
			this.classId = classId;
			this.idx = idx;
			this.ob = val;
			this.args = args;
			this.isPending = true;
			this.isCancelled = false;
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
