package org.meshpoint.anode.js;

import org.meshpoint.anode.bridge.BridgeNative;
import org.meshpoint.anode.idl.Types;
import org.w3c.dom.DoubleArray;

public class JSDoubleArray extends JSArray implements DoubleArray {

	JSDoubleArray(long instHandle) { super(instHandle); }

	public void finalize() { super.dispose(Types.TYPE_DOUBLE|Types.TYPE_ARRAY); }

	/*********************
	 * public API
	 *********************/

	@Override
	public double getElement(int index) {
		return ((JSValue)BridgeNative.getIndexedProperty(env.getHandle(), instHandle, index)).getDoubleValue();
	}

	@Override
	public void setElement(int index, double value) {
		BridgeNative.setIndexedProperty(env.getHandle(), instHandle, index, JSValue.asJSNumber(value));
	}

}
