package org.meshpoint.anode.java;

import org.meshpoint.anode.idl.InterfaceManager;
import org.meshpoint.anode.idl.Types;

public class ObjectArray<T> extends Array implements org.w3c.dom.ObjectArray<T> {
	private Object[] data;

	public ObjectArray(T[] data) {
		this(data, true);
	}

	public ObjectArray(T[] data, boolean isFixedLength) {
		super(0, isFixedLength);
		this.data = data;
		type = Types.fromJavaType(InterfaceManager.getInstance(), data.getClass());
	}

	@Override
	public int getLength() {
		return data.length;
	}

	@Override
	public void setLength(int length) {
		if(isFixedLength) throw new UnsupportedOperationException();
		if(length != data.length) {
			Object[] newData = new Object[length];
			System.arraycopy(data, 0, newData, 0, Math.min(length, data.length));
			data = newData;
		}
	}

	@SuppressWarnings("unchecked")
	@Override
	public T getElement(int index) {
		return (T)data[index];
	}

	@Override
	public void setElement(int index, T value) {
		if(!isFixedLength && index >= data.length) {
			setLength(index + 1);
			for(int i = data.length; i < index; i++) data[i] = value;
		}
		data[index] = value;
	}

}
