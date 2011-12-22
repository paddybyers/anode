package org.meshpoint.anode.js;

import org.meshpoint.anode.bridge.BridgeNative;
import org.meshpoint.anode.idl.Types;
import org.w3c.dom.ByteArray;

public class JSByteArray extends JSArray implements ByteArray {

	JSByteArray(long instHandle) { super(instHandle);}

	public void finalize() { super.dispose(Types.TYPE_BYTE|Types.TYPE_ARRAY); }

	/*********************
	 * public API
	 *********************/

	@Override
	public byte getElement(int index) {
		if(env.isEventThread()) {
			return (byte)((JSValue)BridgeNative.getIndexedProperty(env.getHandle(), instHandle, Types.TYPE_BYTE, index)).getLongValue();
		}
		SyncOp op = deferOp(OP.GET_ELEMENT, Types.TYPE_BYTE, index, null);
		if(op == null) return 0;
		return (byte)((JSValue)op.ob).getLongValue();
	}

	@Override
	public void setElement(int index, byte value) {
		Object element = JSValue.asJSNumber((long)value);
		if(env.isEventThread())
			BridgeNative.setIndexedProperty(env.getHandle(), instHandle, Types.TYPE_BYTE, index, element);
		else
			deferOp(OP.SET_ELEMENT, Types.TYPE_BYTE, index, element);
	}

}
