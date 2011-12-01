package org.meshpoint.anode.js;

import org.meshpoint.anode.bridge.BridgeNative;
import org.meshpoint.anode.bridge.Env;
import org.meshpoint.anode.idl.IDLInterface;
import org.meshpoint.anode.idl.Types.JSType;
import org.meshpoint.anode.type.IInterface;

public class JSInterface implements IInterface {

	/*********************
	 * private state
	 *********************/
	long instHandle; /* (long)Persistent<Object>* */
	Env env;
	IDLInterface idlInterface;

	/*********************
	 * private API
	 *********************/
	JSInterface(long instHandle, IDLInterface idlInterface) {
		this.instHandle = instHandle;
		this.idlInterface = idlInterface;
		env = Env.getCurrent();
		BridgeNative.wrapJSInterface(instHandle, this, idlInterface);
	}

	/*********************
	 * public API
	 *********************/

	@Override
	public JSType getType() {
		return JSType.OBJECT;
	}

	@Override
	public IDLInterface getDeclaredType() {
		return idlInterface;
	}

	public void finalize() {
		env.finalizeQueue.put(instHandle);
	}

	@Override
	public void release() {
		// TODO Auto-generated method stub
		
	}
}
