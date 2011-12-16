package org.meshpoint.anode.java;

import org.meshpoint.anode.idl.Types;

public class DoubleArray extends Array implements org.w3c.dom.DoubleArray {
	private double[] data;

	public DoubleArray(double[] data) {
		this(data, true);
	}

	public DoubleArray(double[] data, boolean isFixedLength) {
		super(Types.TYPE_DOUBLE|Types.TYPE_ARRAY, isFixedLength);
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
			double[] newData = new double[length];
			System.arraycopy(data, 0, newData, 0, Math.min(length, data.length));
			data = newData;
		}
	}

	@Override
	public double getElement(int index) {
		return data[index];
	}

	@Override
	public void setElement(int index, double value) {
		if(!isFixedLength && index >= data.length) {
			setLength(index + 1);
			for(int i = data.length; i < index; i++) data[i] = value;
		}
		data[index] = value;
	}

}
