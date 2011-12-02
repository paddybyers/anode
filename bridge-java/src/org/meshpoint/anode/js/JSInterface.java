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
	protected Env env = Env.getCurrent();
	private IDLInterface iface;
	private long interfaceHandle;

	/*********************
	 * private API
	 *********************/
	protected JSInterface(long instHandle, IDLInterface iface) {
		this.instHandle = instHandle;
		this.iface = iface;
		interfaceHandle = iface.getHandle();
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

	public void finalize() {
		env.finalizeQueue.put(instHandle);
	}

	@Override
	public void release() {
		// TODO Auto-generated method stub
	}
	
	/*********************
	 * bridge API
	 *********************/
	protected Object __invoke(int opIdx, Object[] args) {
		return BridgeNative.invokeJSInterface(instHandle, interfaceHandle, opIdx, args);
	}

	protected Object __get(int attrIdx) {
		return BridgeNative.getJSInterface(instHandle, interfaceHandle, attrIdx);
	}

	protected void __set(int attrIdx, Object val) {
		BridgeNative.setJSInterface(instHandle, interfaceHandle, attrIdx, val);
	}
	
}
