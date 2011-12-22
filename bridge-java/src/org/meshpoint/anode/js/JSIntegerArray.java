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
		if(env.isEventThread()) {
			return (int)((JSValue)BridgeNative.getIndexedProperty(env.getHandle(), instHandle, Types.TYPE_INT, index)).getLongValue();
		}
		SyncOp op = deferOp(OP.GET_ELEMENT, Types.TYPE_INT, index, null);
		if(op == null) return 0;
		return (int)((JSValue)op.ob).getLongValue();
	}

	@Override
	public void setElement(int index, int value) {
		Object element = JSValue.asJSNumber((long)value);
		if(env.isEventThread())
			BridgeNative.setIndexedProperty(env.getHandle(), instHandle, Types.TYPE_INT, index, element);
		else
			deferOp(OP.SET_ELEMENT, Types.TYPE_INT, index, element);
	}

}
