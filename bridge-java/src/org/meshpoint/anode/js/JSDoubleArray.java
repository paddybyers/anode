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
		if(env.isEventThread()) {
			return ((JSValue)BridgeNative.getIndexedProperty(env.getHandle(), instHandle, Types.TYPE_DOUBLE, index)).getDoubleValue();
		}
		SyncOp op = deferOp(OP.GET_ELEMENT, Types.TYPE_DOUBLE, index, null);
		if(op == null) return 0;
		return ((JSValue)op.ob).getDoubleValue();
	}

	@Override
	public void setElement(int index, double value) {
		Object element = JSValue.asJSNumber(value);
		if(env.isEventThread())
			BridgeNative.setIndexedProperty(env.getHandle(), instHandle, Types.TYPE_DOUBLE, index, element);
		else
			deferOp(OP.SET_ELEMENT, Types.TYPE_DOUBLE, index, element);
	}

}
