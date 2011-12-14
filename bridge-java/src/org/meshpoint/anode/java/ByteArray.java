package org.meshpoint.anode.java;

import org.meshpoint.anode.idl.Types;

public class ByteArray extends Array implements org.w3c.dom.ByteArray {
	private byte[] data;

	public ByteArray(byte[] data) {
		this(data, true);
	}

	public ByteArray(byte[] data, boolean isFixedLength) {
		super(Types.TYPE_BYTE|Types.TYPE_ARRAY, isFixedLength);
		this.data = data;
	}

	@Override
	public int getLength() {
		return data.length;
	}

	@Override
	public void setLength(int length) {
		if(isFixedLength) throw new UnsupportedOperationException();
		if(length != data.length) {
			byte[] newData = new byte[length];
			System.arraycopy(data, 0, newData, 0, Math.min(length, data.length));
			data = newData;
		}
	}

	@Override
	public byte getElement(int index) {
		return data[index];
	}

	@Override
	public void setElement(int index, byte value) {
		if(!isFixedLength && index >= data.length)
			setLength(index + 1);
		data[index] = value;
	}

}
