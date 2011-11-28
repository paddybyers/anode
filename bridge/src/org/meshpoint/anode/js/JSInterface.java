package org.meshpoint.anode.js;

import org.meshpoint.node.bridge.NativeBinding;
import org.meshpoint.node.idl.IDLInterface;
import org.meshpoint.node.idl.Types.JSType;
import org.meshpoint.node.type.IInterface;

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
		NativeBinding.wrapJSInterface(instHandle, this, idlInterface);
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
