package org.meshpoint.anode.js;

import org.meshpoint.anode.bridge.BridgeNative;
import org.meshpoint.anode.idl.Types;
import org.w3c.dom.LongArray;

public class JSLongArray extends JSArray implements LongArray {

	JSLongArray(long instHandle) { super(instHandle); }
	
	public void finalize() { super.dispose(Types.TYPE_LONG|Types.TYPE_ARRAY); }

	/*********************
	 * public API
	 *********************/

	@Override
	public long getElement(int index) {
		return ((JSValue)BridgeNative.getIndexedProperty(env.getHandle(), instHandle, index)).getLongValue();
	}

	@Override
	public void setElement(int index, long value) {
		BridgeNative.setIndexedProperty(env.getHandle(), instHandle, index, JSValue.asJSNumber(value));
	}

}
