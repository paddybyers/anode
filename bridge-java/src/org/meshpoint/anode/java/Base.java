package org.meshpoint.anode.java;

import org.meshpoint.node.bridge.NativeBinding;
import org.meshpoint.node.idl.IDLInterface;
import org.meshpoint.node.idl.Types.JSType;
import org.meshpoint.node.type.IInterface;

public class Base implements IInterface {

	/*********************
	 * private state
	 *********************/
	long instHandle; /* (long)Persistent<Object>* */
	IDLInterface idlInterface;

	/*********************
	 * private API
	 *********************/
	Base(IDLInterface idlInterface) {
		this.idlInterface = idlInterface;
		instHandle = NativeBinding.wrapJavaInterface(this, idlInterface);
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

	@Override
	public void release() {
		// TODO Auto-generated method stub
		
	}

}
