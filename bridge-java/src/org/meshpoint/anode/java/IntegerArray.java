package org.meshpoint.anode.java;

import org.meshpoint.anode.idl.Types;

public class IntegerArray extends Array implements org.w3c.dom.IntegerArray {
	private int[] data;

	public IntegerArray(int[] data) {
		this(data, true);
	}

	public IntegerArray(int[] data, boolean isFixedLength) {
		super(Types.TYPE_INT|Types.TYPE_ARRAY, isFixedLength);
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
			int[] newData = new int[length];
			System.arraycopy(data, 0, newData, 0, Math.min(length, data.length));
			data = newData;
		}
	}

	@Override
	public int getElement(int index) {
		return data[index];
	}

	@Override
	public void setElement(int index, int value) {
		if(!isFixedLength && index >= data.length) {
			setLength(index + 1);
			for(int i = data.length; i < index; i++) data[i] = value;
		}
		data[index] = value;
	}

}
