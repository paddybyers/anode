package org.w3c.dom;

public interface IntegerArray extends PrimitiveArray {
	public int getLength();
	public void setLength(int length);
	public int getElement(int index);
	public void setElement(int index, int value);
}
