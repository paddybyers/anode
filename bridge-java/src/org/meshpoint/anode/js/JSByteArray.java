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
		return (byte)((JSValue)BridgeNative.getIndexedProperty(env.getHandle(), instHandle, (Types.TYPE_BYTE|Types.TYPE_ARRAY), index)).getLongValue();
	}

	@Override
	public void setElement(int index, byte value) {
		BridgeNative.setIndexedProperty(env.getHandle(), instHandle, Types.TYPE_BYTE, index, JSValue.asJSNumber((long)value));
	}

}
