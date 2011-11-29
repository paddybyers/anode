package org.meshpoint.anode.js;

import org.meshpoint.anode.bridge.BridgeNative;
import org.meshpoint.anode.idl.IDLInterface;
import org.meshpoint.anode.idl.Types.JSType;
import org.meshpoint.anode.type.IInterface;

public class JSInterface implements IInterface {

	/*********************
	 * private state
	 *********************/
	long instHandle; /* (long)Persistent<Object>* */
	IDLInterface idlInterface;

	/*********************
	 * private API
	 *********************/
	JSInterface(long instHandle, IDLInterface idlInterface) {
		this.instHandle = instHandle;
		this.idlInterface = idlInterface;
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
		idlInterface.getEnv().finalizeQueue.put(instHandle);
	}

	@Override
	public void release() {
		// TODO Auto-generated method stub
		
	}
}
