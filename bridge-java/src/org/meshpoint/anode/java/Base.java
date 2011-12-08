package org.meshpoint.anode.java;

import org.meshpoint.anode.bridge.BridgeNative;
import org.meshpoint.anode.bridge.Env;
import org.meshpoint.anode.idl.IDLInterface;
import org.meshpoint.anode.idl.Types.JSType;
import org.meshpoint.anode.type.IInterface;

public class Base implements IInterface {

	/*********************
	 * private state
	 *********************/
	long instHandle; /* (long)Persistent<Object>* */
	protected Env env = Env.getCurrent();
	private IDLInterface iface;

	/*********************
	 * private API
	 *********************/
	protected Base(IDLInterface iface) {
		this.iface = iface;
		iface.getInboundHandle();
		instHandle = BridgeNative.wrapJavaInterface(this, iface);
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
		return iface;
	}

}
