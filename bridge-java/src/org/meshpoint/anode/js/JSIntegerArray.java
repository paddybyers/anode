package org.meshpoint.anode.js;

import org.meshpoint.anode.bridge.BridgeNative;
import org.meshpoint.anode.idl.Types;
import org.w3c.dom.IntegerArray;

public class JSIntegerArray extends JSArray implements IntegerArray {

	JSIntegerArray(long instHandle) { super(instHandle); }
	
	public void finalize() { super.dispose(Types.TYPE_INT|Types.TYPE_ARRAY); }

	/*********************
	 * public API
	 *********************/

	@Override
	public int getElement(int index) {
		return (int)((JSValue)BridgeNative.getIndexedProperty(env.getHandle(), instHandle, index)).getLongValue();
	}

	@Override
	public void setElement(int index, int value) {
		BridgeNative.setIndexedProperty(env.getHandle(), instHandle, index, JSValue.asJSNumber((int)value));
	}

}
