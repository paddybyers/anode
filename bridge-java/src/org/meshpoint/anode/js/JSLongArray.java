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
		if(env.isEventThread()) {
			return ((JSValue)BridgeNative.getIndexedProperty(env.getHandle(), instHandle, Types.TYPE_LONG, index)).getLongValue();
		}
		SyncOp op = deferOp(OP.GET_ELEMENT, Types.TYPE_LONG, index, null);
		if(op == null) return 0;
		return ((JSValue)op.ob).getLongValue();
	}

	@Override
	public void setElement(int index, long value) {
		Object element = JSValue.asJSNumber(value);
		if(env.isEventThread())
			BridgeNative.setIndexedProperty(env.getHandle(), instHandle, Types.TYPE_LONG, index, element);
		else
			deferOp(OP.SET_ELEMENT, Types.TYPE_LONG, index, element);
	}

}
