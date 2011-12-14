package org.meshpoint.anode.java;

import org.meshpoint.anode.idl.Types;

public class LongArray extends Array implements org.w3c.dom.LongArray {
	private long[] data;

	public LongArray(long[] data) {
		this(data, true);
	}

	public LongArray(long[] data, boolean isFixedLength) {
		super(Types.TYPE_LONG|Types.TYPE_ARRAY, isFixedLength);
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
			long[] newData = new long[length];
			System.arraycopy(data, 0, newData, 0, Math.min(length, data.length));
			data = newData;
		}
	}

	@Override
	public long getElement(int index) {
		return data[index];
	}

	@Override
	public void setElement(int index, long value) {
		if(!isFixedLength && index >= data.length)
			setLength(index + 1);
		data[index] = value;
	}

}
