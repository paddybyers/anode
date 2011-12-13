package org.w3c.dom;

public interface ObjectArray<T> extends Array {
	public int getLength();
	public void setLength(int length);
	public T getElement(int index);
	public void setElement(int index, T value);
}
