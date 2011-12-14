package org.meshpoint.anode.js;

import org.meshpoint.anode.bridge.BridgeNative;
import org.meshpoint.anode.idl.Types;
import org.w3c.dom.ObjectArray;

public class JSObjectArray<T> extends JSArray implements ObjectArray<T> {
	
	JSObjectArray(long instHandle) {
		super(instHandle);
	}

	public void finalize() { super.dispose(Types.TYPE_OBJECT|Types.TYPE_ARRAY); }

	@SuppressWarnings("unchecked")
	@Override
	public T getElement(int index) {
		return (T)BridgeNative.getIndexedProperty(env.getHandle(), instHandle, index);
	}

	@Override
	public void setElement(int index, T value) {
		BridgeNative.setIndexedProperty(env.getHandle(), instHandle, index, value);
	}

}
