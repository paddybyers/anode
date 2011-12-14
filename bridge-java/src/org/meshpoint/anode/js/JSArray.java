package org.meshpoint.anode.js;

import org.meshpoint.anode.bridge.BridgeNative;
import org.meshpoint.anode.bridge.Env;
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
		return BridgeNative.getLength(env.getHandle(), instHandle);
	}

	public void setLength(int length) {
		BridgeNative.setLength(env.getHandle(), instHandle, length);
	}

}
