package org.w3c.dom;

public interface LongArray extends PrimitiveArray {
	public int getLength();
	public void setLength(int length);
	public long getElement(int index);
	public void setElement(int index, long value);
}
